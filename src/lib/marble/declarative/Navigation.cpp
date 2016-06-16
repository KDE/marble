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
#include "MarbleDeclarativeWidget.h"
#include "MarbleModel.h"
#include "MarbleQuickItem.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "PositionTracking.h"
#include "MarbleMath.h"
#include "AutoNavigation.h"
#include "routing/VoiceNavigationModel.h"
#include "ViewportParams.h"

class NavigationPrivate
{
public:
    NavigationPrivate();

    MarbleWidget* m_marbleWidget;

    Marble::MarbleQuickItem * m_marbleQuickItem;

    bool m_muted;

    Marble::RouteSegment m_currentSegment;

    Marble::AutoNavigation* m_autoNavigation;

    Marble::VoiceNavigationModel m_voiceNavigation;

    qreal m_nextInstructionDistance;

    qreal m_destinationDistance;

    double m_screenAccuracy;

    Marble::RouteSegment nextRouteSegment();

    void updateNextInstructionDistance( const Marble::Route &route );

    Marble::MarbleModel * model() const;
};

NavigationPrivate::NavigationPrivate() :
    m_marbleWidget( nullptr ), m_marbleQuickItem( nullptr ), m_muted( false ), m_autoNavigation( 0 ), m_nextInstructionDistance( 0.0 ),
    m_destinationDistance( 0.0 ), m_screenAccuracy(0)
{
    // nothing to do
}

void NavigationPrivate::updateNextInstructionDistance( const Marble::Route &route )
{
    const Marble::GeoDataCoordinates position = route.position();
    const Marble::GeoDataCoordinates interpolated = route.positionOnRoute();
    const Marble::GeoDataCoordinates onRoute = route.currentWaypoint();

    qreal planetRadius = 0;
    if (model()){
        planetRadius = model()->planet()->radius();
    }
    qreal distance = planetRadius * ( distanceSphere( position, interpolated ) + distanceSphere( interpolated, onRoute ) );
    qreal remaining = 0.0;
    const Marble::RouteSegment &segment = route.currentSegment();
    for ( int i=0; i<segment.path().size(); ++i ) {
        if ( segment.path()[i] == onRoute ) {
            distance += segment.path().length( planetRadius, i );
            break;
        }
    }

    bool upcoming = false;
    for ( int i=0; i<route.size(); ++i ) {
        const Marble::RouteSegment &segment = route.at( i );

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

Marble::MarbleModel * NavigationPrivate::model() const
{
    if (m_marbleQuickItem !=  nullptr) {
        return m_marbleQuickItem->model();
    }
    else if (m_marbleWidget != nullptr)
    {
        return m_marbleWidget->model();
    }
    return nullptr;
}

Marble::RouteSegment NavigationPrivate::nextRouteSegment()
{
    if ( m_marbleWidget || m_marbleQuickItem) {
        // Not using m_currentSegment on purpose
        return model()->routingManager()->routingModel()->route().currentSegment().nextRouteSegment();
    }

    return Marble::RouteSegment();
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

MarbleWidget *Navigation::map()
{
    return d->m_marbleWidget;
}

void Navigation::setMap( MarbleWidget* widget )
{
    d->m_marbleWidget = widget;
    if ( d->m_marbleWidget ) {
        // Avoid the QWidget based warning
        d->m_marbleWidget->model()->routingManager()->setShowGuidanceModeStartupWarning( false );
        connect( d->m_marbleWidget->model()->routingManager()->routingModel(),
                SIGNAL(positionChanged()), this, SLOT(update()) );

        delete d->m_autoNavigation;
        d->m_autoNavigation = new Marble::AutoNavigation( d->m_marbleWidget->model(), d->m_marbleWidget->viewport(), this );
        connect( d->m_autoNavigation, SIGNAL(zoomIn(FlyToMode)),
                 d->m_marbleWidget, SLOT(zoomIn()) );
        connect( d->m_autoNavigation, SIGNAL(zoomOut(FlyToMode)),
                 d->m_marbleWidget, SLOT(zoomOut()) );
        connect( d->m_autoNavigation, SIGNAL(centerOn(GeoDataCoordinates,bool)),
                 d->m_marbleWidget, SLOT(centerOn(GeoDataCoordinates)) );

        connect( d->m_marbleWidget, SIGNAL(visibleLatLonAltBoxChanged()),
                 d->m_autoNavigation, SLOT(inhibitAutoAdjustments()) );
        connect( d->m_marbleWidget->model()->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)),
                 &d->m_voiceNavigation, SLOT(handleTrackingStatusChange(PositionProviderStatus)) );
    }
    emit mapChanged();
}

bool Navigation::guidanceModeEnabled() const
{
    if (d->m_marbleWidget || d->m_marbleQuickItem) {
        return d->model()->routingManager()->guidanceModeEnabled();
    }
    return false;
}

void Navigation::setGuidanceModeEnabled( bool enabled )
{
    if ( d->m_marbleWidget || d->m_marbleQuickItem ) {
        d->model()->routingManager()->setGuidanceModeEnabled( enabled );
        d->m_autoNavigation->setAutoZoom( enabled );
        d->m_autoNavigation->setRecenter( enabled ? Marble::AutoNavigation::RecenterOnBorder : Marble::AutoNavigation::DontRecenter );

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
    case Marble::Maneuver::Unknown: return "";
    case Marble::Maneuver::Continue: return "qrc:/marble/turn-continue.svg";
    case Marble::Maneuver::Merge: return "qrc:/marble/turn-merge.svg";
    case Marble::Maneuver::Straight: return "qrc:/marble/turn-continue.svg";
    case Marble::Maneuver::SlightRight: return "qrc:/marble/turn-slight-right.svg";
    case Marble::Maneuver::Right: return "qrc:/marble/turn-right.svg";
    case Marble::Maneuver::SharpRight: return "qrc:/marble/turn-sharp-right.svg";
    case Marble::Maneuver::TurnAround: return "qrc:/marble/turn-around.svg";
    case Marble::Maneuver::SharpLeft: return "qrc:/marble/turn-sharp-left.svg";
    case Marble::Maneuver::Left: return "qrc:/marble/turn-left.svg";
    case Marble::Maneuver::SlightLeft: return "qrc:/marble/turn-slight-left.svg";
    case Marble::Maneuver::RoundaboutFirstExit: return "qrc:/marble/turn-roundabout-first.svg";
    case Marble::Maneuver::RoundaboutSecondExit: return "qrc:/marble/turn-roundabout-second.svg";
    case Marble::Maneuver::RoundaboutThirdExit: return "qrc:/marble/turn-roundabout-third.svg";
    case Marble::Maneuver::RoundaboutExit: return "qrc:/marble/turn-roundabout-far.svg";
    case Marble::Maneuver::ExitLeft: return "qrc:/marble/turn-exit-left.svg";
    case Marble::Maneuver::ExitRight: return "qrc:/marble/turn-exit-right.svg";
    }

    return "";
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

bool Navigation::soundEnabled() const
{
    return !d->m_voiceNavigation.isSpeakerEnabled();
}

void Navigation::setSoundEnabled( bool soundEnabled )
{
    d->m_voiceNavigation.setSpeakerEnabled( !soundEnabled );
}

bool Navigation::deviated() const
{
    if ( d->m_marbleWidget || d->m_marbleQuickItem ) {
        Marble::RoutingModel const * routingModel = d->model()->routingManager()->routingModel();
        return routingModel->deviatedFromRoute();
    }

    return true;
}

Marble::MarbleQuickItem *Navigation::marbleQuickItem() const
{
    return d->m_marbleQuickItem;
}

QPointF Navigation::positionOnRoute() const
{
    Marble::RoutingModel const * routingModel = d->model()->routingManager()->routingModel();
    GeoDataCoordinates  coordinates = routingModel->route().positionOnRoute();
    qreal x = 0;
    qreal y = 0;
    d->m_marbleQuickItem->map()->viewport()->screenCoordinates(coordinates, x, y);
    return QPointF(x,y);
}

QPointF Navigation::currentPosition() const
{
    GeoDataCoordinates coordinates = d->model()->positionTracking()->currentLocation();
    qreal x = 0;
    qreal y = 0;
    d->m_marbleQuickItem->map()->viewport()->screenCoordinates(coordinates, x, y);
    return QPointF(x,y);
}

double Navigation::screenAccuracy() const
{
    double distanceMeter = d->model()->positionTracking()->accuracy().horizontal;
    if(d->m_marbleQuickItem == 0){
        return 0;
    }
    return distanceMeter * d->m_marbleQuickItem->map()->radius() / d->model()->planetRadius();
}

void Navigation::setMarbleQuickItem(Marble::MarbleQuickItem *marbleQuickItem)
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
        disconnect( d->model()->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)),
                 &d->m_voiceNavigation, SLOT(handleTrackingStatusChange(PositionProviderStatus)) );
    }

    d->m_marbleQuickItem = marbleQuickItem;
    if ( d->m_marbleQuickItem ) {
        d->model()->routingManager()->setShowGuidanceModeStartupWarning( false );
        connect( d->model()->routingManager()->routingModel(),
                SIGNAL(positionChanged()), this, SLOT(update()) );
        connect( d->model()->routingManager()->routingModel(),
                SIGNAL(deviatedFromRoute(bool)), this, SIGNAL(deviationChanged()) );

        delete d->m_autoNavigation;
        d->m_autoNavigation = new Marble::AutoNavigation( d->model(), d->m_marbleQuickItem->map()->viewport(), this );
        connect( d->m_autoNavigation, SIGNAL(zoomIn(FlyToMode)),
                 d->m_marbleQuickItem, SLOT(zoomIn()) );
        connect( d->m_autoNavigation, SIGNAL(zoomOut(FlyToMode)),
                 d->m_marbleQuickItem, SLOT(zoomOut()) );
        connect( d->m_autoNavigation, SIGNAL(centerOn(GeoDataCoordinates,bool)),
                 d->m_marbleQuickItem, SLOT(centerOn(GeoDataCoordinates)) );

        connect( d->m_marbleQuickItem, SIGNAL(visibleLatLonAltBoxChanged()),
                 d->m_autoNavigation, SLOT(inhibitAutoAdjustments()) );
        connect( d->model()->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)),
                 &d->m_voiceNavigation, SLOT(handleTrackingStatusChange(PositionProviderStatus)) );

    }
    emit marbleQuickItemChanged(marbleQuickItem);
}

void Navigation::update()
{
    if (!d->model()) {
        return;
    }

    Marble::RoutingModel const * routingModel = d->model()->routingManager()->routingModel();
    d->updateNextInstructionDistance( routingModel->route() );
    emit nextInstructionDistanceChanged();
    emit destinationDistanceChanged();
    Marble::RouteSegment segment = routingModel->route().currentSegment();
    if ( !d->m_muted ) {
        d->m_voiceNavigation.update( routingModel->route(), d->m_nextInstructionDistance, d->m_destinationDistance, routingModel->deviatedFromRoute() );
    }
    if ( segment != d->m_currentSegment ) {
        d->m_currentSegment = segment;
        emit nextInstructionTextChanged();
        emit nextInstructionImageChanged();
        emit nextRoadChanged();
    }
}


#include "moc_Navigation.cpp"
