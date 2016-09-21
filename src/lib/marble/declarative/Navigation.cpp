//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "Navigation.h"

#include "Planet.h"
#include "MarbleModel.h"
#include "MarbleQuickItem.h"
#include "routing/Route.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "PositionTracking.h"
#include "MarbleMath.h"
#include "AutoNavigation.h"
#include "routing/VoiceNavigationModel.h"
#include "ViewportParams.h"
#include "GeoDataAccuracy.h"

namespace Marble {

class NavigationPrivate
{
public:
    NavigationPrivate();

    MarbleQuickItem * m_marbleQuickItem;

    bool m_muted;

    RouteSegment m_currentSegment;

    AutoNavigation* m_autoNavigation;

    VoiceNavigationModel m_voiceNavigation;

    qreal m_nextInstructionDistance;

    qreal m_destinationDistance;

    double m_screenAccuracy;
    QPointF m_screenPosition;

    RouteSegment nextRouteSegment();

    void updateNextInstructionDistance( const Route &route );

    MarbleModel * model() const;

    QPointF positionOnRoute() const;
    QPointF currentPosition() const;

    RouteSegment m_secondLastSegment;
    RouteSegment m_lastSegment;

};

NavigationPrivate::NavigationPrivate() :
    m_marbleQuickItem( nullptr ), m_muted( false ), m_autoNavigation( 0 ), m_nextInstructionDistance( 0.0 ),
    m_destinationDistance( 0.0 ), m_screenAccuracy(0)
{
    // nothing to do
}

void NavigationPrivate::updateNextInstructionDistance( const Route &route )
{
    const GeoDataCoordinates position = route.position();
    const GeoDataCoordinates interpolated = route.positionOnRoute();
    const GeoDataCoordinates onRoute = route.currentWaypoint();

    qreal planetRadius = 0;
    if (model()){
        planetRadius = model()->planet()->radius();
    }
    qreal distance = planetRadius * ( distanceSphere( position, interpolated ) + distanceSphere( interpolated, onRoute ) );
    qreal remaining = 0.0;
    const RouteSegment &segment = route.currentSegment();
    for ( int i=0; i<segment.path().size(); ++i ) {
        if ( segment.path()[i] == onRoute ) {
            distance += segment.path().length( planetRadius, i );
            break;
        }
    }

    bool upcoming = false;
    for ( int i=0; i<route.size(); ++i ) {
        const RouteSegment &segment = route.at( i );

        if ( upcoming ) {
            remaining += segment.path().length( planetRadius );
        }

        if ( segment == route.currentSegment() ) {
            upcoming = true;
        }
    }

    m_nextInstructionDistance = distance;
    m_destinationDistance = distance + remaining;
}

MarbleModel * NavigationPrivate::model() const
{
    return m_marbleQuickItem ? m_marbleQuickItem->model() : nullptr;
}

RouteSegment NavigationPrivate::nextRouteSegment()
{
    // Not using m_currentSegment on purpose
    return m_marbleQuickItem ? model()->routingManager()->routingModel()->route().currentSegment().nextRouteSegment() : RouteSegment();
}

Navigation::Navigation( QObject* parent) :
    QObject( parent ), d( new NavigationPrivate )
{
    connect( &d->m_voiceNavigation, SIGNAL(instructionChanged()), this, SIGNAL(voiceNavigationAnnouncementChanged()) );
}

Navigation::~Navigation()
{
    delete d;
}


bool Navigation::guidanceModeEnabled() const
{
    return d->m_marbleQuickItem ? d->model()->routingManager()->guidanceModeEnabled() : false;
}

void Navigation::setGuidanceModeEnabled( bool enabled )
{
    if ( d->m_marbleQuickItem ) {
        d->model()->routingManager()->setGuidanceModeEnabled( enabled );
        d->m_autoNavigation->setAutoZoom( enabled );
        d->m_autoNavigation->setRecenter( enabled ? AutoNavigation::RecenterOnBorder : AutoNavigation::DontRecenter );

        if ( enabled && !d->m_muted ) {
            //d->m_audio.announceStart();
        }
    }
}

bool Navigation::muted() const
{
    return d->m_muted;
}

void Navigation::setMuted(bool enabled)
{
    d->m_muted = enabled;
}

QString Navigation::nextInstructionText() const
{
    return d->nextRouteSegment().maneuver().instructionText();
}

QString Navigation::nextRoad() const
{
    return d->nextRouteSegment().maneuver().roadName();
}

QString Navigation::nextInstructionImage() const
{
    switch ( d->nextRouteSegment().maneuver().direction() ) {
    case Maneuver::Continue:             return QStringLiteral("qrc:/marble/turn-continue.svg");
    case Maneuver::Merge:                return QStringLiteral("qrc:/marble/turn-merge.svg");
    case Maneuver::Straight:             return QStringLiteral("qrc:/marble/turn-continue.svg");
    case Maneuver::SlightRight:          return QStringLiteral("qrc:/marble/turn-slight-right.svg");
    case Maneuver::Right:                return QStringLiteral("qrc:/marble/turn-right.svg");
    case Maneuver::SharpRight:           return QStringLiteral("qrc:/marble/turn-sharp-right.svg");
    case Maneuver::TurnAround:           return QStringLiteral("qrc:/marble/turn-around.svg");
    case Maneuver::SharpLeft:            return QStringLiteral("qrc:/marble/turn-sharp-left.svg");
    case Maneuver::Left:                 return QStringLiteral("qrc:/marble/turn-left.svg");
    case Maneuver::SlightLeft:           return QStringLiteral("qrc:/marble/turn-slight-left.svg");
    case Maneuver::RoundaboutFirstExit:  return QStringLiteral("qrc:/marble/turn-roundabout-first.svg");
    case Maneuver::RoundaboutSecondExit: return QStringLiteral("qrc:/marble/turn-roundabout-second.svg");
    case Maneuver::RoundaboutThirdExit:  return QStringLiteral("qrc:/marble/turn-roundabout-third.svg");
    case Maneuver::RoundaboutExit:       return QStringLiteral("qrc:/marble/turn-roundabout-far.svg");
    case Maneuver::ExitLeft:             return QStringLiteral("qrc:/marble/turn-exit-left.svg");
    case Maneuver::ExitRight:            return QStringLiteral("qrc:/marble/turn-exit-right.svg");
    case Maneuver::Unknown:
    default:
        return QString();
    }
}

qreal Navigation::nextInstructionDistance() const
{
    return d->m_nextInstructionDistance;
}

qreal Navigation::destinationDistance() const
{
    return d->m_destinationDistance;
}

QString Navigation::voiceNavigationAnnouncement() const
{
    return d->m_voiceNavigation.instruction();
}

QString Navigation::speaker() const
{
    return d->m_voiceNavigation.speaker();
}

void Navigation::setSpeaker( const QString &speaker )
{
    d->m_voiceNavigation.setSpeaker( speaker );
}

bool Navigation::deviated() const
{
    if ( d->m_marbleQuickItem ) {
        RoutingModel const * routingModel = d->model()->routingManager()->routingModel();
        return routingModel->deviatedFromRoute();
    }

    return true;
}

MarbleQuickItem *Navigation::marbleQuickItem() const
{
    return d->m_marbleQuickItem;
}

QPointF NavigationPrivate::positionOnRoute() const
{
    RoutingModel const * routingModel = model()->routingManager()->routingModel();
    GeoDataCoordinates  coordinates = routingModel->route().positionOnRoute();
    qreal x = 0;
    qreal y = 0;
    m_marbleQuickItem->map()->viewport()->screenCoordinates(coordinates, x, y);
    return QPointF(x,y);
}

QPointF NavigationPrivate::currentPosition() const
{
    GeoDataCoordinates coordinates = model()->positionTracking()->currentLocation();
    qreal x = 0;
    qreal y = 0;
    m_marbleQuickItem->map()->viewport()->screenCoordinates(coordinates, x, y);
    return QPointF(x,y);
}

QPointF Navigation::screenPosition() const
{
    return d->m_screenPosition;
}

double Navigation::screenAccuracy() const
{
    return d->m_screenAccuracy;
}

void Navigation::setMarbleQuickItem(MarbleQuickItem *marbleQuickItem)
{
    if ( d->m_marbleQuickItem == marbleQuickItem) {
        return;
    }

    if (d->m_marbleQuickItem) {
        disconnect( d->model()->routingManager()->routingModel(),
                 SIGNAL(positionChanged()), this, SLOT(update()) );
        disconnect( d->m_autoNavigation, SIGNAL(zoomIn(FlyToMode)),
                 d->m_marbleQuickItem, SLOT(zoomIn()) );
        disconnect( d->m_autoNavigation, SIGNAL(zoomOut(FlyToMode)),
                 d->m_marbleQuickItem, SLOT(zoomOut()) );
        disconnect( d->m_autoNavigation, SIGNAL(centerOn(GeoDataCoordinates,bool)),
                 d->m_marbleQuickItem, SLOT(centerOn(GeoDataCoordinates)) );

        disconnect( d->m_marbleQuickItem, SIGNAL(visibleLatLonAltBoxChanged()),
                 d->m_autoNavigation, SLOT(inhibitAutoAdjustments()) );
    }

    d->m_marbleQuickItem = marbleQuickItem;
    if ( d->m_marbleQuickItem ) {
        d->model()->routingManager()->setShowGuidanceModeStartupWarning( false );
        connect( d->model()->routingManager()->routingModel(),
                SIGNAL(positionChanged()), this, SLOT(update()) );
        connect( d->model()->routingManager()->routingModel(),
                SIGNAL(deviatedFromRoute(bool)), this, SIGNAL(deviationChanged()) );

        delete d->m_autoNavigation;
        d->m_autoNavigation = new AutoNavigation( d->model(), d->m_marbleQuickItem->map()->viewport(), this );
        connect( d->m_autoNavigation, SIGNAL(zoomIn(FlyToMode)),
                 d->m_marbleQuickItem, SLOT(zoomIn()) );
        connect( d->m_autoNavigation, SIGNAL(zoomOut(FlyToMode)),
                 d->m_marbleQuickItem, SLOT(zoomOut()) );
        connect( d->m_autoNavigation, SIGNAL(centerOn(GeoDataCoordinates,bool)),
                 d->m_marbleQuickItem, SLOT(centerOn(GeoDataCoordinates)) );

        connect( d->m_marbleQuickItem, SIGNAL(visibleLatLonAltBoxChanged()),
                 d->m_autoNavigation, SLOT(inhibitAutoAdjustments()) );

        connect( d->m_marbleQuickItem, SIGNAL(visibleLatLonAltBoxChanged()),
                 this, SLOT(updateScreenPosition()) );
        connect( d->model()->positionTracking(), SIGNAL(gpsLocation(GeoDataCoordinates,qreal)),
                 this, SLOT(updateScreenPosition()) );
        connect( d->model()->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)),
                 this, SLOT(updateScreenPosition()) );

    }
    emit marbleQuickItemChanged(marbleQuickItem);
}

void Navigation::update()
{
    if (!d->model()) {
        return;
    }

    RoutingModel const * routingModel = d->model()->routingManager()->routingModel();
    d->updateNextInstructionDistance( routingModel->route() );
    emit nextInstructionDistanceChanged();
    emit destinationDistanceChanged();
    RouteSegment segment = routingModel->route().currentSegment();

    if ( !d->m_muted ) {
        d->m_voiceNavigation.update( routingModel->route(), d->m_nextInstructionDistance, d->m_destinationDistance, routingModel->deviatedFromRoute() );
    }
    if ( segment != d->m_currentSegment ) {
        d->m_currentSegment = segment;
        emit nextInstructionTextChanged();
        emit nextInstructionImageChanged();
        emit nextRoadChanged();
    }

    updateScreenPosition();
}

void Navigation::updateScreenPosition()
{
    if(d->m_marbleQuickItem) {
        double distanceMeter = d->model()->positionTracking()->accuracy().horizontal;
        d->m_screenAccuracy = distanceMeter * d->m_marbleQuickItem->map()->radius() / d->model()->planetRadius();
        emit screenAccuracyChanged();

        d->m_screenPosition = deviated() ? d->currentPosition() : d->positionOnRoute();
        emit screenPositionChanged();
    }
}

}

#include "moc_Navigation.cpp"
