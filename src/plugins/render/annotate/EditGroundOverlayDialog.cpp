//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

// Self
#include "EditGroundOverlayDialog.h"
#include "ui_EditGroundOverlayDialog.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>


namespace Marble
{

class EditGroundOverlayDialog::Private : public Ui::UiEditGroundOverlayDialog
{

public:
    GeoDataGroundOverlay *m_overlay;
    TextureLayer         *m_textureLayer;

    Private( GeoDataGroundOverlay *overlay, TextureLayer *textureLayer );
    ~Private();

    void updateCoordinates();
};

EditGroundOverlayDialog::Private::Private( GeoDataGroundOverlay *overlay, TextureLayer *textureLayer ) :
    Ui::UiEditGroundOverlayDialog(),
    m_overlay( overlay ),
    m_textureLayer( textureLayer )
{
    // nothing to do
}

EditGroundOverlayDialog::Private::~Private()
{
    // nothing to do
}

EditGroundOverlayDialog::EditGroundOverlayDialog( GeoDataGroundOverlay *overlay,
                                                  TextureLayer *textureLayer,
                                                  QWidget *parent ) :
    QDialog( parent ),
    d( new Private( overlay, textureLayer ) )
{
    d->setupUi( this );

    d->m_name->setText( overlay->name() );
    d->m_link->setText( overlay->absoluteIconFile() );
    d->m_description->setText( overlay->description() );

    d->m_north->setRange( -90, 90 );
    d->m_south->setRange( -90, 90 );
    d->m_west->setRange( -180, 180 );
    d->m_east->setRange( -180, 180 );
    d->m_rotation->setRange( -360, 360 );

    GeoDataLatLonBox latLonBox = overlay->latLonBox();
    d->m_north->setValue( latLonBox.north( GeoDataCoordinates::Degree ) );
    d->m_south->setValue( latLonBox.south( GeoDataCoordinates::Degree ) );
    d->m_west->setValue( latLonBox.west( GeoDataCoordinates::Degree ) );
    d->m_east->setValue( latLonBox.east( GeoDataCoordinates::Degree ) );
    d->m_rotation->setValue( latLonBox.rotation( GeoDataCoordinates::Degree ) );

    connect( d->m_browseButton, SIGNAL(clicked()), this, SLOT(loadPicture()) );
    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(pressed()), this, SLOT(checkFields()) );
    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(clicked()), this, SLOT(updateGroundOverlay()) );
    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(clicked()), this, SLOT(setGroundOverlayUpdated()) );
    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(clicked()), d->m_textureLayer, SLOT(reset()) );
}

EditGroundOverlayDialog::~EditGroundOverlayDialog()
{
    delete d;
}

void EditGroundOverlayDialog::loadPicture()
{
    const QString filename = QFileDialog::getOpenFileName( this,
                                                           tr( "Open Annotation File" ),
                                                           QString(),
                                                           tr( "All Supported Files (*.jpg *.png)" ) );
    if ( filename.isNull() ) {
        return;
    }

    d->m_link->setText( filename );
}

void EditGroundOverlayDialog::updateGroundOverlay()
{
    d->m_overlay->setName( d->m_name->text() );
    d->m_overlay->setIconFile( d->m_link->text() );
    d->m_overlay->setDescription( d->m_description->toPlainText() );

    d->m_overlay->latLonBox().setBoundaries( d->m_north->value(),
                                             d->m_south->value(),
                                             d->m_east->value(),
                                             d->m_west->value(),
                                             GeoDataCoordinates::Degree  );

    d->m_overlay->latLonBox().setRotation( d->m_rotation->value(), GeoDataCoordinates::Degree );
}

void EditGroundOverlayDialog::setGroundOverlayUpdated()
{
    emit groundOverlayUpdated( d->m_overlay );
}

void EditGroundOverlayDialog::checkFields()
{
    if ( d->m_name->text().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No name specified" ),
                              tr( "Please specify a name for this ground overlay." ) );
    } else if ( d->m_link->text().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No image specified" ),
                              tr( "Please specify an image file." ) );
    } else {
        QFileInfo fileInfo( d->m_link->text() );
        if ( !fileInfo.exists() ) {
            QMessageBox::warning( this,
                                  tr( "Invalid image path" ),
                                  tr( "Please specify a valid path for the image file." ) );
        }
    }
}

}

#include "EditGroundOverlayDialog.moc"
