//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "CurrentLocationWidget.h"

// Marble
#include "AutoNavigation.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetPopupMenu.h"
#include "GeoDataCoordinates.h"
#include "PositionProviderPlugin.h"
#include "PluginManager.h"
#include "PositionTracking.h"
#include "routing/RoutingManager.h"

using namespace Marble;
/* TRANSLATOR Marble::CurrentLocationWidget */

// Ui
#include "ui_CurrentLocationWidget.h"

#include <QtCore/QDateTime>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

namespace Marble
{

class CurrentLocationWidgetPrivate
{
 public:
    CurrentLocationWidgetPrivate();

    Ui::CurrentLocationWidget      m_currentLocationUi;
    MarbleWidget                  *m_widget;
    AutoNavigation *m_adjustNavigation;

    QList<const PositionProviderPlugin*> m_positionProviderPlugins;
    GeoDataCoordinates             m_currentPosition;

    QString m_lastOpenPath;
    QString m_lastSavePath;

    void receiveGpsCoordinates( const GeoDataCoordinates &position, qreal speed );
    void adjustPositionTrackingStatus( PositionProviderStatus status );
    void changePositionProvider( const QString &provider );
    void trackPlacemark();
    void centerOnCurrentLocation();
    void updateRecenterComboBox( AutoNavigation::CenterMode centerMode );
    void updateAutoZoomCheckBox( bool autoZoom );
    void updateActivePositionProvider( PositionProviderPlugin* );
    void saveTrack();
    void openTrack();
    void clearTrack();
};

CurrentLocationWidgetPrivate::CurrentLocationWidgetPrivate()
    : m_widget( 0 ),
      m_adjustNavigation( 0 ),
      m_positionProviderPlugins(),
      m_currentPosition(),
      m_lastOpenPath(),
      m_lastSavePath()
{
}

CurrentLocationWidget::CurrentLocationWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new CurrentLocationWidgetPrivate() )
{
    d->m_currentLocationUi.setupUi( this );

    connect( d->m_currentLocationUi.recenterComboBox, SIGNAL ( currentIndexChanged( int ) ),
            this, SLOT( setRecenterMode( int ) ) );

    connect( d->m_currentLocationUi.autoZoomCheckBox, SIGNAL( clicked( bool ) ),
             this, SLOT( setAutoZoom( bool ) ) );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    d->m_currentLocationUi.positionTrackingComboBox->setVisible( !smallScreen );
    d->m_currentLocationUi.locationLabel->setVisible( !smallScreen );
}

CurrentLocationWidget::~CurrentLocationWidget()
{
    delete d;
}

void CurrentLocationWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;

    d->m_adjustNavigation = new AutoNavigation( widget->model(), widget->viewport(), this );
    d->m_widget->model()->routingManager()->setAutoNavigation( d->m_adjustNavigation );

    const PluginManager* pluginManager = d->m_widget->model()->pluginManager();
    d->m_positionProviderPlugins = pluginManager->positionProviderPlugins();
    foreach( const PositionProviderPlugin *plugin, d->m_positionProviderPlugins ) {
        d->m_currentLocationUi.positionTrackingComboBox->addItem( plugin->guiString() );
    }
    if ( d->m_positionProviderPlugins.isEmpty() ) {
        d->m_currentLocationUi.positionTrackingComboBox->setEnabled( false );
        QString html = "<p>No Position Tracking Plugin installed.</p>";
        d->m_currentLocationUi.locationLabel->setText( html );
        d->m_currentLocationUi.locationLabel->setEnabled ( true );
        bool const hasTrack = !d->m_widget->model()->positionTracking()->isTrackEmpty();
        d->m_currentLocationUi.showTrackCheckBox->setEnabled( hasTrack );
        d->m_currentLocationUi.saveTrackButton->setEnabled( hasTrack );
        d->m_currentLocationUi.clearTrackButton->setEnabled( hasTrack );
    }

    //disconnect CurrentLocation Signals
    disconnect( d->m_widget->model()->positionTracking(),
             SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
             this, SLOT( receiveGpsCoordinates( GeoDataCoordinates, qreal ) ) );
    disconnect( d->m_widget->model()->positionTracking(),
             SIGNAL( positionProviderPluginChanged( PositionProviderPlugin* ) ),
             this, SLOT( updateActivePositionProvider( PositionProviderPlugin* ) ) );
    disconnect( d->m_currentLocationUi.positionTrackingComboBox, SIGNAL( currentIndexChanged( QString ) ),
             this, SLOT( changePositionProvider( QString ) ) );
    disconnect( d->m_currentLocationUi.locationLabel, SIGNAL( linkActivated( QString ) ),
             this, SLOT( centerOnCurrentLocation() ) );
    disconnect( d->m_widget->model()->positionTracking(),
             SIGNAL( statusChanged( PositionProviderStatus) ),this,
             SLOT( adjustPositionTrackingStatus( PositionProviderStatus) ) );

    disconnect( d->m_adjustNavigation, SIGNAL( recenterModeChanged( AutoNavigation::CenterMode ) ),
             this, SLOT( updateRecenterComboBox( AutoNavigation::CenterMode ) ) );
    disconnect( d->m_adjustNavigation, SIGNAL( autoZoomToggled( bool ) ),
             this, SLOT( updateAutoZoomCheckBox( bool ) ) );
    disconnect( d->m_widget->model(), SIGNAL( trackedPlacemarkChanged( const GeoDataPlacemark* ) ),
             this, SLOT( trackPlacemark() ) );

    //connect CurrentLoctaion signals
    connect( d->m_widget->model()->positionTracking(),
             SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
             this, SLOT( receiveGpsCoordinates( GeoDataCoordinates, qreal ) ) );
    connect( d->m_widget->model()->positionTracking(),
             SIGNAL( positionProviderPluginChanged( PositionProviderPlugin* ) ),
             this, SLOT( updateActivePositionProvider( PositionProviderPlugin* ) ) );
    d->updateActivePositionProvider( d->m_widget->model()->positionTracking()->positionProviderPlugin() );
    connect( d->m_currentLocationUi.positionTrackingComboBox, SIGNAL( currentIndexChanged( QString ) ),
             this, SLOT( changePositionProvider( QString ) ) );
    connect( d->m_currentLocationUi.locationLabel, SIGNAL( linkActivated( QString ) ),
             this, SLOT( centerOnCurrentLocation() ) );
    connect( d->m_widget->model()->positionTracking(),
             SIGNAL( statusChanged( PositionProviderStatus) ), this,
             SLOT( adjustPositionTrackingStatus( PositionProviderStatus) ) );

    connect( d->m_adjustNavigation, SIGNAL( recenterModeChanged( AutoNavigation::CenterMode ) ),
             this, SLOT( updateRecenterComboBox( AutoNavigation::CenterMode ) ) );
    connect( d->m_adjustNavigation, SIGNAL( autoZoomToggled( bool ) ),
             this, SLOT( updateAutoZoomCheckBox( bool ) ) );
    connect( d->m_adjustNavigation, SIGNAL( zoomIn( FlyToMode ) ),
             d->m_widget, SLOT( zoomIn( FlyToMode ) ) );
    connect( d->m_adjustNavigation, SIGNAL( zoomOut( FlyToMode ) ),
             d->m_widget, SLOT( zoomOut( FlyToMode ) ) );
    connect( d->m_adjustNavigation, SIGNAL( centerOn( const GeoDataCoordinates &, bool ) ),
             d->m_widget, SLOT( centerOn( const GeoDataCoordinates &, bool ) ) );

    connect( d->m_widget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox ) ),
             d->m_adjustNavigation, SLOT( inhibitAutoAdjustments() ) );

    connect (d->m_currentLocationUi.showTrackCheckBox, SIGNAL( clicked(bool) ),
             d->m_widget->model()->positionTracking(), SLOT( setTrackVisible(bool) ));
    connect (d->m_currentLocationUi.showTrackCheckBox, SIGNAL( clicked(bool) ),
             d->m_widget, SLOT(update()));
    if ( d->m_widget->model()->positionTracking()->trackVisible() ) {
        d->m_currentLocationUi.showTrackCheckBox->setCheckState(Qt::Checked);
    }
    connect ( d->m_currentLocationUi.saveTrackButton, SIGNAL( clicked(bool)),
              this, SLOT(saveTrack()));
    connect ( d->m_currentLocationUi.openTrackButton, SIGNAL( clicked(bool)),
              this, SLOT(openTrack()));
    connect (d->m_currentLocationUi.clearTrackButton, SIGNAL( clicked(bool)),
             this, SLOT(clearTrack()));
    connect( d->m_widget->model(), SIGNAL( trackedPlacemarkChanged( const GeoDataPlacemark* ) ),
             this, SLOT( trackPlacemark() ) );
}

void CurrentLocationWidgetPrivate::adjustPositionTrackingStatus( PositionProviderStatus status )
{
    if ( status == PositionProviderStatusAvailable ) {
        return;
    }

    QString html = "<html><body><p>";

    switch ( status ) {
        case PositionProviderStatusUnavailable:
        html += QObject::tr( "Waiting for current location information..." );
            break;
        case PositionProviderStatusAcquiring:
            html += QObject::tr( "Initializing current location service..." );
            break;
        case PositionProviderStatusAvailable:
            Q_ASSERT( false );
            break;
        case PositionProviderStatusError:
            html += QObject::tr( "Error when determining current location: " );
            html += m_widget->model()->positionTracking()->error();
            break;
    }

    html += "</p></body></html>";
    m_currentLocationUi.locationLabel->setEnabled( true );
    m_currentLocationUi.locationLabel->setText( html );
}

void CurrentLocationWidgetPrivate::updateActivePositionProvider( PositionProviderPlugin *plugin )
{
    m_currentLocationUi.positionTrackingComboBox->blockSignals( true );
    if ( !plugin ) {
        m_currentLocationUi.positionTrackingComboBox->setCurrentIndex( 0 );
    } else {
        for( int i=0; i<m_currentLocationUi.positionTrackingComboBox->count(); ++i ) {
            if ( m_currentLocationUi.positionTrackingComboBox->itemText( i ) == plugin->guiString() ) {
                m_currentLocationUi.positionTrackingComboBox->setCurrentIndex( i );
                break;
            }
        }
    }
    m_currentLocationUi.positionTrackingComboBox->blockSignals( false );
    m_currentLocationUi.recenterLabel->setEnabled( plugin );
    m_currentLocationUi.recenterComboBox->setEnabled( plugin );
    m_currentLocationUi.autoZoomCheckBox->setEnabled( plugin );

}

void CurrentLocationWidgetPrivate::receiveGpsCoordinates( const GeoDataCoordinates &position, qreal speed )
{
    m_currentPosition = position;
    QString unitString;
    QString altitudeUnitString;
    QString distanceUnitString;
    qreal unitSpeed = 0.0;
    qreal altitude = 0.0;
    qreal length = m_widget->model()->positionTracking()->length( m_widget->model()->planetRadius() );

    QString html = "<html><body>";
    html += "<table cellspacing=\"2\" cellpadding=\"2\">";
    html += "<tr><td>Longitude</td><td><a href=\"http://edu.kde.org/marble\">%1</a></td></tr>";
    html += "<tr><td>Latitude</td><td><a href=\"http://edu.kde.org/marble\">%2</a></td></tr>";
    html += "<tr><td>Altitude</td><td>%3</td></tr>";
    html += "<tr><td>Speed</td><td>%4</td></tr>";
    html += "<tr><td>Distance</td><td>%5</td></tr>";
    html += "</table>";
    html += "</body></html>";

    switch ( MarbleGlobal::getInstance()->locale()->measurementSystem() ) {
        case QLocale::MetricSystem:
        //kilometers per hour
        unitString = QObject::tr("km/h");
        unitSpeed = speed * HOUR2SEC * METER2KM;
        altitudeUnitString = QObject::tr("m");
        distanceUnitString = QObject::tr("m");
        if ( length > 1000.0 ) {
            length /= 1000.0;
            distanceUnitString = QObject::tr("km");
        }
        altitude = position.altitude();
        break;

        case QLocale::ImperialSystem:
        //miles per hour
        unitString = QObject::tr("m/h");
        unitSpeed = speed * HOUR2SEC * METER2KM * KM2MI;
        altitudeUnitString = QObject::tr("ft");
        distanceUnitString = QObject::tr("ft");
        altitude = position.altitude() * M2FT;
        length *= M2FT;
        break;
    }
    // TODO read this value from the incoming signal
    const QString speedString = QLocale::system().toString( unitSpeed, 'f', 1);
    const QString altitudeString = QString( "%1 %2" ).arg( altitude, 0, 'f', 1, QChar(' ') ).arg( altitudeUnitString );
    const QString distanceString = QString( "%1 %2" ).arg( length, 0, 'f', 1, QChar(' ') ).arg( distanceUnitString );

    html = html.arg( position.lonToString() ).arg( position.latToString() );
    html = html.arg( altitudeString ).arg( speedString + ' ' + unitString );
    html = html.arg( distanceString );
    m_currentLocationUi.locationLabel->setText( html );
    m_currentLocationUi.showTrackCheckBox->setEnabled( true );
    m_currentLocationUi.saveTrackButton->setEnabled( true );
    m_currentLocationUi.clearTrackButton->setEnabled( true );
}

void CurrentLocationWidgetPrivate::changePositionProvider( const QString &provider )
{
    bool hasProvider = ( provider != QObject::tr("Disabled") );

    if ( hasProvider ) {
        foreach( const PositionProviderPlugin* plugin, m_positionProviderPlugins ) {
            if ( plugin->guiString() == provider ) {
                m_currentLocationUi.locationLabel->setEnabled( true );
                PositionProviderPlugin* instance = plugin->newInstance();
                PositionTracking *tracking = m_widget->model()->positionTracking();
                instance->setMarbleModel( m_widget->model() );
                tracking->setPositionProviderPlugin( instance );
                m_widget->update();
                return;
            }
        }
    }
    else {
        m_currentLocationUi.locationLabel->setEnabled( false );
        m_widget->model()->positionTracking()->setPositionProviderPlugin( 0 );
        m_widget->update();
    }
}

void CurrentLocationWidgetPrivate::trackPlacemark()
{
    changePositionProvider( "Placemark" );
    m_adjustNavigation->setRecenter( AutoNavigation::AlwaysRecenter );
}

void CurrentLocationWidget::setRecenterMode( int mode )
{
    if ( mode >= 0 && mode <= AutoNavigation::RecenterOnBorder ) {
        AutoNavigation::CenterMode centerMode = ( AutoNavigation::CenterMode ) mode;
        d->m_adjustNavigation->setRecenter( centerMode );
    }
}

void CurrentLocationWidget::setAutoZoom( bool autoZoom )
{
    d->m_adjustNavigation->setAutoZoom( autoZoom );
}

void CurrentLocationWidgetPrivate::updateAutoZoomCheckBox( bool autoZoom )
{
    m_currentLocationUi.autoZoomCheckBox->setChecked( autoZoom );
}

void CurrentLocationWidgetPrivate::updateRecenterComboBox( AutoNavigation::CenterMode centerMode )
{
    m_currentLocationUi.recenterComboBox->setCurrentIndex( centerMode );
}

void CurrentLocationWidgetPrivate::centerOnCurrentLocation()
{
    m_widget->centerOn(m_currentPosition, true);
}

void CurrentLocationWidgetPrivate::saveTrack()
{
    QString suggested = m_lastSavePath;
    QString fileName = QFileDialog::getSaveFileName(m_widget, QObject::tr("Save Track"), // krazy:exclude=qclasses
                                                    suggested.append('/' + QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss") + ".kml"),
                            QObject::tr("KML File (*.kml)"));
    if ( fileName.isEmpty() ) {
        return;
    }
    if ( !fileName.endsWith(".kml", Qt::CaseInsensitive) ) {
        fileName.append( ".kml" );
    }
    QFileInfo file( fileName );
    m_lastSavePath = file.absolutePath();
    m_widget->model()->positionTracking()->saveTrack( fileName );
}

void CurrentLocationWidgetPrivate::openTrack()
{
    QString suggested = m_lastOpenPath;
    QString fileName = QFileDialog::getOpenFileName( m_widget, QObject::tr("Open Track"), // krazy:exclude=qclasses
                                                    suggested, QObject::tr("KML File (*.kml)"));
    if ( !fileName.isEmpty() ) {
        QFileInfo file( fileName );
        m_lastOpenPath = file.absolutePath();
        m_widget->model()->addGeoDataFile( fileName );
    }
}

void CurrentLocationWidgetPrivate::clearTrack()
{
    const int result = QMessageBox::question( m_widget,
                                              QObject::tr( "Clear current track" ),
                                              QObject::tr( "Are you sure you want to clear the current track?" ),
                                              QMessageBox::Yes,
                                              QMessageBox::No );

    if ( result == QMessageBox::Yes ) {
        m_widget->model()->positionTracking()->clearTrack();
        m_widget->update();
        m_currentLocationUi.saveTrackButton->setEnabled( false );
        m_currentLocationUi.clearTrackButton->setEnabled( false );
    }
}

AutoNavigation::CenterMode CurrentLocationWidget::recenterMode() const
{
    return d->m_adjustNavigation->recenterMode();
}

bool CurrentLocationWidget::autoZoom() const
{
    return d->m_adjustNavigation->autoZoom();
}

bool CurrentLocationWidget::trackVisible() const
{
    return d->m_widget->model()->positionTracking()->trackVisible();
}

QString CurrentLocationWidget::lastOpenPath() const
{
    return d->m_lastOpenPath;
}

QString CurrentLocationWidget::lastSavePath() const
{
    return d->m_lastSavePath;
}

void CurrentLocationWidget::setTrackVisible( bool visible )
{
    d->m_currentLocationUi.showTrackCheckBox->setChecked( visible );
    d->m_widget->model()->positionTracking()->setTrackVisible( visible );
}

void CurrentLocationWidget::setLastOpenPath( const QString &path )
{
    d->m_lastOpenPath = path;
}

void CurrentLocationWidget::setLastSavePath( const QString &path )
{
    d->m_lastSavePath = path;
}

}

#include "CurrentLocationWidget.moc"
