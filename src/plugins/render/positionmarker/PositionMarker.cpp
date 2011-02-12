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
#include <QtCore/QRect>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QColorDialog>
#include <QtGui/QTransform>

#include <cmath>

#include "ui_PositionMarkerConfigWidget.h"
#include "AbstractProjection.h"
#include "MarbleDataFacade.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "PositionTracking.h"
#include "ViewportParams.h"

namespace Marble
{

const int PositionMarker::sm_defaultSizeStep = 2;
const float PositionMarker::sm_resizeSteps[] = { 0.25, 0.5, 1.0, 2.0, 4.0 };
const int PositionMarker::sm_numResizeSteps = sizeof( sm_resizeSteps ) / sizeof( sm_resizeSteps[0] );

PositionMarker::PositionMarker ()
    : RenderPlugin(),
      m_isInitialized( false ),
      m_useCustomCursor( false ),
      m_defaultCursorPath( MarbleDirs::path( "svg/track_turtle.svg" ) ),
      m_viewport( 0 ),
      ui_configWidget( 0 ),
      m_aboutDialog( 0 ),
      m_configDialog( 0 ),
      m_settings(),
      m_visibleTrailPoints( 0 ),
      m_showTrail ( false )
{
    // Create list of rects for trail with increasing size.
    // Last element is not drawn, it only stores the next position.
    GeoDataCoordinates coordinates;
    for( int i = 0; i < sm_numTrailPoints + 1; ++i ) {
        m_trail.push_back( coordinates );
    }
    connect( this, SIGNAL( settingsChanged( QString ) ),
             this, SLOT( updateSettings() ) );
}

PositionMarker::~PositionMarker ()
{
    delete m_aboutDialog;
    delete ui_configWidget;
    delete m_configDialog;
}

QStringList PositionMarker::renderPosition() const
{
    return QStringList( "HOVERS_ABOVE_SURFACE" );
}

QString PositionMarker::renderPolicy() const
{
    return "ALWAYS";
}

QStringList PositionMarker::backendTypes() const
{
    return QStringList( "positionmarker" );
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
    return QString( "positionMarker" );
}

QString PositionMarker::description() const
{
    return tr( "draws a marker at the current position" );
}

QIcon PositionMarker::icon() const
{
    return QIcon();
}

QDialog *PositionMarker::aboutDialog() const
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Position Marker Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009, 2010 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author amanson, ewoerner, tgridel, dmarth;
        amanson.name = "Andrew Manson";
        amanson.task = tr( "Developer" );
        amanson.email = "g.real.ate@gmail.com";
        ewoerner.name = "Eckhart Woerner";
        ewoerner.task = tr( "Developer" );
        ewoerner.email = "ewoerner@kde.org";
        tgridel.name = "Thibaut Gridel";
        tgridel.task = tr( "Developer" );
        tgridel.email = "tgridel@free.fr";
        dmarth.name = "Daniel Marth";
        dmarth.task = tr( "Developer" );
        dmarth.email = "danielmarth@gmx.at";
        authors.append( amanson );
        authors.append( ewoerner );
        authors.append( tgridel );
        authors.append( dmarth );
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
}

QDialog *PositionMarker::configDialog() const
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::PositionMarkerConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        ui_configWidget->m_resizeSlider->setMaximum( sm_numResizeSteps - 1 );
        readSettings();
        connect( ui_configWidget->m_buttonBox, SIGNAL( accepted() ),
                 SLOT( writeSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( rejected() ),
                 SLOT( readSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( clicked ( QAbstractButton * ) ),
                 SLOT( evaluateClickedButton( QAbstractButton * ) ) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 SLOT( writeSettings() ) );
        connect( ui_configWidget->m_fileChooserButton, SIGNAL( clicked() ),
                 SLOT( chooseCustomCursor() ) );
        connect( ui_configWidget->m_resizeSlider, SIGNAL( valueChanged( int ) ),
                 SLOT( resizeCursor( int ) ) );
        connect( ui_configWidget->m_acColorChooserButton, SIGNAL( clicked() ),
                 SLOT( chooseColor() ) );
        connect( ui_configWidget->m_trailColorChooserButton, SIGNAL( clicked() ),
                 SLOT( chooseColor() ) );
    }
    return m_configDialog;
}

void PositionMarker::initialize()
{
    if ( dataFacade() ) {
        connect( dataFacade()->positionTracking(), SIGNAL( gpsLocation( GeoDataCoordinates,qreal ) ),
                this, SLOT( setPosition( GeoDataCoordinates ) ) );
        m_isInitialized = true;
    }
    loadDefaultCursor();
}

bool PositionMarker::isInitialized() const
{
    return m_isInitialized;
}

void PositionMarker::update()
{
    if ( !m_viewport ) {
        return;
    }
    if( ! ( m_currentPosition == m_previousPosition ) )
    {
        QPointF position;
        QPointF previousPosition;

        m_viewport->currentProjection()->screenCoordinates( m_currentPosition,
                                                            m_viewport,
                                                            position );
        m_viewport->currentProjection()->screenCoordinates( m_previousPosition,
                                                            m_viewport,
                                                            previousPosition );

        // calculate the arrow shape, oriented by the heading
        // and with constant size
        QPointF unitVector = position - previousPosition;
        // check that some screen progress was made
        if( unitVector.x() || unitVector.y() ) {
            // magnitude should be >0
            qreal magnitude = sqrt( ( unitVector.x() * unitVector.x() )
                                    + ( unitVector.y() * unitVector.y() ) );
            m_heading = atan( unitVector.y() / unitVector.x() ) + M_PI / 2.0;
            unitVector = unitVector / magnitude;
            QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x() );
            QPointF relativeLeft = - ( unitVector * 9   ) + ( unitVector2 * 9 );
            QPointF relativeRight = - ( unitVector * 9 ) - ( unitVector2 * 9 );
            QPointF relativeTip =  unitVector * 19.0 ;

            m_arrow.clear();
            m_arrow << position
                    << position + ( relativeLeft * m_cursorSize )
                    << position + ( relativeTip * m_cursorSize )
                    << position + ( relativeRight * m_cursorSize );

            m_dirtyRegion = QRegion();
            m_dirtyRegion += ( m_arrow.boundingRect().toRect() );
            m_dirtyRegion += ( m_previousArrow.boundingRect().toRect() );

            // Check if position has changed.
            if( !( m_currentPosition == m_trail[sm_numTrailPoints] ) ) {
                // Move trail forward and add new point.
                for( int i = 0;  i < sm_numTrailPoints; ++i ) {
                    m_trail[i] = m_trail[i + 1];
                }
                m_trail[sm_numTrailPoints] = m_currentPosition;
                if( m_visibleTrailPoints == 0 ) {
                    m_trail[sm_numTrailPoints - 1] = m_trail[sm_numTrailPoints];
                }
                if( m_visibleTrailPoints < sm_numTrailPoints ) {
                    ++m_visibleTrailPoints;
                }
            }
        }
    }
}

bool PositionMarker::render( GeoPainter *painter,
                           ViewportParams *viewport,
                           const QString& renderPos,
                           GeoSceneLayer * layer )
{
    Q_UNUSED( layer )
    bool const gpsActive = dataFacade()->positionTracking()->positionProviderPlugin() != 0;
    if ( gpsActive && renderPosition().contains(renderPos) )
    {
        if ( m_viewport != viewport ) {
            m_viewport = viewport;
        }
        update();
        painter->save();
        painter->autoMapQuality();

        GeoDataAccuracy accuracy = dataFacade()->positionTracking()->accuracy();
        if ( accuracy.horizontal > 0 && accuracy.horizontal < 1000 ) {
            // Paint a circle indicating the position accuracy
            painter->setPen( Qt::transparent );
            int width = qRound( accuracy.horizontal * viewport->radius() / EARTH_RADIUS );
            if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
                int arrowSize = qMax<int>( m_arrow.boundingRect().width(), m_arrow.boundingRect().height() );
                width = qMax<int>( width, arrowSize + 10 );
            }

            painter->setBrush( m_acColor );
            painter->drawEllipse( m_currentPosition, width, width );
        }

        // Draw trail if requested.
        if( m_showTrail ) {
            painter->save();
            // Use selected color to draw trail.
            painter->setBrush( m_trailColor );
            painter->setPen( m_trailColor );
            QRectF trailRect;
            QPointF trailPoint;
            float opacity = 1.0;
            for( int i = sm_numTrailPoints - 1; i >= 0 && i >= sm_numTrailPoints - m_visibleTrailPoints; --i ) {
                // Get screen coordinates from coordinates on the map.
                m_viewport->currentProjection()->screenCoordinates( m_trail[i], m_viewport, trailPoint );
                int size = ( i + 1 ) * 5;
                trailRect.setX( trailPoint.x() - size / 2.0 );
                trailRect.setY( trailPoint.y() - size / 2.0 );
                trailRect.setWidth( size );
                trailRect.setHeight( size );
                painter->setOpacity( opacity );
                painter->drawEllipse( trailRect );
                opacity -= 0.15;
            }
            painter->restore();
        }

        if( m_useCustomCursor)
        {
            QRect rect = m_arrow.boundingRect().toRect();
            if( rect.isValid() )
            {
                QTransform transform;
                transform.translate( -m_customCursor.width() / 2, -m_customCursor.height() / 2 );
                transform.rotateRadians( m_heading );
                transform.translate( m_customCursor.width() / 2, m_customCursor.height() / 2 );
                if( painter->mapQuality() == HighQuality || painter->mapQuality() == PrintQuality )
                    painter->drawPixmap( rect.topLeft(), m_customCursor.transformed( transform, Qt::SmoothTransformation ) );
                else
                    painter->drawPixmap( rect.topLeft(), m_customCursor.transformed( transform ) );
            }
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
    return m_settings;
}

void PositionMarker::setSettings( QHash<QString,QVariant> settings )
{
    if ( !settings.contains( "useCustomCursor" ) ) {
        settings.insert( "useCustomCursor", false );
    }
    if ( !settings.contains( "cursorPath" ) ) {
        settings.insert( "cursorPath", m_defaultCursorPath );
    }
    if ( !settings.contains( "cursorSize" ) ) {
        settings.insert( "cursorSize", 1.0 );
    }
    if( !settings.contains( "acColor" ) ) {
        QColor defaultColor = oxygenBrickRed4;
        bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
        defaultColor.setAlpha( smallScreen ? 80 : 40 );
        settings.insert( "acColor", defaultColor );
    }
    if( !settings.contains( "trailColor" ) ) {
        settings.insert( "trailColor", QColor( 0, 0, 255 ) );
    }
    if( !settings.contains( "showTrail" ) ) {
        settings.insert( "showTrail", false );
    }

    m_settings = settings;
    readSettings();
    emit settingsChanged( nameId() );
}

void PositionMarker::readSettings() const
{
    if ( !m_configDialog ) {
        return;
    }

    if( m_settings.value( "useCustomCursor" ).toBool() )
        ui_configWidget->m_customCursor->click();
    else
        ui_configWidget->m_originalCursor->click();

    bool found = false;
    float cursorSize = m_settings.value( "cursorSize" ).toFloat();
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
    palette.setColor( QPalette::Button, QColor( m_settings.value( "acColor" ).value<QColor>()) );
    ui_configWidget->m_acColorChooserButton->setPalette( palette );
    palette = ui_configWidget->m_trailColorChooserButton->palette();
    palette.setColor( QPalette::Button, QColor( m_settings.value( "trailColor" ).value<QColor>()) );
    ui_configWidget->m_trailColorChooserButton->setPalette( palette );
    ui_configWidget->m_trailCheckBox->setChecked( m_settings.value( "showTrail" ).toBool() );
}

void PositionMarker::writeSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    m_settings.insert( "useCustomCursor", ui_configWidget->m_customCursor->isChecked() );
    m_settings.insert( "cursorPath", m_cursorPath );
    m_settings.insert( "cursorSize", sm_resizeSteps[ui_configWidget->m_resizeSlider->value()] );
    m_settings.insert( "acColor", m_acColor );
    m_settings.insert( "trailColor", m_trailColor );
    m_settings.insert( "showTrail", ui_configWidget->m_trailCheckBox->isChecked() );

    emit settingsChanged( nameId() );
}

void PositionMarker::updateSettings()
{
    m_useCustomCursor = m_settings.value( "useCustomCursor" ).toBool();
    m_cursorPath = m_settings.value( "cursorPath" ).toString();
    m_cursorSize =  m_settings.value( "cursorSize" ).toFloat();
    loadCustomCursor( m_cursorPath, m_useCustomCursor );
    m_acColor = m_settings.value( "acColor" ).value<QColor>();
    m_trailColor = m_settings.value( "trailColor" ).value<QColor>();
    m_showTrail = m_settings.value( "showTrail" ).toBool();
}

void PositionMarker::setPosition( const GeoDataCoordinates &position )
{
    m_previousPosition = m_currentPosition;
    m_currentPosition = position;
    if ( m_viewport->viewLatLonAltBox().contains( m_currentPosition ) )
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
        QMessageBox::warning( NULL, tr( "Position Marker Plugin" ), tr( "Unable to load custom cursor, default cursor will be used. "
                                                       "Make sure this is a valid image file." ), QMessageBox::Ok );
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
        initialColor = m_acColor;
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
            m_acColor = color;
            palette = ui_configWidget->m_acColorChooserButton->palette();
            palette.setColor( QPalette::Button, m_acColor );
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

void PositionMarker::evaluateClickedButton( QAbstractButton *button )
{
    if( button == ui_configWidget->m_buttonBox->button( QDialogButtonBox::RestoreDefaults ) )
        restoreDefaultSettings();
}

}

Q_EXPORT_PLUGIN2( PositionMarker, Marble::PositionMarker )

#include "PositionMarker.moc"
