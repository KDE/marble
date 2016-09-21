//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
// Copyright 2010   Daniel Marth    <danielmarth@gmx.at>
//

#include "PositionMarker.h"

#include "MarbleDebug.h"
#include <QRect>
#include <qmath.h>
#include <QFileDialog>
#include <QPushButton>
#include <QColorDialog>
#include <QTransform>

#include <cmath>

#include "ui_PositionMarkerConfigWidget.h"
#include "MarbleModel.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "PositionTracking.h"
#include "ViewportParams.h"
#include "Planet.h"
#include "GeoDataAccuracy.h"

namespace Marble
{

const int PositionMarker::sm_defaultSizeStep = 2;
const float PositionMarker::sm_resizeSteps[] = { 0.25, 0.5, 1.0, 2.0, 4.0 };
const int PositionMarker::sm_numResizeSteps = sizeof( sm_resizeSteps ) / sizeof( sm_resizeSteps[0] );

PositionMarker::PositionMarker( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_marbleModel( marbleModel ),
      m_isInitialized( false ),
      m_useCustomCursor( false ),
      m_defaultCursorPath(MarbleDirs::path(QStringLiteral("svg/track_turtle.svg"))),
      m_lastBoundingBox(),
      ui_configWidget( 0 ),
      m_configDialog( 0 ),
      m_cursorPath( m_defaultCursorPath ),
      m_cursorSize( 1.0 ),
      m_accuracyColor( Oxygen::brickRed4 ),
      m_trailColor( 0, 0, 255 ),
      m_heading( 0.0 ),
      m_showTrail ( false )
{
    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    m_accuracyColor.setAlpha( smallScreen ? 80 : 40 );
}

PositionMarker::~PositionMarker ()
{
    delete ui_configWidget;
    delete m_configDialog;
}

QStringList PositionMarker::renderPosition() const
{
    return QStringList(QStringLiteral("HOVERS_ABOVE_SURFACE"));
}

QString PositionMarker::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList PositionMarker::backendTypes() const
{
    return QStringList(QStringLiteral("positionmarker"));
}

QString PositionMarker::name() const
{
    return tr( "Position Marker" );
}

QString PositionMarker::guiString() const
{
    return tr( "&Position Marker" );
}

QString PositionMarker::nameId() const
{
    return QStringLiteral("positionMarker");
}

QString PositionMarker::version() const
{
    return QStringLiteral("1.0");
}

QString PositionMarker::description() const
{
    return tr( "draws a marker at the current position" );
}

QString PositionMarker::copyrightYears() const
{
    return QStringLiteral("2009, 2010");
}

QVector<PluginAuthor> PositionMarker::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Andrew Manson"), QStringLiteral("g.real.ate@gmail.com"))
            << PluginAuthor(QStringLiteral("Eckhart Woerner"), QStringLiteral("ewoerner@kde.org"))
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"))
            << PluginAuthor(QStringLiteral("Daniel Marth"), QStringLiteral("danielmarth@gmx.at"));
}

QIcon PositionMarker::icon() const
{
    return QIcon(QStringLiteral(":/icons/positionmarker.png"));
}

QDialog *PositionMarker::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::PositionMarkerConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        ui_configWidget->m_resizeSlider->setMaximum( sm_numResizeSteps - 1 );
        readSettings();
        connect( ui_configWidget->m_buttonBox, SIGNAL(accepted()),
                 SLOT(writeSettings()) );
        connect( ui_configWidget->m_buttonBox, SIGNAL(rejected()),
                 SLOT(readSettings()) );
        connect( ui_configWidget->m_buttonBox->button( QDialogButtonBox::RestoreDefaults ), SIGNAL(clicked()),
                 SLOT(restoreDefaultSettings()) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()),
                 SLOT(writeSettings()) );
        connect( ui_configWidget->m_fileChooserButton, SIGNAL(clicked()),
                 SLOT(chooseCustomCursor()) );
        connect( ui_configWidget->m_resizeSlider, SIGNAL(valueChanged(int)),
                 SLOT(resizeCursor(int)) );
        connect( ui_configWidget->m_acColorChooserButton, SIGNAL(clicked()),
                 SLOT(chooseColor()) );
        connect( ui_configWidget->m_trailColorChooserButton, SIGNAL(clicked()),
                 SLOT(chooseColor()) );
    }
    return m_configDialog;
}

void PositionMarker::initialize()
{
    if ( marbleModel() ) {
        connect( marbleModel()->positionTracking(), SIGNAL(gpsLocation(GeoDataCoordinates,qreal)),
                this, SLOT(setPosition(GeoDataCoordinates)) );
        connect( marbleModel()->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)),
                this, SIGNAL(repaintNeeded()) );
        m_isInitialized = true;
    }
    loadDefaultCursor();
}

bool PositionMarker::isInitialized() const
{
    return m_isInitialized;
}

bool PositionMarker::render( GeoPainter *painter,
                           ViewportParams *viewport,
                           const QString& renderPos,
                           GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    bool const gpsActive = marbleModel()->positionTracking()->positionProviderPlugin() != 0;
    bool const positionAvailable = marbleModel()->positionTracking()->status() == PositionProviderStatusAvailable;
    bool const positionValid = m_currentPosition.isValid();
    if ( gpsActive && positionAvailable && positionValid ) {
        m_lastBoundingBox = viewport->viewLatLonAltBox();

        qreal screenPositionX, screenPositionY;
        if (!viewport->screenCoordinates( m_currentPosition, screenPositionX, screenPositionY )){
            return true;
        }
        const GeoDataCoordinates top( m_currentPosition.longitude(), m_currentPosition.latitude()+0.1 );
        qreal screenTopX, screenTopY;
        if (!viewport->screenCoordinates( top, screenTopX, screenTopY )){
            return true;
        }
        qreal const correction = -90.0 + RAD2DEG * atan2( screenPositionY -screenTopY, screenPositionX - screenTopX );
        const qreal rotation = m_heading + correction;

        if ( m_useCustomCursor ) {
            QTransform transform;
            transform.rotate( rotation );
            bool const highQuality = painter->mapQuality() == HighQuality || painter->mapQuality() == PrintQuality;
            Qt::TransformationMode const mode = highQuality ? Qt::SmoothTransformation : Qt::FastTransformation;
            m_customCursorTransformed = m_customCursor.transformed( transform, mode );
        } else {
            // Calculate the scaled arrow shape
            const QPointF baseX( m_cursorSize, 0.0 );
            const QPointF baseY( 0.0, m_cursorSize );
            const QPointF relativeLeft  = - ( baseX * 9 ) + ( baseY * 9 );
            const QPointF relativeRight =   ( baseX * 9 ) + ( baseY * 9 );
            const QPointF relativeTip   = - ( baseY * 19.0 );
            m_arrow = QPolygonF() << QPointF( 0.0, 0.0 ) << relativeLeft << relativeTip << relativeRight;

            // Rotate the shape according to the current direction and move it to the screen center
            QMatrix transformation;
            transformation.translate( screenPositionX, screenPositionY );
            transformation.rotate( rotation );
            m_arrow = m_arrow * transformation;

            m_dirtyRegion = QRegion();
            m_dirtyRegion += ( m_arrow.boundingRect().toRect() );
            m_dirtyRegion += ( m_previousArrow.boundingRect().toRect() );
        }

        painter->save();

        GeoDataAccuracy accuracy = marbleModel()->positionTracking()->accuracy();
        if ( accuracy.horizontal > 0 && accuracy.horizontal < 1000 ) {
            // Paint a circle indicating the position accuracy
            painter->setPen( Qt::transparent );
            qreal planetRadius = m_marbleModel->planet()->radius();
            int width = qRound( accuracy.horizontal * viewport->radius() / planetRadius );
            if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
                int arrowSize = qMax<int>( m_arrow.boundingRect().width(), m_arrow.boundingRect().height() );
                width = qMax<int>( width, arrowSize + 10 );
            }

            painter->setBrush( m_accuracyColor );
            painter->drawEllipse( m_currentPosition, width, width );
        }

        // Draw trail if requested.
        if( m_showTrail ) {
            painter->save();

            // Use selected color to draw trail.
            painter->setBrush( m_trailColor );
            painter->setPen( m_trailColor );

            // we don't draw m_trail[0] which is current position
            for( int i = 1; i < m_trail.size(); ++i ) {
                // Get screen coordinates from coordinates on the map.
                qreal trailPointX, trailPointY;
                viewport->screenCoordinates( m_trail[i], trailPointX, trailPointY );

                const int size = ( sm_numTrailPoints - i ) * 3;
                QRectF trailRect;
                trailRect.setX( trailPointX - size / 2.0 );
                trailRect.setY( trailPointY - size / 2.0 );
                trailRect.setWidth( size );
                trailRect.setHeight( size );

                const qreal opacity = 1.0 - 0.15 * ( i - 1 );
                painter->setOpacity( opacity );
                painter->drawEllipse( trailRect );
            }

            painter->restore();
        }

        if( m_useCustomCursor)
        {
            painter->drawPixmap( m_currentPosition, m_customCursorTransformed );
        }
        else
        {
            painter->setPen( Qt::black );
            painter->setBrush( Qt::white );
            painter->drawPolygon( m_arrow );
        }

        painter->restore();
        m_previousArrow = m_arrow;
    }
    return true;
}

QHash<QString,QVariant> PositionMarker::settings() const
{
    QHash<QString, QVariant> settings = RenderPlugin::settings();

    settings.insert(QStringLiteral("useCustomCursor"), m_useCustomCursor);
    settings.insert(QStringLiteral("cursorPath"), m_cursorPath);
    settings.insert(QStringLiteral("cursorSize"), m_cursorSize);
    settings.insert(QStringLiteral("acColor"), m_accuracyColor);
    settings.insert(QStringLiteral("trailColor"), m_trailColor);
    settings.insert(QStringLiteral("showTrail"), m_showTrail);

    return settings;
}

void PositionMarker::setSettings( const QHash<QString, QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    QColor defaultColor = Oxygen::brickRed4;
    defaultColor.setAlpha( smallScreen ? 80 : 40 );

    m_useCustomCursor = settings.value(QStringLiteral("useCustomCursor"), false).toBool();
    m_cursorPath = settings.value(QStringLiteral("cursorPath"), m_defaultCursorPath).toString();
    m_cursorSize = settings.value(QStringLiteral("cursorSize"), 1.0).toFloat();
    loadCustomCursor( m_cursorPath, m_useCustomCursor );

    m_accuracyColor = settings.value(QStringLiteral("acColor"), defaultColor).value<QColor>();
    m_trailColor = settings.value(QStringLiteral("trailColor"), QColor(0, 0, 255)).value<QColor>();
    m_showTrail = settings.value(QStringLiteral("showTrail"), false).toBool();

    readSettings();
}

void PositionMarker::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    if( m_useCustomCursor )
        ui_configWidget->m_customCursor->click();
    else
        ui_configWidget->m_originalCursor->click();

    bool found = false;
    float cursorSize = m_cursorSize;
    for( int i = 0; i < sm_numResizeSteps && !found; i++ )
    {
        if( sm_resizeSteps[i] == cursorSize )
        {
            ui_configWidget->m_resizeSlider->setValue( i );
            found = true;
        }
    }
    if( !found )
    {
        ui_configWidget->m_resizeSlider->setValue( sm_defaultSizeStep );
        cursorSize = sm_resizeSteps[sm_defaultSizeStep];
    }

    ui_configWidget->m_sizeLabel->setText( tr( "Cursor Size: %1" ).arg( cursorSize ) );
    QPalette palette = ui_configWidget->m_acColorChooserButton->palette();
    palette.setColor( QPalette::Button, m_accuracyColor );
    ui_configWidget->m_acColorChooserButton->setPalette( palette );
    palette = ui_configWidget->m_trailColorChooserButton->palette();
    palette.setColor( QPalette::Button, m_trailColor );
    ui_configWidget->m_trailColorChooserButton->setPalette( palette );
    ui_configWidget->m_trailCheckBox->setChecked( m_showTrail );
}

void PositionMarker::writeSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    m_useCustomCursor = ui_configWidget->m_customCursor->isChecked();
    m_cursorPath = m_cursorPath;
    m_cursorSize = sm_resizeSteps[ui_configWidget->m_resizeSlider->value()];
    m_accuracyColor = m_accuracyColor;
    m_trailColor = m_trailColor;
    m_showTrail = ui_configWidget->m_trailCheckBox->isChecked();

    emit settingsChanged( nameId() );
}

void PositionMarker::setPosition( const GeoDataCoordinates &position )
{
    m_previousPosition = m_currentPosition;
    m_currentPosition = position;
    m_heading = marbleModel()->positionTracking()->direction();
    // Update the trail
    m_trail.push_front( m_currentPosition );
    for( int i = sm_numTrailPoints + 1; i< m_trail.size(); ++i ) {
            m_trail.pop_back();
    }
    if ( m_lastBoundingBox.contains( m_currentPosition ) )
    {
        emit repaintNeeded( m_dirtyRegion );
    }
}

void PositionMarker::chooseCustomCursor()
{
    QString filename = QFileDialog::getOpenFileName( NULL, tr( "Choose Custom Cursor" ) );
    if( !filename.isEmpty() )
        loadCustomCursor( filename, true );
}

void PositionMarker::loadCustomCursor( const QString& filename, bool useCursor )
{
    m_customCursor = QPixmap( filename ).scaled( 22 * m_cursorSize, 22 * m_cursorSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    if( !m_customCursor.isNull() )
    {
        if( m_configDialog )
        {
            if( useCursor )
                ui_configWidget->m_customCursor->click();
            ui_configWidget->m_fileChooserButton->setIconSize( QSize( m_customCursor.width(), m_customCursor.height() ) );
            ui_configWidget->m_fileChooserButton->setIcon( QIcon( m_customCursor ) );
        }
        m_cursorPath = filename;
    }
    else
    {
        mDebug() << "Unable to load custom cursor from " << filename << ". "
                 << "The default cursor will be used instead";
        if ( m_configDialog )
            ui_configWidget->m_fileChooserButton->setIcon( QIcon( m_defaultCursor ) );
        m_customCursor = m_defaultCursor;
        m_cursorPath = m_defaultCursorPath;
    }
}

void PositionMarker::loadDefaultCursor()
{
    m_defaultCursor = QPixmap( m_defaultCursorPath ).scaled( 22 * m_cursorSize, 22 * m_cursorSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
}

void PositionMarker::chooseColor()
{
    QColor initialColor;
    if( sender() == ui_configWidget->m_acColorChooserButton ) {
        initialColor = m_accuracyColor;
    }
    else if( sender() == ui_configWidget->m_trailColorChooserButton ) {
        initialColor = m_trailColor;
    }
    QColor color = QColorDialog::getColor( initialColor, NULL, 
                                           tr( "Please choose a color" ), 
                                           QColorDialog::ShowAlphaChannel );
    if( color.isValid() )
    {
        QPalette palette;
        if( sender() == ui_configWidget->m_acColorChooserButton ) {
            m_accuracyColor = color;
            palette = ui_configWidget->m_acColorChooserButton->palette();
            palette.setColor( QPalette::Button, m_accuracyColor );
            ui_configWidget->m_acColorChooserButton->setPalette( palette );
        }
        else if( sender() == ui_configWidget->m_trailColorChooserButton ) {
            m_trailColor = color;
            palette = ui_configWidget->m_trailColorChooserButton->palette();
            palette.setColor( QPalette::Button, m_trailColor );
            ui_configWidget->m_trailColorChooserButton->setPalette( palette );
        }
    }
}

void PositionMarker::resizeCursor( int step )
{
    m_cursorSize = sm_resizeSteps[step];
    float newSize = 22 * m_cursorSize;
    m_customCursor = QPixmap( m_cursorPath ).scaled( newSize, newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    ui_configWidget->m_sizeLabel->setText( tr( "Cursor Size: %1" ).arg( m_cursorSize ) );
    if( !m_customCursor.isNull() )
    {
        ui_configWidget->m_fileChooserButton->setIconSize( QSize( m_customCursor.width(), m_customCursor.height() ) );
        ui_configWidget->m_fileChooserButton->setIcon( QIcon( m_customCursor ) );
    }
    loadDefaultCursor();
}

qreal PositionMarker::zValue() const
{
    return 1.0;
}

}

#include "moc_PositionMarker.cpp"
