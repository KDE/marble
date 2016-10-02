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
#include "EditPolygonDialog.h"
#include "ui_EditPolygonDialog.h"

// Qt
#include <QColorDialog>
#include <QMessageBox>

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataTypes.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "NodeModel.h"
#include "NodeItemDelegate.h"
#include "FormattedTextWidget.h"
#include "StyleBuilder.h"
#include "osm/OsmTagEditorWidget.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmRelationManagerWidget.h"

namespace Marble {

class Q_DECL_HIDDEN EditPolygonDialog::Private : public Ui::UiEditPolygonDialog
{
public:
    Private( GeoDataPlacemark *placemark );
    ~Private();

    GeoDataPlacemark *m_placemark;

    QColorDialog *m_linesDialog;
    QColorDialog *m_polyDialog;

    QString m_initialDescription;
    QString m_initialName;
    GeoDataStyle m_initialStyle;
    GeoDataLinearRing m_initialOuterBoundary;
    OsmPlacemarkData m_initialOsmData;
    bool m_hadInitialOsmData;

    NodeModel *m_nodeModel;
    NodeItemDelegate *m_delegate;
    OsmTagEditorWidget *m_osmTagEditorWidget;
    OsmRelationManagerWidget *m_osmRelationManagerWidget;

};

EditPolygonDialog::Private::Private( GeoDataPlacemark *placemark ) :
    Ui::UiEditPolygonDialog(),
    m_placemark( placemark ),
    m_linesDialog( 0 ),
    m_polyDialog( 0 ),
    m_nodeModel( new NodeModel ),
    m_osmTagEditorWidget( 0 ),
    m_osmRelationManagerWidget( 0 )
{
    // nothing to do
}

EditPolygonDialog::Private::~Private()
{
    delete m_linesDialog;
    delete m_polyDialog;
    delete m_nodeModel;
    delete m_delegate;
}

EditPolygonDialog::EditPolygonDialog( GeoDataPlacemark *placemark,
                                      const QHash<qint64, OsmPlacemarkData> *relations,
                                      QWidget *parent ) :
    QDialog( parent ),
    d( new Private( placemark ) )
{
    d->setupUi( this );

    // There's no point showing Relations and Tags tabs if the editor was not
    // loaded from the annotate plugin ( loaded from tourWidget.. )
    if ( relations ) {
        // Adding the osm tag editor widget tab
        d->m_osmTagEditorWidget = new OsmTagEditorWidget( placemark, this );
        d->tabWidget->addTab( d->m_osmTagEditorWidget, tr( "Tags" ) );
        QObject::connect( d->m_osmTagEditorWidget, SIGNAL( placemarkChanged( GeoDataFeature* ) ),
                          this, SLOT( updatePolygon() ) );

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

    d->m_initialStyle = *placemark->style();

    // If the polygon has just been drawn, assign it a default name.
    if ( d->m_placemark->name().isNull() ) {
        d->m_placemark->setName( tr("Untitled Polygon") );
    }
    d->m_name->setText( placemark->name() );
    d->m_initialName = placemark->name();
    connect( d->m_name, SIGNAL(editingFinished()), this, SLOT(updatePolygon()) );

    d->m_formattedTextWidget->setText( placemark->description() );
    d->m_initialDescription = placemark->description();

    // Get the current style properties.
    const GeoDataLineStyle lineStyle = placemark->style()->lineStyle();
    const GeoDataPolyStyle polyStyle = placemark->style()->polyStyle();

    d->m_linesWidth->setRange( 0.1, 5.0 );
    d->m_linesWidth->setValue( lineStyle.width() );
    connect( d->m_linesWidth, SIGNAL(valueChanged(double)), this, SLOT( handleChangingStyle() ) );

    // Adjust the "Filled"/"Not Filled" option according to its current fill.
    if ( polyStyle.fill() ) {
        d->m_filledColor->setCurrentIndex( 0 );
    } else {
        d->m_filledColor->setCurrentIndex( 1 );
    }
    connect( d->m_filledColor, SIGNAL(currentIndexChanged(int)), this, SLOT( handleChangingStyle() ) );

    // Adjust the color buttons' icons to the current lines and polygon colors.
    QPixmap linesPixmap( d->m_linesColorButton->iconSize().width(),
                         d->m_linesColorButton->iconSize().height() );
    linesPixmap.fill( lineStyle.color() );
    d->m_linesColorButton->setIcon( QIcon( linesPixmap ) );

    QPixmap polyPixmap( d->m_polyColorButton->iconSize().width(),
                        d->m_polyColorButton->iconSize().height() );
    polyPixmap.fill( polyStyle.color() );
    d->m_polyColorButton->setIcon( QIcon( polyPixmap ) );

    // Setup the color dialogs.
    d->m_linesDialog = new QColorDialog( this );
    d->m_linesDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_linesDialog->setCurrentColor( lineStyle.color() );
    connect( d->m_linesColorButton, SIGNAL(clicked()), d->m_linesDialog, SLOT(exec()) );
    connect( d->m_linesDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateLinesDialog(QColor)) );
    connect( d->m_linesDialog, SIGNAL(colorSelected(QColor)), this, SLOT( handleChangingStyle() ) );

    d->m_polyDialog = new QColorDialog( this );
    d->m_polyDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_polyDialog->setCurrentColor( polyStyle.color() );
    connect( d->m_polyColorButton, SIGNAL(clicked()), d->m_polyDialog, SLOT(exec()) );
    connect( d->m_polyDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updatePolyDialog(QColor)) );
    connect( d->m_polyDialog, SIGNAL(colorSelected(QColor)), this, SLOT( handleChangingStyle() ) );

    // Setting the NodeView's delegate: mainly used for the editing the polygon's nodes
    d->m_delegate = new NodeItemDelegate( d->m_placemark, d->m_nodeView );

    connect( d->m_delegate, SIGNAL(modelChanged(GeoDataPlacemark*)),
             this, SLOT(handleItemMoving(GeoDataPlacemark*)) );
    connect( d->m_delegate, SIGNAL(geometryChanged()),
             this, SLOT(updatePolygon()) );

    d->m_nodeView->setItemDelegate( d->m_delegate );
    d->m_nodeView->setEditTriggers( QAbstractItemView::AllEditTriggers );

    // Populating the model
    if( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark->geometry() );
        GeoDataLinearRing outerBoundary = polygon->outerBoundary();
        for( int i = 0; i < outerBoundary.size(); ++i ) {
            d->m_nodeModel->addNode( outerBoundary.at( i ) );
        }
        d->m_initialOuterBoundary = outerBoundary;
    }

    d->m_nodeView->setModel( d->m_nodeModel );

    // Resize column to contents size for better UI.
    d->m_nodeView->resizeColumnToContents( 0 );

    // Promote "Ok" button to default button.
    d->buttonBox->button( QDialogButtonBox::Ok )->setDefault( true );

    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(pressed()), this, SLOT(checkFields()) );
    connect( this, SIGNAL(accepted()), SLOT(updatePolygon()) );
    connect( this, SIGNAL(finished(int)), SLOT(restoreInitial(int)) );

    // Ensure that the dialog gets deleted when closing it (either when clicking OK or
    // Close).
    connect( this, SIGNAL(finished(int)), SLOT(deleteLater()) );
}

EditPolygonDialog::~EditPolygonDialog()
{
    delete d;
}

void EditPolygonDialog::handleAddingNode( const GeoDataCoordinates &node )
{
    d->m_nodeModel->addNode( node );
}

void EditPolygonDialog::handleItemMoving( GeoDataPlacemark *item )
{
    if( item == d->m_placemark ) {
        d->m_nodeModel->clear();
        if( d->m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( d->m_placemark->geometry() );
            GeoDataLinearRing outerBoundary = polygon->outerBoundary();
            for( int i = 0; i < outerBoundary.size(); ++i ) {
                d->m_nodeModel->addNode( outerBoundary.at( i ) );
            }
        }
    }
}

void EditPolygonDialog::handleChangingStyle()
{

    // The default style of the polygon has been changed, thus the old style URL is no longer valid
    d->m_placemark->setStyleUrl(QString());

    GeoDataStyle::Ptr style(new GeoDataStyle( *d->m_placemark->style() ));
    style->lineStyle().setWidth( d->m_linesWidth->value() );
    // 0 corresponds to "Filled" and 1 corresponds to "Not Filled".
    style->polyStyle().setFill( !d->m_filledColor->currentIndex() );
    style->setId(d->m_placemark->id() + QLatin1String("Style"));


    // Adjust the lines/polygon colors.
    // QColorDialog::currentColor() also works even if the color dialog
    // has not been exec'ed, while QColorDialog::selectedColor() does not.
    style->lineStyle().setColor( d->m_linesDialog->currentColor() );
    style->polyStyle().setColor( d->m_polyDialog->currentColor() );

    d->m_placemark->setStyle( style );

    updatePolygon();
}

void EditPolygonDialog::updatePolygon()
{
    d->m_placemark->setName( d->m_name->text() );
    d->m_placemark->setDescription( d->m_formattedTextWidget->text() );

    // If there is not custom style initialized( default #polyline url is used ) and there is a osmTag-based style
    // available, set it
    const OsmPlacemarkData osmData = d->m_osmTagEditorWidget->placemarkData();
    const GeoDataPlacemark::GeoDataVisualCategory category = StyleBuilder::determineVisualCategory(osmData);
    if (d->m_placemark->styleUrl() == QLatin1String("#polygon") && category != GeoDataPlacemark::None) {
        d->m_placemark->setStyle( GeoDataStyle::Ptr() ); // first clear style so style gets set by setVisualCategory()
        d->m_placemark->setVisualCategory( category );
    }

    emit polygonUpdated( d->m_placemark );
}

void EditPolygonDialog::updateLinesDialog( const QColor &color )
{
    QPixmap linesPixmap( d->m_linesColorButton->iconSize().width(),
                         d->m_linesColorButton->iconSize().height() );
    linesPixmap.fill( color );
    d->m_linesColorButton->setIcon( QIcon( linesPixmap ) );
}

void EditPolygonDialog::updatePolyDialog( const QColor &color )
{
    QPixmap polyPixmap( d->m_polyColorButton->iconSize().width(),
                        d->m_polyColorButton->iconSize().height() );
    polyPixmap.fill( color );
    d->m_polyColorButton->setIcon( QIcon( polyPixmap ) );
}

void EditPolygonDialog::checkFields()
{
    bool ok = true;
    if ( d->m_name->text().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No name specified" ),
                              tr( "Please specify a name for this polygon." ) );
        ok = false;
    } else {
        if ( d->m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( d->m_placemark->geometry() );
            if( polygon->outerBoundary().size() < 3 ) {
                QMessageBox::warning( this,
                                      tr( "Not enough nodes specified." ),
                                      tr( "Please specify at least 3 nodes for the polygon by clicking on the map." ) );
                ok = false;
            }
        }
    }
    if( ok ) {
        accept();
    }
}

void EditPolygonDialog::restoreInitial( int result )
{
    if ( result ) {
        return;
    }

    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( d->m_placemark->geometry() );
    GeoDataLinearRing outerBoundary = polygon->outerBoundary();

    if ( outerBoundary != d->m_initialOuterBoundary ) {
        polygon->setOuterBoundary( d->m_initialOuterBoundary );
    }

    if ( d->m_placemark->name() != d->m_initialName ) {
        d->m_placemark->setName( d->m_initialName );
    }

    if ( d->m_placemark->description() != d->m_initialDescription ) {
        d->m_placemark->setDescription( d->m_initialDescription );
    }

    if ( *d->m_placemark->style() != d->m_initialStyle ) {
        d->m_placemark->setStyle( GeoDataStyle::Ptr(new GeoDataStyle( d->m_initialStyle )) );
    }

    if( d->m_hadInitialOsmData ) {
        d->m_placemark->setOsmData( d->m_initialOsmData );
    }

    emit polygonUpdated( d->m_placemark );
}

}
#include "moc_EditPolygonDialog.cpp"
