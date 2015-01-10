//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru   <crucerucalincristian@gmail.com>
// Copyright 2015      Ilya Kowalewski <illya.kovalevskyy@gmail.com>
//

// self
#include "EditTextAnnotationDialog.h"
#include "ui_EditTextAnnotationDialog.h"
#include "ui_ElevationWidget.h"

// Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QColorDialog>

// Marble
#include "GeoDataStyle.h"
#include "GeoDataPlacemark.h"
#include "MarbleWidget.h"
#include "MarbleLocale.h"

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
    QString m_styleColorTabName;
    bool m_initialIsPlacemarkVisible;
    bool m_initialIsBaloonVisible;
    bool m_initialDescriptionIsCDATA;
    QString m_initialId;
    Ui::ElevationWidget *m_elevationWidget;
    MarbleLocale::MeasureUnit m_elevationUnit;
    QString m_initialTargetId;
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
    delete m_elevationWidget;
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
    // Setup id, name, icon link and latitude/longitude values.
    d->m_header->setId( placemark->id() );
    d->m_initialId = placemark->id();
    d->m_header->setName( placemark->name() );
    d->m_initialName = placemark->name();
    d->m_isPlacemarkVisible->setChecked( placemark->isVisible() );
    d->m_initialIsPlacemarkVisible = placemark->isVisible();
    d->m_header->setIconLink( placemark->style()->iconStyle().iconPath() );
    d->m_header->setTargetId( placemark->targetId() );
    d->m_initialTargetId = placemark->targetId();
    MarbleWidget* marbleWidget = dynamic_cast<MarbleWidget*>( parent );
    if( marbleWidget != 0 ) {
        const AngleUnit defaultAngleUnit = marbleWidget->defaultAngleUnit();
        const GeoDataCoordinates::Notation notation =
            (defaultAngleUnit == DecimalDegree) ? GeoDataCoordinates::Decimal :
            (defaultAngleUnit == DMSDegree) ?     GeoDataCoordinates::DMS :
            /* else, UTM */                       GeoDataCoordinates::DMS;
        d->m_header->setNotation( notation );
    }
    connect( d->m_header, SIGNAL(valueChanged()), this, SLOT(
                 TextAnnotation()) );

    d->m_description->setPlainText( placemark->description() );
    d->m_initialDescription = placemark->description();
    d->m_initialDescriptionIsCDATA = placemark->descriptionIsCDATA();
    d->m_isBalloonVisible->setChecked( placemark->isBalloonVisible() );
    d->m_initialIsBaloonVisible = placemark->isBalloonVisible();

    d->m_header->setLatitude( placemark->coordinate().latitude( GeoDataCoordinates::Degree ) );
    d->m_header->setLongitude( placemark->coordinate().longitude( GeoDataCoordinates::Degree ) );
    d->m_initialCoords = GeoDataCoordinates( d->m_header->longitude(),
                                             d->m_header->latitude(),
                                             0,
                                             GeoDataCoordinates::Degree );

    d->m_elevationWidget = new Ui::ElevationWidget;
    QWidget *elevationTab = new QWidget;
    d->m_elevationWidget->setupUi( elevationTab );
    d->tabWidget->addTab( elevationTab, tr("Elevation") );
    qreal altitude = d->m_placemark->coordinate().altitude();
    MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
    if ( altitude == 0.0 ) {
        switch ( locale->measurementSystem() ) {
        case MarbleLocale::MetricSystem:
            d->m_elevationUnit = MarbleLocale::Meter;
            break;
        case MarbleLocale::ImperialSystem:
            d->m_elevationUnit = MarbleLocale::Foot;
            break;
        case MarbleLocale::NauticalSystem:
            d->m_elevationUnit = MarbleLocale::NauticalMile;
            break;
        }

        d->m_elevationWidget->elevationSpinBox->setSuffix( locale->unitAbbreviation((d->m_elevationUnit)) );
    } else {
        qreal convertedAltitude;
        const MarbleLocale::MeasurementSystem currentSystem = locale->measurementSystem();
        locale->meterToTargetUnit( altitude, currentSystem, convertedAltitude, d->m_elevationUnit );
        d->m_elevationWidget->elevationSpinBox->setValue( convertedAltitude );
        d->m_elevationWidget->elevationSpinBox->setSuffix( locale->unitAbbreviation(d->m_elevationUnit) );
    }

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

    connect( d->buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(pressed()), this, SLOT(checkFields()) );
    connect( this, SIGNAL(accepted()), SLOT(updateTextAnnotation()) );
    connect( this, SIGNAL(accepted()), SLOT(updatePlacemarkAltitude()) );
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

QStringList EditTextAnnotationDialog::idFilter() const
{
    return d->m_header->idFilter();
}

QStringList EditTextAnnotationDialog::targetIds() const
{
    return d->m_header->targetIdList();
}

bool EditTextAnnotationDialog::isTargetIdFieldVisible() const
{
    return d->m_header->isTargetIdVisible();
}

bool EditTextAnnotationDialog::isIdFieldVisible() const
{
    return d->m_header->isIdVisible();
}

void EditTextAnnotationDialog::updateDialogFields()
{
    d->m_header->setLatitude( d->m_placemark->coordinate().latitude( GeoDataCoordinates::Degree ) );
    d->m_header->setLongitude( d->m_placemark->coordinate().longitude( GeoDataCoordinates::Degree ) );
}

void EditTextAnnotationDialog::setIdFilter(const QStringList &filter)
{
    d->m_header->setIdFilter( filter );
}

void EditTextAnnotationDialog::setTargetIds(const QStringList &targetIds)
{
    d->m_header->setTargetIdList( targetIds );
}

void EditTextAnnotationDialog::setTargetIdFieldVisible(bool visible)
{
    d->m_header->setTargetIdVisible( visible );
}

void EditTextAnnotationDialog::setIdFieldVisible(bool visible)
{
    d->m_header->setIdVisible( visible );
}

void EditTextAnnotationDialog::updateTextAnnotation()
{
    d->m_placemark->setDescription( d->m_description->toPlainText() );
    //allow for HTML in the description
    d->m_placemark->setDescriptionCDATA( true );
    d->m_placemark->setName( d->m_header->name() );
    d->m_placemark->setCoordinate( GeoDataCoordinates( d->m_header->longitude(),
                                                                         d->m_header->latitude(),
                                                                         0,
                                                                         GeoDataCoordinates::Degree ) );

    d->m_placemark->setVisible( d->m_isPlacemarkVisible->isChecked() );
    d->m_placemark->setBalloonVisible( d->m_isBalloonVisible->isChecked() );
    d->m_placemark->setId( d->m_header->id() );
    d->m_placemark->setTargetId( d->m_header->targetId() );

    GeoDataStyle *newStyle = new GeoDataStyle( *d->m_placemark->style() );

    QFileInfo fileInfo( d->m_header->iconLink() );
    if ( fileInfo.exists() ) {
        newStyle->iconStyle().setIconPath( d->m_header->iconLink() );
    }

    newStyle->iconStyle().setScale( d->m_iconScale->value() );
    newStyle->labelStyle().setScale( d->m_labelScale->value() );

    newStyle->iconStyle().setColor( d->m_iconColorDialog->currentColor() );
    newStyle->labelStyle().setColor( d->m_labelColorDialog->currentColor() );

    d->m_placemark->setStyle( newStyle );

    emit textAnnotationUpdated( d->m_placemark );
}

void EditTextAnnotationDialog::checkFields()
{
    if ( d->m_header->name().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No name specified" ),
                              tr( "Please specify a name for this placemark." ) );
    } else if ( isIdFieldVisible() && d->m_header->id().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No ID specified" ),
                              tr( "Please specify a ID for this placemark." ) );
    } else if ( isIdFieldVisible() && !d->m_header->isIdValid() ) {
        QMessageBox::warning( this,
                              tr( "ID is invalid" ),
                              tr( "Please specify a valid ID for this placemark." ) );
    } else if ( d->m_header->iconLink().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No image specified" ),
                              tr( "Please specify an icon for this placemark." ) );
    } else {
        QFileInfo fileInfo( d->m_header->iconLink() );
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

void EditTextAnnotationDialog::updatePlacemarkAltitude()
{
    GeoDataCoordinates coord = d->m_placemark->coordinate();
    qreal altitude = d->m_elevationWidget->elevationSpinBox->value();

    switch (d->m_elevationUnit) {
    case MarbleLocale::Meter:
        break;
    case MarbleLocale::Milimeter:
        altitude *= MM2M;
        break;
    case MarbleLocale::Kilometer:
        altitude *= KM2METER;
        break;
    case MarbleLocale::Centimeter:
        altitude *= CM2M;
        break;
    case MarbleLocale::Foot:
        altitude *= FT2M;
        break;
    case MarbleLocale::Inch:
        altitude *= IN2M;
        break;
    case MarbleLocale::Yard:
        altitude *= YD2M;
        break;
    case MarbleLocale::Mile:
        altitude *= MI2KM * KM2METER;
        break;
    case MarbleLocale::NauticalMile:
        altitude *= NM2KM * KM2METER;
        break;
    default:
        break;
    }

    coord.setAltitude(altitude);
    d->m_placemark->setCoordinate(coord);
}

void EditTextAnnotationDialog::restoreInitial( int result )
{
    if ( result ) {
        return;
    }

    if ( d->m_placemark->name() != d->m_initialName ) {
        d->m_placemark->setName( d->m_initialName );
    }

    if ( isIdFieldVisible() && d->m_placemark->id() != d->m_initialId ) {
        d->m_placemark->setId( d->m_initialId );
    }

    if ( isTargetIdFieldVisible() && d->m_placemark->targetId() != d->m_initialTargetId ) {
        d->m_placemark->setTargetId( d->m_initialTargetId );
    }

    if ( d->m_placemark->description() != d->m_initialDescription ) {
        d->m_placemark->setDescription( d->m_initialDescription );
    }

    if ( d->m_placemark->descriptionIsCDATA() != d->m_initialDescriptionIsCDATA ) {
        d->m_placemark->setDescriptionCDATA( d->m_initialDescriptionIsCDATA );
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

    if( d->m_placemark->isVisible() != d->m_initialIsPlacemarkVisible ) {
        d->m_placemark->setVisible( d->m_initialIsPlacemarkVisible );
    }

    if( d->m_placemark->isBalloonVisible() != d->m_initialIsBaloonVisible ) {
        d->m_placemark->setVisible( d->m_initialIsBaloonVisible );
    }

    emit textAnnotationUpdated( d->m_placemark );
}

}

#include "EditTextAnnotationDialog.moc"
