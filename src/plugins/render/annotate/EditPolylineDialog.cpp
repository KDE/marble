//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
//

// Self
#include "EditPolylineDialog.h"
#include "ui_EditPolylineDialog.h"

// Qt
#include <QColorDialog>
#include <QMessageBox>

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"
#include "GeoDataStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataTypes.h"
#include "NodeModel.h"
#include "FormattedTextWidget.h"
#include "NodeItemDelegate.h"
#include "StyleBuilder.h"
#include "osm/OsmTagEditorWidget.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmRelationManagerWidget.h"


namespace Marble
{

class Q_DECL_HIDDEN EditPolylineDialog::Private : public Ui::UiEditPolylineDialog
{
public:
    Private( GeoDataPlacemark *placemark);
    ~Private();

    QColorDialog *m_linesDialog;
    OsmTagEditorWidget *m_osmTagEditorWidget;
    OsmRelationManagerWidget *m_osmRelationManagerWidget;
    GeoDataPlacemark *m_placemark;

    // Used to restore if the Cancel button is pressed.
    QString m_initialName;
    QString m_initialDescription;
    GeoDataLineStyle m_initialLineStyle;
    GeoDataLineString m_initialLineString;
    OsmPlacemarkData m_initialOsmData;
    bool m_hadInitialOsmData;

    NodeItemDelegate *m_delegate;
    NodeModel *m_nodeModel;
};

EditPolylineDialog::Private::Private( GeoDataPlacemark *placemark ) :
    Ui::UiEditPolylineDialog(),
    m_linesDialog( 0 ),
    m_osmTagEditorWidget( 0 ),
    m_osmRelationManagerWidget( 0 ),
    m_placemark( placemark ),
    m_nodeModel( new NodeModel )
{
    // nothing to do
}

EditPolylineDialog::Private::~Private()
{
    delete m_linesDialog;
    delete m_nodeModel;
    delete m_delegate;
}

EditPolylineDialog::EditPolylineDialog( GeoDataPlacemark *placemark,
                                        const QHash<qint64, OsmPlacemarkData> *relations,
                                        QWidget *parent ) :
    QDialog( parent ) ,
    d ( new Private( placemark ) )
{
    d->setupUi( this );

    // There's no point showing Relations and Tags tabs if the editor was not
    // loaded from the annotate plugin ( loaded from tourWidget.. )
    if ( relations ) {
        // Adding the osm tag editor widget tab
        d->m_osmTagEditorWidget = new OsmTagEditorWidget( placemark, this );
        d->tabWidget->addTab( d->m_osmTagEditorWidget, tr( "Tags" ) );
        QObject::connect( d->m_osmTagEditorWidget, SIGNAL( placemarkChanged( GeoDataFeature* ) ),
                          this, SLOT( updatePolyline() ) );

        // Adding the osm relation editor widget tab
        d->m_osmRelationManagerWidget = new OsmRelationManagerWidget( placemark, relations, this );
        d->tabWidget->addTab( d->m_osmRelationManagerWidget, tr( "Relations" ) );
        QObject::connect( d->m_osmRelationManagerWidget, SIGNAL( relationCreated( const OsmPlacemarkData& ) ),
                          this, SIGNAL( relationCreated( const OsmPlacemarkData& ) ) );

        adjustSize();
    }

    d->m_hadInitialOsmData = placemark->hasOsmData();
    if ( d->m_hadInitialOsmData ) {
        d->m_initialOsmData = placemark->osmData();
    }

    // If the polygon has just been drawn, assign it a default name.
    if ( d->m_placemark->name().isNull() ) {
        d->m_placemark->setName( tr("Untitled Path") );
    }

    d->m_initialLineString = *(static_cast<GeoDataLineString*>( placemark->geometry() ) );
    d->m_name->setText( placemark->name() );
    d->m_initialName = d->m_name->text();
    connect( d->m_name, SIGNAL(editingFinished()), this, SLOT(updatePolyline()) );

    d->m_formattedTextWidget->setText( placemark->description() );
    d->m_initialDescription = d->m_formattedTextWidget->text();

    d->m_linesWidth->setRange( 0.1, 5.0 );


    // Get the current style properties.
    const GeoDataLineStyle lineStyle = placemark->style()->lineStyle();
    d->m_initialLineStyle = lineStyle;

    d->m_linesWidth->setValue( lineStyle.width() );
    connect( d->m_linesWidth, SIGNAL(valueChanged(double)), this, SLOT(handleChangingStyle()) );

    // Adjust the color button's icon to the current lines color.
    QPixmap linesPixmap( d->m_linesColorButton->iconSize() );
    linesPixmap.fill( lineStyle.color() );
    d->m_linesColorButton->setIcon( QIcon( linesPixmap ) );

    // Setting the NodeView's delegate: mainly used for the editing the polyline's nodes
    d->m_delegate = new NodeItemDelegate( d->m_placemark, d->m_nodeView );

    connect( d->m_delegate, SIGNAL(modelChanged(GeoDataPlacemark*)),
             this, SLOT(handleItemMoving(GeoDataPlacemark*)) );
    connect( d->m_delegate, SIGNAL(geometryChanged()),
             this, SLOT(updatePolyline()) );

    d->m_nodeView->setItemDelegate( d->m_delegate );
    d->m_nodeView->setEditTriggers( QAbstractItemView::AllEditTriggers );

    // Setup the color dialogs.
    d->m_linesDialog = new QColorDialog( this );
    d->m_linesDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_linesDialog->setCurrentColor( lineStyle.color() );
    connect( d->m_linesColorButton, SIGNAL(clicked()), d->m_linesDialog, SLOT(exec()) );
    connect( d->m_linesDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateLinesDialog(QColor)) );
    connect( d->m_linesDialog, SIGNAL(colorSelected(QColor)), this, SLOT(handleChangingStyle()) );

    if( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
        GeoDataLineString *lineString = static_cast<GeoDataLineString*>( placemark->geometry() );
        for( int i = 0; i < lineString->size(); ++i ) {
            d->m_nodeModel->addNode( lineString->at( i ) );
        }
    }
    d->m_nodeView->setModel( d->m_nodeModel );

    // Resize column to contents size for better UI.
    d->m_nodeView->resizeColumnToContents( 0 );

    // Promote "Ok" button to default button.
    d->buttonBox->button( QDialogButtonBox::Ok )->setDefault( true );

    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(pressed()), this, SLOT(checkFields()) );
    connect( this, SIGNAL(accepted()), SLOT(updatePolyline()) );
    connect( this, SIGNAL(finished(int)), SLOT(restoreInitial(int)) );

    // Ensure that the dialog gets deleted when closing it (either when clicking OK or
    // Close).
    connect( this, SIGNAL(finished(int)), SLOT(deleteLater()) );
}

EditPolylineDialog::~EditPolylineDialog()
{
    delete d;
}

void EditPolylineDialog::handleAddingNode( const GeoDataCoordinates &node )
{
    d->m_nodeModel->addNode( node );
}

void EditPolylineDialog::handleItemMoving( GeoDataPlacemark *item )
{
    if( item == d->m_placemark ) {
        d->m_nodeModel->clear();
        if( d->m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
            GeoDataLineString *lineString = static_cast<GeoDataLineString*>( d->m_placemark->geometry() );
            for( int i = 0; i < lineString->size(); ++i ) {
                d->m_nodeModel->addNode( lineString->at( i ) );
            }
        }
    }
}

void EditPolylineDialog::handleChangingStyle()
{
    // The default style has been changed, thus the old style URL is no longer valid
    // The polyline is now considered to have a customStyle
    d->m_placemark->setStyleUrl(QString());

    GeoDataStyle::Ptr newStyle(new GeoDataStyle( *d->m_placemark->style() ));
    newStyle->lineStyle().setColor( d->m_linesDialog->currentColor() );
    newStyle->lineStyle().setWidth( d->m_linesWidth->value() );
    newStyle->setId(d->m_placemark->id() + QLatin1String("Style"));
    d->m_placemark->setStyle( newStyle );

    updatePolyline();
}

void EditPolylineDialog::updatePolyline()
{
    d->m_placemark->setDescription( d->m_formattedTextWidget->text() );
    d->m_placemark->setName( d->m_name->text() );

    // If there is no custom style initialized( default #polyline url is used ) and there is a osmTag-based style
    // available, set it
    const OsmPlacemarkData osmData = d->m_osmTagEditorWidget->placemarkData();
    const GeoDataPlacemark::GeoDataVisualCategory category = StyleBuilder::determineVisualCategory(osmData);
    if (d->m_placemark->styleUrl() == QLatin1String("#polyline") && category != GeoDataPlacemark::None) {
        d->m_placemark->setStyle( GeoDataStyle::Ptr() ); // first clear style so style gets set by setVisualCategory()
        d->m_placemark->setVisualCategory( category );
    }

    emit polylineUpdated( d->m_placemark );
}


void EditPolylineDialog::updateLinesDialog( const QColor &color )
{
    QPixmap linesPixmap( d->m_linesColorButton->iconSize().width(),
                         d->m_linesColorButton->iconSize().height() );
    linesPixmap.fill( color );
    d->m_linesColorButton->setIcon( QIcon( linesPixmap ) );
}

void EditPolylineDialog::restoreInitial( int result )
{
    if ( result ) {
        return;
    }

    GeoDataLineString* currentLineString = static_cast<GeoDataLineString*>( d->m_placemark->geometry() );

    if( *currentLineString != d->m_initialLineString ) {
        d->m_placemark->setGeometry( new GeoDataLineString( d->m_initialLineString ) );
    }

    if ( d->m_placemark->name() != d->m_initialName ) {
        d->m_placemark->setName( d->m_initialName );
    }

    if ( d->m_placemark->description() != d->m_initialDescription ) {
        d->m_placemark->setDescription( d->m_initialDescription );
    }

    if ( d->m_placemark->style()->lineStyle() != d->m_initialLineStyle ) {
        GeoDataStyle::Ptr newStyle(new GeoDataStyle( *d->m_placemark->style() ));
        newStyle->setLineStyle( d->m_initialLineStyle );
        d->m_placemark->setStyle( newStyle );
    }

    if( d->m_hadInitialOsmData ) {
        d->m_placemark->setOsmData( d->m_initialOsmData );
    }

    emit polylineUpdated( d->m_placemark );
}

void EditPolylineDialog::checkFields()
{
    bool ok = true;
    if ( d->m_name->text().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No name specified" ),
                              tr( "Please specify a name for this polyline." ) );
        ok = false;
    } else {
        if ( d->m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
            GeoDataLineString *lineString = static_cast<GeoDataLineString*>( d->m_placemark->geometry() );
            if( lineString->size() < 2 ) {
                QMessageBox::warning( this,
                                      tr( "Not enough nodes specified." ),
                                      tr( "Please specify at least 2 nodes for the path by clicking on the map." ) );
                ok = false;
            }
        }
    }
    if( ok ) {
        accept();
    }
}

}

#include "moc_EditPolylineDialog.cpp"
