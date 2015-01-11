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
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QColorDialog>
#include <QCheckBox>
#include <QToolBar>
#include <QTextEdit>
#include <QFontComboBox>
#include <QPushButton>
#include <QLineEdit>

// Marble
#include "GeoDataStyle.h"
#include "GeoDataPlacemark.h"
#include "MarbleWidget.h"
#include "MarbleLocale.h"
#include "AddLinkDialog.h"

namespace Marble {

class EditPlacemarkDialog::Private : public Ui::UiEditPlacemarkDialog
{
public:
    Private( GeoDataPlacemark *placemark );
    ~Private();

    GeoDataPlacemark *m_placemark;

    // Attached to label/icon/text color selectors.
    QColorDialog *m_iconColorDialog;
    QColorDialog *m_labelColorDialog;
    QColorDialog *m_textColorDialog;

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
    QPushButton *m_textColorButton;
};

EditPlacemarkDialog::Private::Private( GeoDataPlacemark *placemark ) :
    Ui::UiEditPlacemarkDialog(),
    m_placemark( placemark ),
    m_iconColorDialog( 0 ),
    m_labelColorDialog( 0 ),
    m_firstEditing( false ),
    m_textColorButton( new QPushButton )
{
    // nothing to do
}

EditPlacemarkDialog::Private::~Private()
{
    delete m_elevationWidget;
    delete m_iconColorDialog;
    delete m_labelColorDialog;
    delete m_textColorDialog;
}

EditPlacemarkDialog::EditPlacemarkDialog( GeoDataPlacemark *placemark, QWidget *parent ) :
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
                 updateTextAnnotation()) );

    if( d->m_isFormattedTextMode->isChecked() ) {
        d->m_description->setHtml( placemark->description() );
    } else {
        d->m_description->setPlainText( placemark->description() );
    }
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

    d->m_formattedTextToolBar->setVisible( false );
    d->m_fontSize->setVisible( false );
    d->m_fontFamily->setVisible( false );
    QAction *separator = d->m_formattedTextToolBar->insertSeparator( d->m_actionAddImage );
    d->m_formattedTextToolBar->insertWidget( separator, d->m_textColorButton );
    d->m_textColorButton->setMaximumSize( 24, 24 );
    QPixmap textColorPixmap( d->m_textColorButton->iconSize().width(),
                        d->m_textColorButton->iconSize().height() );
    textColorPixmap.fill( d->m_description->textCursor().charFormat().foreground().color() );
    d->m_textColorButton->setIcon( QIcon( textColorPixmap ) );
    d->m_textColorDialog = new QColorDialog( this );
    d->m_textColorDialog->setOption( QColorDialog::ShowAlphaChannel );
    d->m_textColorDialog->setCurrentColor( d->m_description->textCursor().charFormat().foreground().color() );
    d->m_fontSize->setValidator( new QIntValidator( 1, 9000, this ) );
    int index = d->m_fontSize->findText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    if( index != -1 ) {
        d->m_fontSize->setCurrentIndex( index );
    } else {
        d->m_fontSize->lineEdit()->setText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    }
    connect( d->m_textColorButton, SIGNAL( clicked() ), d->m_textColorDialog, SLOT( exec() ) );
    connect( d->m_textColorDialog, SIGNAL( colorSelected( QColor ) ), this, SLOT( setTextCursorColor( const QColor& ) ) );
    connect( d->m_isFormattedTextMode, SIGNAL( toggled( bool ) ), this, SLOT( toggleDescriptionEditMode( bool ) ) );
    connect( d->m_fontFamily, SIGNAL( currentFontChanged( QFont ) ), this, SLOT( setTextCursorFont( QFont ) ) );
    connect( d->m_fontSize, SIGNAL( editTextChanged( QString ) ), this, SLOT( setTextCursorFontSize( QString ) ) );
    connect( d->m_actionBold, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorBold( bool ) ) );
    connect( d->m_actionItalics, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorItalic( bool ) ) );
    connect( d->m_actionUnderlined, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorUnderlined( bool ) ) );
    connect( d->m_actionAddImage, SIGNAL( triggered() ), this, SLOT( addImageToDescription() ) );
    connect( d->m_actionAddLink, SIGNAL( triggered() ), this, SLOT( addLinkToDescription() ) );
    connect( d->m_description, SIGNAL( cursorPositionChanged() ), this, SLOT( updateDescriptionEditButtons() ) );

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

void EditPlacemarkDialog::setFirstTimeEditing( bool enabled )
{
    d->m_firstEditing = enabled;
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
    d->m_description->setReadOnly(state);
    d->m_isBalloonVisible->setDisabled(state);
    d->m_isPlacemarkVisible->setDisabled(state);
    d->style_color_tab->setDisabled(state);
}

void EditPlacemarkDialog::updateTextAnnotation()
{
    if( d->m_isFormattedTextMode->isChecked() ) {
        d->m_placemark->setDescription( d->m_description->toHtml() );
    } else {
        d->m_placemark->setDescription( d->m_description->toPlainText() );
    }
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
    } else if ( d->m_header->iconLink().isEmpty() ) {
        QMessageBox::warning( this,
                              tr( "No image specified" ),
                              tr( "Please specify an icon for this placemark." ) );
    } else if( !QFileInfo( d->m_header->iconLink() ).exists() ) {
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

void EditPlacemarkDialog::toggleDescriptionEditMode(bool isFormattedTextMode)
{
    d->m_formattedTextToolBar->setVisible( isFormattedTextMode );
    d->m_fontSize->setVisible( isFormattedTextMode );
    d->m_fontFamily->setVisible( isFormattedTextMode );
    if( isFormattedTextMode ) {
        d->m_description->setHtml( d->m_description->toPlainText() );
    } else {
        QTextCursor cursor = d->m_description->textCursor();
        QTextCharFormat format;
        format.setFont( QFont() );
        format.setFontWeight( QFont::Normal );
        format.setFontItalic( false );
        format.setFontUnderline( false );
        format.clearForeground();
        cursor.setCharFormat( format );
        d->m_description->setTextCursor( cursor );
        d->m_description->setPlainText( d->m_description->toHtml() );
    }
}

void EditPlacemarkDialog::setTextCursorBold( bool bold )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontWeight( bold ? QFont::Bold : QFont::Normal );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void EditPlacemarkDialog::setTextCursorItalic( bool italic )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontItalic( italic );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void EditPlacemarkDialog::setTextCursorUnderlined( bool underlined )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontUnderline( underlined );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void EditPlacemarkDialog::setTextCursorColor( const QColor &color )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    QBrush brush( color );
    format.setForeground( brush );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
    QPixmap textColorPixmap( d->m_textColorButton->iconSize().width(),
                        d->m_textColorButton->iconSize().height() );
    textColorPixmap.fill( format.foreground().color() );
    d->m_textColorButton->setIcon( QIcon( textColorPixmap ) );
    d->m_textColorDialog->setCurrentColor( format.foreground().color() );
}

void EditPlacemarkDialog::setTextCursorFont( const QFont &font )
{
    QTextCursor cursor = d->m_description->textCursor();
    QTextCharFormat format;
    format.setFontFamily( font.family() );
    cursor.mergeCharFormat( format );
    d->m_description->setTextCursor( cursor );
}

void EditPlacemarkDialog::setTextCursorFontSize(const QString &fontSize)
{
    bool ok = false;
    int size = fontSize.toInt( &ok );
    if( ok ) {
        QTextCursor cursor = d->m_description->textCursor();
        QTextCharFormat format;
        format.setFontPointSize( size );
        cursor.mergeCharFormat( format );
        d->m_description->setTextCursor( cursor );
    }
}

void EditPlacemarkDialog::addImageToDescription()
{
    QString filename = QFileDialog::getOpenFileName( this, tr( "Choose image" ), tr( "All Supported Files (*.png *.jpg *.jpeg)" )  );
    QImage image( filename );
    if( !image.isNull() ) {
        QTextCursor cursor = d->m_description->textCursor();
        cursor.insertImage( image, filename );
    }
}

void EditPlacemarkDialog::addLinkToDescription()
{
    QPointer<AddLinkDialog> dialog = new AddLinkDialog( this );
    if( dialog->exec() ) {
        QTextCharFormat oldFormat = d->m_description->textCursor().charFormat();
        QTextCharFormat linkFormat = oldFormat;
        linkFormat.setAnchor( true );
        linkFormat.setFontUnderline( true );
        linkFormat.setForeground( QApplication::palette().link() );
        linkFormat.setAnchorHref( dialog->url() );
        d->m_description->textCursor().insertText( dialog->name(), linkFormat );
        QTextCursor cursor =  d->m_description->textCursor();
        cursor.setCharFormat( oldFormat );
        d->m_description->setTextCursor( cursor );
        d->m_description->textCursor().insertText( " " );
    }
}

void EditPlacemarkDialog::updateDescriptionEditButtons()
{
    disconnect( d->m_actionBold, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorBold( bool ) ) );
    disconnect( d->m_actionItalics, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorItalic( bool ) ) );
    disconnect( d->m_actionUnderlined, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorUnderlined( bool ) ) );
    disconnect( d->m_fontFamily, SIGNAL( currentFontChanged( QFont ) ), this, SLOT( setTextCursorFont( QFont ) ) );
    disconnect( d->m_fontSize, SIGNAL( editTextChanged( QString ) ), this, SLOT( setTextCursorFontSize( QString ) ) );

    QTextCharFormat format = d->m_description->textCursor().charFormat();

    d->m_fontFamily->setCurrentFont( format.font() );

    if( format.fontWeight() == QFont::Bold ) {
        d->m_actionBold->setChecked( true );
    } else if ( format.fontWeight() == QFont::Normal ) {
        d->m_actionBold->setChecked( false );
    }
    d->m_actionItalics->setChecked( format.fontItalic() );
    d->m_actionUnderlined->setChecked( format.fontUnderline() );

    QPixmap textColorPixmap( d->m_textColorButton->iconSize().width(),
                        d->m_textColorButton->iconSize().height() );
    textColorPixmap.fill( format.foreground().color() );
    d->m_textColorButton->setIcon( QIcon( textColorPixmap ) );
    d->m_textColorDialog->setCurrentColor( format.foreground().color() );

    int index = d->m_fontSize->findText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    if( index != -1 ) {
        d->m_fontSize->setCurrentIndex( index );
    } else {
        d->m_fontSize->lineEdit()->setText( QString::number( d->m_description->textCursor().charFormat().font().pointSize() ) );
    }
    connect( d->m_actionBold, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorBold( bool ) ) );
    connect( d->m_actionItalics, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorItalic( bool ) ) );
    connect( d->m_actionUnderlined, SIGNAL( toggled( bool ) ), this, SLOT( setTextCursorUnderlined( bool ) ) );
    connect( d->m_fontFamily, SIGNAL( currentFontChanged( QFont ) ), this, SLOT( setTextCursorFont( QFont ) ) );
    connect( d->m_fontSize, SIGNAL( editTextChanged( QString ) ), this, SLOT( setTextCursorFontSize( QString ) ) );
}

}

#include "EditPlacemarkDialog.moc"
