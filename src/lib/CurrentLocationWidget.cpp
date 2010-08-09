//
// This file is part of the Marble Desktop Globe.
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
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "GeoDataCoordinates.h"
#include "PositionProviderPlugin.h"
#include "PluginManager.h"
#include "gps/PositionTracking.h"

using namespace Marble;
// Ui
#include "ui_CurrentLocationWidget.h"

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
};

CurrentLocationWidget::CurrentLocationWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new CurrentLocationWidgetPrivate() )
{
    d->m_currentLocationUi.setupUi( this );

    d->m_locale = MarbleGlobal::getInstance()->locale();

    connect( d->m_currentLocationUi.recenterComboBox, SIGNAL ( activated ( int ) ),
            this, SLOT( setRecenter( int ) ) );

    connect(  d->m_currentLocationUi.autoZoomCheckBox, SIGNAL( clicked( bool ) ),
             this, SLOT( setAutoZoom( bool ) ) );
}

CurrentLocationWidget::~CurrentLocationWidget()
{
    delete d;
}

void CurrentLocationWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;

    d->m_adjustNavigation = new AdjustNavigation( d->m_widget, this );

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
    }

    //connect CurrentLoctaion signals
    connect( d->m_widget->model()->positionTracking(),
             SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
             this, SLOT( receiveGpsCoordinates( GeoDataCoordinates, qreal ) ) );
    connect( d->m_currentLocationUi.positionTrackingComboBox, SIGNAL( currentIndexChanged( QString ) ),
             this, SLOT( changePositionProvider( QString ) ) );
    connect( d->m_currentLocationUi.locationLabel, SIGNAL( linkActivated( QString ) ),
             this, SLOT( centerOnCurrentLocation() ) );
    connect( d->m_widget->model()->positionTracking(),
             SIGNAL( statusChanged( PositionProviderStatus) ), this,
             SLOT( adjustPositionTrackingStatus( PositionProviderStatus) ) );
}

void CurrentLocationWidget::adjustPositionTrackingStatus( PositionProviderStatus status )
{
    if ( status == PositionProviderStatusAvailable ) {
        return;
    }

    QString html = "<html><body><p>";

    switch ( status ) {
        case PositionProviderStatusUnavailable:
            html += tr( "Waiting for current location information..." );
            break;
        case PositionProviderStatusAcquiring:
            html += tr( "Initializing current location service..." );
            break;
        case PositionProviderStatusAvailable:
            Q_ASSERT( false );
            break;
        case PositionProviderStatusError:
            html += tr( "Error when determining current location: " );
            html += d->m_widget->model()->positionTracking()->error();
            break;
    }

    html += "</p></body></html>";
    d->m_currentLocationUi.locationLabel->setEnabled( true );
    d->m_currentLocationUi.locationLabel->setText( html );
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
}

void CurrentLocationWidget::changePositionProvider( const QString &provider )
{
    if ( provider == tr("Disabled") ) {
        d->m_currentLocationUi.locationLabel->setEnabled( false );
        d->m_widget->map()->setShowGps( false );
        d->m_widget->model()->positionTracking()->setPositionProviderPlugin( 0 );
        d->m_widget->update();
    }
    else {
        foreach( PositionProviderPlugin* plugin, d->m_positionProviderPlugins ) {
            if ( plugin->guiString() == provider ) {
                d->m_currentLocationUi.locationLabel->setEnabled( true );
                PositionProviderPlugin* instance = plugin->newInstance();
                PositionTracking *tracking = d->m_widget->model()->positionTracking();
                tracking->setPositionProviderPlugin( instance );
                d->m_widget->map()->setShowGps( true );
                d->m_widget->update();
                return;
            }
        }
    }
}

void CurrentLocationWidget::setRecenter( int centerMode )
{
    d->m_adjustNavigation->setRecenter( centerMode );
}

void CurrentLocationWidget::setAutoZoom( bool autoZoom )
{
    d->m_adjustNavigation->setAutoZoom( autoZoom );
}

void CurrentLocationWidget::centerOnCurrentLocation()
{
    d->m_widget->centerOn(d->m_currentPosition, true);
}

}

#include "CurrentLocationWidget.moc"
