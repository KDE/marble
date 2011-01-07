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
#include "AdjustNavigation.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
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

namespace Marble
{

class CurrentLocationWidgetPrivate
{
 public:
    Ui::CurrentLocationWidget      m_currentLocationUi;
    MarbleWidget                  *m_widget;
    AdjustNavigation *m_adjustNavigation;

    QList<PositionProviderPlugin*> m_positionProviderPlugins;
    GeoDataCoordinates             m_currentPosition;

    MarbleLocale*                  m_locale;

    void adjustPositionTrackingStatus( PositionProviderStatus status );
    void changePositionProvider( const QString &provider );
    void centerOnCurrentLocation();
    void updateRecenterComboBox( int centerMode );
    void updateAutoZoomCheckBox( bool autoZoom );
    void updateActivePositionProvider( PositionProviderPlugin* );
    void saveTrack();
    void clearTrack();
};

CurrentLocationWidget::CurrentLocationWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new CurrentLocationWidgetPrivate() )
{
    d->m_currentLocationUi.setupUi( this );

    d->m_locale = MarbleGlobal::getInstance()->locale();

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

    d->m_adjustNavigation = new AdjustNavigation( d->m_widget, this );
    d->m_widget->model()->routingManager()->setAdjustNavigation( d->m_adjustNavigation );

    PluginManager* pluginManager = d->m_widget->model()->pluginManager();
    d->m_positionProviderPlugins = pluginManager->createPositionProviderPlugins();
    foreach( const PositionProviderPlugin *plugin, d->m_positionProviderPlugins ) {
        d->m_currentLocationUi.positionTrackingComboBox->addItem( plugin->guiString() );
    }
    if ( d->m_positionProviderPlugins.isEmpty() ) {
        d->m_currentLocationUi.positionTrackingComboBox->setEnabled( false );
        QString html = "<p>No Position Tracking Plugin installed.</p>";
        d->m_currentLocationUi.locationLabel->setText( html );
        d->m_currentLocationUi.locationLabel->setEnabled ( true );
        d->m_currentLocationUi.showTrackCheckBox->setEnabled( false );
        d->m_currentLocationUi.saveTrackPushButton->setEnabled( false );
        d->m_currentLocationUi.clearTrackPushButton->setEnabled( false );
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

    disconnect( d->m_adjustNavigation, SIGNAL( recenterModeChanged( int ) ),
             this, SLOT( updateRecenterComboBox( int ) ) );
    disconnect( d->m_adjustNavigation, SIGNAL( autoZoomToggled( bool ) ),
             this, SLOT( updateAutoZoomCheckBox( bool ) ) );

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

    connect( d->m_adjustNavigation, SIGNAL( recenterModeChanged( int ) ),
             this, SLOT( updateRecenterComboBox( int ) ) );
    connect( d->m_adjustNavigation, SIGNAL( autoZoomToggled( bool ) ),
             this, SLOT( updateAutoZoomCheckBox( bool ) ) );
    connect (d->m_currentLocationUi.showTrackCheckBox, SIGNAL( clicked(bool) ),
             d->m_widget->model()->positionTracking(), SLOT( setTrackVisible(bool) ));
    connect (d->m_currentLocationUi.showTrackCheckBox, SIGNAL( clicked(bool) ),
             d->m_widget, SLOT(repaint()));
    if ( d->m_widget->model()->positionTracking()->trackVisible() ) {
        d->m_currentLocationUi.showTrackCheckBox->setCheckState(Qt::Checked);
    }
    connect ( d->m_currentLocationUi.saveTrackPushButton, SIGNAL( clicked(bool)),
              this, SLOT(saveTrack()));
    connect (d->m_currentLocationUi.clearTrackPushButton, SIGNAL( clicked(bool)),
             this, SLOT(clearTrack()));
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

void CurrentLocationWidget::receiveGpsCoordinates( const GeoDataCoordinates &position, qreal speed )
{
    d->m_currentPosition = position;
    QString unitString;
    QString speedString;
    QString distanceUnitString;
    QString distanceString;
    qreal unitSpeed = 0.0;
    qreal distance = 0.0;

    QString html = "<html><body>";
    html += "<table cellspacing=\"2\" cellpadding=\"2\">";
    html += "<tr><td>Longitude</td><td><a href=\"http://edu.kde.org/marble\">%1</a></td></tr>";
    html += "<tr><td>Latitude</td><td><a href=\"http://edu.kde.org/marble\">%2</a></td></tr>";
    html += "<tr><td>Altitude</td><td>%3</td></tr>";
    html += "<tr><td>Speed</td><td>%4</td></tr>";
    html += "</table>";
    html += "</body></html>";

    switch ( d->m_locale->measureSystem() ) {
        case Metric:
        //kilometers per hour
        unitString = tr("km/h");
        unitSpeed = speed * HOUR2SEC * METER2KM;
        distanceUnitString = tr("m");
        distance = position.altitude();
        break;

        case Imperial:
        //miles per hour
        unitString = tr("m/h");
        unitSpeed = speed * HOUR2SEC * METER2KM * KM2MI;
        distanceUnitString = tr("ft");
        distance = position.altitude() * M2FT;
        break;
    }
    // TODO read this value from the incoming signal
    speedString = QLocale::system().toString( unitSpeed, 'f', 1);
    distanceString = QString( "%1 %2" ).arg( distance, 0, 'f', 1, QChar(' ') ).arg( distanceUnitString );

    html = html.arg( position.lonToString() ).arg( position.latToString() );
    html = html.arg( distanceString ).arg( speedString + ' ' + unitString );
    d->m_currentLocationUi.locationLabel->setText( html );
    d->m_currentLocationUi.showTrackCheckBox->setEnabled( true );
    d->m_currentLocationUi.saveTrackPushButton->setEnabled( true );
    d->m_currentLocationUi.clearTrackPushButton->setEnabled( true );
}

void CurrentLocationWidgetPrivate::changePositionProvider( const QString &provider )
{
    bool hasProvider = ( provider != QObject::tr("Disabled") );

    if ( hasProvider ) {
        foreach( PositionProviderPlugin* plugin, m_positionProviderPlugins ) {
            if ( plugin->guiString() == provider ) {
                m_currentLocationUi.locationLabel->setEnabled( true );
                PositionProviderPlugin* instance = plugin->newInstance();
                PositionTracking *tracking = m_widget->model()->positionTracking();
                tracking->setPositionProviderPlugin( instance );
                m_widget->setShowGps( true );
                m_widget->update();
                return;
            }
        }
    }
    else {
        m_currentLocationUi.locationLabel->setEnabled( false );
        m_widget->setShowGps( false );
        m_widget->model()->positionTracking()->setPositionProviderPlugin( 0 );
        m_widget->update();
    }
}

void CurrentLocationWidget::setRecenterMode( int centerMode )
{
    d->m_adjustNavigation->setRecenter( centerMode );
}

void CurrentLocationWidget::setAutoZoom( bool autoZoom )
{
    d->m_adjustNavigation->setAutoZoom( autoZoom );
}

void CurrentLocationWidgetPrivate::updateAutoZoomCheckBox( bool autoZoom )
{
    m_currentLocationUi.autoZoomCheckBox->setChecked( autoZoom );
}

void CurrentLocationWidgetPrivate::updateRecenterComboBox( int centerMode )
{
    m_currentLocationUi.recenterComboBox->setCurrentIndex( centerMode );
}

void CurrentLocationWidgetPrivate::centerOnCurrentLocation()
{
    m_widget->centerOn(m_currentPosition, true);
}

void CurrentLocationWidgetPrivate::saveTrack()
{
    static QString s_dirName = QDir::homePath();
    QString fileName = QFileDialog::getSaveFileName(m_widget, QObject::tr("Save Track"), // krazy:exclude=qclasses
                                                    s_dirName.append('/' + QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss") + ".kml"),
                            QObject::tr("KML File (*.kml)"));
    if ( !fileName.isEmpty() ) {
        QFileInfo file( fileName );
        s_dirName = file.absolutePath();
        m_widget->model()->positionTracking()->saveTrack( fileName );
    }
}

void CurrentLocationWidgetPrivate::clearTrack()
{
    m_widget->model()->positionTracking()->clearTrack();
    m_widget->repaint();
    m_currentLocationUi.saveTrackPushButton->setEnabled( false );
    m_currentLocationUi.clearTrackPushButton->setEnabled( false );
}

}

#include "CurrentLocationWidget.moc"
