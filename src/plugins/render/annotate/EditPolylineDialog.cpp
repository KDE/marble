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
#include "GeoDataStyle.h"
#include "GeoDataTypes.h"
#include "NodeModel.h"


namespace Marble
{

class EditPolylineDialog::Private : public Ui::UiEditPolylineDialog
{
public:
    Private( GeoDataPlacemark *placemark);
    ~Private();

    // Used to tell whether the settings before showing the dialog should be restored on
    // pressing the 'Cancel' button or not.
    bool m_firstEditing;

    QColorDialog *m_linesDialog;
    GeoDataPlacemark *m_placemark;

    // Used to restore if the Cancel button is pressed.
    QString m_initialName;
    QString m_initialDescription;
    GeoDataLineStyle m_initialLineStyle;

    NodeModel *m_nodeModel;
};

EditPolylineDialog::Private::Private( GeoDataPlacemark *placemark ) :
    Ui::UiEditPolylineDialog(),
    m_firstEditing( false ),
    m_linesDialog( 0 ),
    m_placemark( placemark ),
    m_nodeModel( new NodeModel )
{
    // nothing to do
}

EditPolylineDialog::Private::~Private()
{
    delete m_linesDialog;
    delete m_nodeModel;
}

EditPolylineDialog::EditPolylineDialog( GeoDataPlacemark *placemark, QWidget *parent ) :
    QDialog( parent ) ,
    d ( new Private( placemark ) )
{
    d->setupUi( this );

    // If the polygon has just been drawn, assign it a default name.
    if ( d->m_placemark->name().isNull() ) {
        d->m_placemark->setName( tr("Untitled Path") );
    }


    d->m_name->setText( placemark->name() );
    d->m_initialName = d->m_name->text();
    connect( d->m_name, SIGNAL(editingFinished()), this, SLOT(updatePolyline()) );

    d->m_description->setText( placemark->description() );
    d->m_initialDescription = d->m_description->toPlainText();

    d->m_linesWidth->setRange( 0.1, 5.0 );


    // Get the current style properties.
    const GeoDataLineStyle lineStyle = placemark->style()->lineStyle();
    d->m_initialLineStyle = lineStyle;

    d->m_linesWidth->setValue( lineStyle.width() );
    connect( d->m_linesWidth, SIGNAL(editingFinished()), this, SLOT(updatePolyline()) );

    // Adjust the color button's icon to the current lines color.
    QPixmap linesPixmap( d->m_linesColorButton->iconSize().width(),
                         d->m_linesColorButton->iconSize().height() );
    linesPixmap.fill( lineStyle.color() );
    d->m_linesColorButton->setIcon( QIcon( linesPixmap ) );

    // Setup the color dialogs.
    d->m_linesDialog = new QColorDialog( this );
    d->m_linesDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_linesDialog->setCurrentColor( lineStyle.color() );
    connect( d->m_linesColorButton, SIGNAL(clicked()), d->m_linesDialog, SLOT(exec()) );
    connect( d->m_linesDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateLinesDialog(const QColor&)) );
    connect( d->m_linesDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updatePolyline()) );

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

void EditPolylineDialog::setFirstTimeEditing( bool enabled )
{
    d->m_firstEditing = enabled;
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

void EditPolylineDialog::updatePolyline()
{
    d->m_placemark->setDescription( d->m_description->toPlainText() );
    d->m_placemark->setName( d->m_name->text() );


    GeoDataStyle *newStyle = new GeoDataStyle( *d->m_placemark->style() );
    newStyle->lineStyle().setColor( d->m_linesDialog->currentColor() );
    newStyle->lineStyle().setWidth( d->m_linesWidth->value() );
    d->m_placemark->setStyle( newStyle );

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

    if ( d->m_placemark->name() != d->m_initialName ) {
        d->m_placemark->setName( d->m_initialName );
    }

    if ( d->m_placemark->description() != d->m_initialDescription ) {
        d->m_placemark->setDescription( d->m_initialDescription );
    }

    if ( d->m_placemark->style()->lineStyle() != d->m_initialLineStyle ) {
        GeoDataStyle *newStyle = new GeoDataStyle( *d->m_placemark->style() );
        newStyle->setLineStyle( d->m_initialLineStyle );
        d->m_placemark->setStyle( newStyle );
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

#include "EditPolylineDialog.moc"
