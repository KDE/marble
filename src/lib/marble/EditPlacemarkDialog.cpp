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
#include "EditPlacemarkDialog.h"
#include "ui_EditPlacemarkDialog.h"
#include "ui_ElevationWidget.h"

// Qt
#include <QMessageBox>
#include <QFileInfo>
#include <QColorDialog>
#include <QCheckBox>

// Marble
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataPlacemark.h"
#include "MarbleWidget.h"
#include "MarbleLocale.h"
#include "FormattedTextWidget.h"
#include "StyleBuilder.h"
#include "osm/OsmTagEditorWidget.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmRelationManagerWidget.h"

namespace Marble {

class Q_DECL_HIDDEN EditPlacemarkDialog::Private : public Ui::UiEditPlacemarkDialog
{
public:
    Private( GeoDataPlacemark *placemark );
    ~Private();

    GeoDataPlacemark *m_placemark;

    // Attached to label/icon/text color selectors.
    QColorDialog *m_iconColorDialog;
    QColorDialog *m_labelColorDialog;

    // Used to restore if the Cancel button is pressed.
    QString m_initialDescription;
    QString m_initialName;
    GeoDataCoordinates m_initialCoords;
    GeoDataStyle m_initialStyle;
    GeoDataPlacemark::GeoDataVisualCategory m_initialVisualCategory;
    OsmPlacemarkData m_initialOsmData;
    QString m_styleColorTabName;
    bool m_initialIsPlacemarkVisible;
    bool m_initialIsBaloonVisible;
    bool m_initialDescriptionIsCDATA;
    bool m_hadInitialOsmData;
    QString m_initialId;
    Ui::ElevationWidget *m_elevationWidget;
    OsmTagEditorWidget *m_osmTagEditorWidget;
    OsmRelationManagerWidget *m_osmRelationManagerWidget;
    MarbleLocale::MeasureUnit m_elevationUnit;
    QString m_initialTargetId;

};

EditPlacemarkDialog::Private::Private( GeoDataPlacemark *placemark ) :
    Ui::UiEditPlacemarkDialog(),
    m_placemark( placemark ),
    m_iconColorDialog( 0 ),
    m_labelColorDialog( 0 ),
    m_osmTagEditorWidget( 0 ),
    m_osmRelationManagerWidget( 0 )
{
    // nothing to do
}

EditPlacemarkDialog::Private::~Private()
{
    delete m_elevationWidget;
    delete m_osmTagEditorWidget;
    delete m_osmRelationManagerWidget;
    delete m_iconColorDialog;
    delete m_labelColorDialog;
}

EditPlacemarkDialog::EditPlacemarkDialog( GeoDataPlacemark *placemark,
                                          const QHash<qint64, OsmPlacemarkData> *relations,
                                          QWidget *parent ) :
    QDialog( parent ),
    d( new Private( placemark ) )
{
    d->setupUi( this );

    // Store initial style so that it can be restored if the 'Cancel' button is pressed.
    d->m_initialStyle = *placemark->style();

    d->m_initialVisualCategory = placemark->visualCategory();


    d->m_hadInitialOsmData = placemark->hasOsmData();
    if ( d->m_hadInitialOsmData ) {
        d->m_initialOsmData = placemark->osmData();
    }


    // If the placemark has just been created, assign    it a default name.
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
            /* else, UTM */                       GeoDataCoordinates::UTM;
        d->m_header->setNotation( notation );
    }
    connect( d->m_header, SIGNAL(valueChanged()), this, SLOT(
                 updateTextAnnotation()) );

    d->m_formattedTextWidget->setText( placemark->description() );
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

    // There's no point showing Relations and Tags tabs if the editor was not
    // loaded from the annotate plugin ( loaded from tourWidget.. )
    if ( relations ) {
        // Adding the osm tag editor widget tab
        d->m_osmTagEditorWidget = new OsmTagEditorWidget( placemark, this );
        d->tabWidget->addTab( d->m_osmTagEditorWidget, tr( "Tags" ) );
        QObject::connect( d->m_osmTagEditorWidget, SIGNAL( placemarkChanged( GeoDataFeature* ) ),
                          this, SLOT( updateTextAnnotation() ) );

        // Adding the osm relation editor widget tab
        d->m_osmRelationManagerWidget = new OsmRelationManagerWidget( placemark, relations, this );
        d->tabWidget->addTab( d->m_osmRelationManagerWidget, tr( "Relations" ) );
        QObject::connect( d->m_osmRelationManagerWidget, SIGNAL( relationCreated( const OsmPlacemarkData& ) ),
                          this, SIGNAL( relationCreated( const OsmPlacemarkData& ) ) );
    }

    // Adding the elevation widget tab
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
    connect( d->m_labelColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateLabelDialog(QColor)) );
    connect( d->m_labelColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateTextAnnotation()) );

    d->m_iconColorDialog = new QColorDialog( this );
    d->m_iconColorDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_iconColorDialog->setCurrentColor( iconStyle.color() );
    connect( d->m_iconButton, SIGNAL(clicked()), d->m_iconColorDialog, SLOT(exec()) );
    connect( d->m_iconColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateIconDialog(QColor)) );
    connect( d->m_iconColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(updateTextAnnotation()) );

    connect( d->m_isBalloonVisible, SIGNAL(toggled(bool)), this, SLOT(updateTextAnnotation()) );

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

EditPlacemarkDialog::~EditPlacemarkDialog()
{
    delete d;
}

void EditPlacemarkDialog::setLabelColor( const QColor &color )
{
    d->m_labelColorDialog->setCurrentColor(color);
    updateLabelDialog(color);
}

QStringList EditPlacemarkDialog::idFilter() const
{
    return d->m_header->idFilter();
}

QStringList EditPlacemarkDialog::targetIds() const
{
    return d->m_header->targetIdList();
}

bool EditPlacemarkDialog::isTargetIdFieldVisible() const
{
    return d->m_header->isTargetIdVisible();
}

bool EditPlacemarkDialog::isIdFieldVisible() const
{
    return d->m_header->isIdVisible();
}

void EditPlacemarkDialog::updateDialogFields()
{
    d->m_header->setLatitude( d->m_placemark->coordinate().latitude( GeoDataCoordinates::Degree ) );
    d->m_header->setLongitude( d->m_placemark->coordinate().longitude( GeoDataCoordinates::Degree ) );
}

void EditPlacemarkDialog::setIdFilter(const QStringList &filter)
{
    d->m_header->setIdFilter( filter );
}

void EditPlacemarkDialog::setTargetIds(const QStringList &targetIds)
{
    d->m_header->setTargetIdList( targetIds );
}

void EditPlacemarkDialog::setTargetIdFieldVisible(bool visible)
{
    d->m_header->setTargetIdVisible( visible );
}

void EditPlacemarkDialog::setIdFieldVisible(bool visible)
{
    d->m_header->setIdVisible( visible );
}

void EditPlacemarkDialog::setReadOnly(bool state)
{
    d->m_header->setReadOnly(state);
    d->m_formattedTextWidget->setReadOnly(state);
    d->m_isBalloonVisible->setDisabled(state);
    d->m_isPlacemarkVisible->setDisabled(state);
    d->style_color_tab->setDisabled(state);
}

void EditPlacemarkDialog::updateTextAnnotation()
{
    d->m_placemark->setDescription( d->m_formattedTextWidget->text() );
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


    if ( !d->m_header->iconLink().isEmpty() ) {
        QFileInfo fileInfo( d->m_header->iconLink() );

        GeoDataStyle::Ptr newStyle(new GeoDataStyle( *d->m_placemark->style() ));
        if ( fileInfo.exists() ) {
            newStyle->iconStyle().setIconPath( d->m_header->iconLink() );
        }

        newStyle->iconStyle().setScale( d->m_iconScale->value() );
        newStyle->labelStyle().setScale( d->m_labelScale->value() );
        newStyle->iconStyle().setColor( d->m_iconColorDialog->currentColor() );
        newStyle->labelStyle().setColor( d->m_labelColorDialog->currentColor() );
        d->m_placemark->setStyle( newStyle );
    }
    else {
        const OsmPlacemarkData osmData = d->m_osmTagEditorWidget->placemarkData();
        const GeoDataPlacemark::GeoDataVisualCategory category = StyleBuilder::determineVisualCategory(osmData);
        if (category != GeoDataPlacemark::None) {
            d->m_placemark->setStyle(GeoDataStyle::Ptr());
            d->m_placemark->setVisualCategory( category );
        }
    }

    emit textAnnotationUpdated( d->m_placemark );
}

void EditPlacemarkDialog::checkFields()
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
    } else if (d->m_header->iconLink().isEmpty() && d->m_placemark->visualCategory() == GeoDataPlacemark::None) {
        QMessageBox::warning( this,
                              tr( "No image specified" ),
                              tr( "Please specify an icon for this placemark or add a valid tag." ) );
    } else if( !d->m_header->iconLink().isEmpty() && !QFileInfo( d->m_header->iconLink() ).exists() ) {
        QMessageBox::warning( this,
                              tr( "Invalid icon path" ),
                              tr( "Please specify a valid path for the icon file." ) );
    } else {
        accept();
    }
}

void EditPlacemarkDialog::updateLabelDialog( const QColor &color )
{
    QPixmap labelPixmap( d->m_labelButton->iconSize().width(),
                         d->m_labelButton->iconSize().height() );
    labelPixmap.fill( color );
    d->m_labelButton->setIcon( QIcon( labelPixmap ) );
}

void EditPlacemarkDialog::updateIconDialog( const QColor &color )
{
    QPixmap iconPixmap( d->m_iconButton->iconSize().width(),
                        d->m_iconButton->iconSize().height() );
    iconPixmap.fill( color );
    d->m_iconButton->setIcon( QIcon( iconPixmap ) );
}

void EditPlacemarkDialog::updatePlacemarkAltitude()
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

void EditPlacemarkDialog::restoreInitial( int result )
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

    if ( d->m_placemark->visualCategory() != d->m_initialVisualCategory ) {
        d->m_placemark->setVisualCategory( d->m_initialVisualCategory );
    }

    if ( *d->m_placemark->style() != d->m_initialStyle ) {
        d->m_placemark->setStyle( GeoDataStyle::Ptr(new GeoDataStyle( d->m_initialStyle )) );
    }

    if( d->m_placemark->isVisible() != d->m_initialIsPlacemarkVisible ) {
        d->m_placemark->setVisible( d->m_initialIsPlacemarkVisible );
    }

    if( d->m_hadInitialOsmData ) {
        d->m_placemark->setOsmData( d->m_initialOsmData );
    }

    if( d->m_placemark->isBalloonVisible() != d->m_initialIsBaloonVisible ) {
        d->m_placemark->setVisible( d->m_initialIsBaloonVisible );
    }

    emit textAnnotationUpdated( d->m_placemark );
}

}

#include "moc_EditPlacemarkDialog.cpp"
