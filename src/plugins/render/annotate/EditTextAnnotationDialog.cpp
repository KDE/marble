//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
//

// self
#include "EditTextAnnotationDialog.h"
#include "ui_EditTextAnnotationDialog.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QColorDialog>

// Marble
#include "GeoDataStyle.h"
#include "GeoDataPlacemark.h"
#include "PlacemarkTextAnnotation.h"


namespace Marble {

class EditTextAnnotationDialog::Private : public Ui::UiEditTextAnnotationDialog
{
public:
    Private( GeoDataPlacemark *placemark );
    ~Private();

    GeoDataPlacemark *m_placemark;

    // Attached to label/icon color selectors.
    QColorDialog *m_iconColorDialog;
    QColorDialog *m_labelColorDialog;

    // Used to tell whether the settings before showing the dialog should be restored on
    // pressing the 'Cancel' button or not.
    bool m_firstEditing;

    // Used to restore if the Cancel button is pressed.
    QString m_initialDescription;
    QString m_initialName;
    GeoDataCoordinates m_initialCoords;
    GeoDataStyle m_initialStyle;
};

EditTextAnnotationDialog::Private::Private( GeoDataPlacemark *placemark ) :
    Ui::UiEditTextAnnotationDialog(),
    m_placemark( placemark ),
    m_iconColorDialog( 0 ),
    m_labelColorDialog( 0 ),
    m_firstEditing( false )
{
    // nothing to do
}

EditTextAnnotationDialog::Private::~Private()
{
    delete m_iconColorDialog;
    delete m_labelColorDialog;
}

EditTextAnnotationDialog::EditTextAnnotationDialog( GeoDataPlacemark *placemark, QWidget *parent ) :
    QDialog( parent ),
    d( new Private( placemark ) )
{
    d->setupUi( this );

    // Store initial style so that it can be restored if the 'Cancel' button is pressed.
    d->m_initialStyle = *placemark->style();


    // If the placemark has just been created, assign it a default name.
    if ( placemark->name().isNull() ) {
        placemark->setName( tr("Untitled Placemark") );
    }
    // Setup name, icon link and latitude/longitude values.
    d->m_name->setText( placemark->name() );
    d->m_initialName = placemark->name();
    connect( d->m_name, SIGNAL(editingFinished()), this, SLOT(updateTextAnnotation()) );

    d->m_link->setText( placemark->style()->iconStyle().iconPath() );
    connect( d->m_link, SIGNAL(editingFinished()), this, SLOT(updateTextAnnotation()) );

    d->m_description->setText( placemark->description() );
    d->m_initialDescription = placemark->description();

    // Initialize the range for label/icon size.
    // FIXME: What should be the maximum size?
    d->m_labelScale->setRange( 1.0, 5.0 );
    d->m_iconScale->setRange( 1.0, 5.0 );
    // Initialize the range for latitude/longitude.
    d->m_latitude->setRange( -90, 90 );
    d->m_longitude->setRange( -180, 180 );


    d->m_latitude->setValue( placemark->coordinate().latitude( GeoDataCoordinates::Degree ) );
    connect( d->m_latitude, SIGNAL(editingFinished()), this, SLOT(updateTextAnnotation()) );
    d->m_longitude->setValue( placemark->coordinate().longitude( GeoDataCoordinates::Degree ) );
    connect( d->m_longitude, SIGNAL(editingFinished()), this, SLOT(updateTextAnnotation()) );
    d->m_initialCoords = GeoDataCoordinates( d->m_longitude->value(),
                                             d->m_latitude->value(),
                                             0,
                                             GeoDataCoordinates::Degree );


    // Adjust icon and label scales.
    d->m_iconScale->setValue( placemark->style()->iconStyle().scale() );
    connect( d->m_iconScale, SIGNAL(valueChanged(double)), this, SLOT(updateTextAnnotation()) );

    d->m_labelScale->setValue( placemark->style()->labelStyle().scale() );
    connect( d->m_labelScale, SIGNAL(valueChanged(double)), this, SLOT(updateTextAnnotation()) );


    // Adjust the current color of the two push buttons' pixmap to resemble the label and icon colors.
    const GeoDataLabelStyle labelStyle = placemark->style()->labelStyle();
    const GeoDataIconStyle iconStyle = placemark->style()->iconStyle();

    QPixmap labelPixmap( d->m_labelButton->iconSize().width(),
                         d->m_labelButton->iconSize().height() );
    labelPixmap.fill( labelStyle.color() );
    d->m_labelButton->setIcon( QIcon( labelPixmap ) );

    QPixmap iconPixmap( d->m_iconButton->iconSize().width(),
                        d->m_iconButton->iconSize().height() );
    iconPixmap.fill( iconStyle.color() );
    d->m_iconButton->setIcon( QIcon( iconPixmap ) );

    // Setup the color dialogs.
    d->m_labelColorDialog = new QColorDialog( this );
    d->m_labelColorDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_labelColorDialog->setCurrentColor( labelStyle.color() );
    connect( d->m_labelButton, SIGNAL(clicked()), d->m_labelColorDialog, SLOT(exec()) );
    connect( d->m_labelColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateLabelDialog(const QColor&)) );
    connect( d->m_labelColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateTextAnnotation()) );

    d->m_iconColorDialog = new QColorDialog( this );
    d->m_iconColorDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_iconColorDialog->setCurrentColor( iconStyle.color() );
    connect( d->m_iconButton, SIGNAL(clicked()), d->m_iconColorDialog, SLOT(exec()) );
    connect( d->m_iconColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateIconDialog(const QColor&)) );
    connect( d->m_iconColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateTextAnnotation()) );


    // Promote "Ok" button to default button.
    d->buttonBox->button( QDialogButtonBox::Ok )->setDefault( true );

    connect( d->m_browseButton, SIGNAL(clicked()), this, SLOT(loadIconFile()) );
    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(pressed()), this, SLOT(checkFields()) );
    connect( this, SIGNAL(accepted()), SLOT(updateTextAnnotation()) );
    connect( this, SIGNAL(finished(int)), SLOT(restoreInitial(int)) );

    // Ensure that the dialog gets deleted when closing it (either when clicking OK or
    // Close).
    connect( this, SIGNAL(finished(int)), SLOT(deleteLater()) );
}

EditTextAnnotationDialog::~EditTextAnnotationDialog()
{
    delete d;
}

void EditTextAnnotationDialog::setFirstTimeEditing( bool enabled )
{
    d->m_firstEditing = enabled;
}

void EditTextAnnotationDialog::updateDialogFields()
{
    d->m_latitude->setValue( d->m_placemark->coordinate().latitude( GeoDataCoordinates::Degree ) );
    d->m_longitude->setValue( d->m_placemark->coordinate().longitude( GeoDataCoordinates::Degree ) );
}

void EditTextAnnotationDialog::updateTextAnnotation()
{
    d->m_placemark->setDescription( d->m_description->toPlainText() );
    d->m_placemark->setName( d->m_name->text() );
    d->m_placemark->setCoordinate( GeoDataCoordinates( d->m_longitude->value(),
                                                                         d->m_latitude->value(),
                                                                         0,
                                                                         GeoDataCoordinates::Degree ) );


    GeoDataStyle *newStyle = new GeoDataStyle( *d->m_placemark->style() );

    QFileInfo fileInfo( d->m_link->text() );
    if ( fileInfo.exists() ) {
        newStyle->iconStyle().setIconPath( d->m_link->text() );
    }

    newStyle->iconStyle().setScale( d->m_iconScale->value() );
    newStyle->labelStyle().setScale( d->m_labelScale->value() );

    newStyle->iconStyle().setColor( d->m_iconColorDialog->currentColor() );
    newStyle->labelStyle().setColor( d->m_labelColorDialog->currentColor() );

    d->m_placemark->setStyle( newStyle );


    emit textAnnotationUpdated( d->m_placemark );
}

void EditTextAnnotationDialog::loadIconFile()
{
    const QString filename = QFileDialog::getOpenFileName( this,
                                                           tr( "Open Annotation File" ),
                                                           QString(),
                                                           tr( "All Supported Files (*.png)" ) );
    if ( filename.isNull() ) {
        return;
    }

    d->m_link->setText( filename );
}

void EditTextAnnotationDialog::checkFields()
{
    if ( d->m_name->text().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No name specified" ),
                              tr( "Please specify a name for this placemark." ) );
    } else if ( d->m_link->text().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No image specified" ),
                              tr( "Please specify an icon for this placemark." ) );
    } else {
        QFileInfo fileInfo( d->m_link->text() );
        if ( !fileInfo.exists() ) {
            QMessageBox::warning( this,
                                  tr( "Invalid icon path" ),
                                  tr( "Please specify a valid path for the icon file." ) );
        }
    }
}

void EditTextAnnotationDialog::updateLabelDialog( const QColor &color )
{
    QPixmap labelPixmap( d->m_labelButton->iconSize().width(),
                         d->m_labelButton->iconSize().height() );
    labelPixmap.fill( color );
    d->m_labelButton->setIcon( QIcon( labelPixmap ) );
}

void EditTextAnnotationDialog::updateIconDialog( const QColor &color )
{
    QPixmap iconPixmap( d->m_iconButton->iconSize().width(),
                        d->m_iconButton->iconSize().height() );
    iconPixmap.fill( color );
    d->m_iconButton->setIcon( QIcon( iconPixmap ) );
}

void EditTextAnnotationDialog::restoreInitial( int result )
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

    if ( d->m_placemark->coordinate().latitude( GeoDataCoordinates::Degree ) !=
         d->m_initialCoords.latitude( GeoDataCoordinates::Degree ) ||
         d->m_placemark->coordinate().longitude( GeoDataCoordinates::Degree ) !=
         d->m_initialCoords.longitude( GeoDataCoordinates::Degree ) ) {
        d->m_placemark->setCoordinate( d->m_initialCoords );
    }

    if ( *d->m_placemark->style() != d->m_initialStyle ) {
        d->m_placemark->setStyle( new GeoDataStyle( d->m_initialStyle ) );
    }

    emit textAnnotationUpdated( d->m_placemark );
}

}

#include "EditTextAnnotationDialog.moc"
