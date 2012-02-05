//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Navigation.h"

#include "MarbleDeclarativeWidget.h"
#include "MarbleModel.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "PositionTracking.h"
#include "MarbleMath.h"
#include "AutoNavigation.h"
#include "routing/VoiceNavigationModel.h"

class NavigationPrivate
{
public:
    NavigationPrivate();

    MarbleWidget* m_marbleWidget;

    bool m_muted;

    Marble::RouteSegment m_currentSegment;

    Marble::AutoNavigation* m_autoNavigation;

    Marble::VoiceNavigationModel m_voiceNavigation;

    qreal m_nextInstructionDistance;

    qreal m_destinationDistance;

    Marble::RouteSegment nextRouteSegment();

    void updateNextInstructionDistance( const Marble::RoutingModel *model );
};

NavigationPrivate::NavigationPrivate() :
    m_marbleWidget( 0 ), m_muted( false ), m_autoNavigation( 0 ), m_nextInstructionDistance( 0.0 ),
    m_destinationDistance( 0.0 )
{
    // nothing to do
}

void NavigationPrivate::updateNextInstructionDistance( const Marble::RoutingModel *model )
{
    Marble::GeoDataCoordinates position = model->route().position();
    Marble::GeoDataCoordinates interpolated = model->route().positionOnRoute();
    Marble::GeoDataCoordinates onRoute = model->route().currentWaypoint();
    qreal distance = Marble::EARTH_RADIUS * ( distanceSphere( position, interpolated ) + distanceSphere( interpolated, onRoute ) );
    qreal remaining = 0.0;
    const Marble::RouteSegment &segment = model->route().currentSegment();
    for ( int i=0; i<segment.path().size(); ++i ) {
        if ( segment.path()[i] == onRoute ) {
            distance += segment.path().length( Marble::EARTH_RADIUS, i );
            break;
        }
    }

    bool upcoming = false;
    for ( int i=0; i<model->route().size(); ++i ) {
        const Marble::RouteSegment &segment = model->route().at( i );

        if ( upcoming ) {
            remaining += segment.path().length( Marble::EARTH_RADIUS );
        }

        if ( segment == model->route().currentSegment() ) {
            upcoming = true;
        }
    }

    m_nextInstructionDistance = distance;
    m_destinationDistance = distance + remaining;
}

Marble::RouteSegment NavigationPrivate::nextRouteSegment()
{
    if ( m_marbleWidget ) {
        // Not using m_currentSegment on purpose
        return m_marbleWidget->model()->routingManager()->routingModel()->route().currentSegment().nextRouteSegment();
    }

    return Marble::RouteSegment();
}

Navigation::Navigation( QObject* parent) :
    QObject( parent ), d( new NavigationPrivate )
{
    connect( &d->m_voiceNavigation, SIGNAL( instructionChanged() ), this, SIGNAL( voiceNavigationAnnouncementChanged() ) );
}

Navigation::~Navigation()
{
    delete d;
}

void Navigation::setMarbleWidget( MarbleWidget* widget )
{
    d->m_marbleWidget = widget;
    if ( d->m_marbleWidget ) {
        // Avoid the QWidget based warning
        d->m_marbleWidget->model()->routingManager()->setShowGuidanceModeStartupWarning( false );
        connect( d->m_marbleWidget->model()->routingManager()->routingModel(),
                SIGNAL( positionChanged() ), this, SLOT( update() ) );

        d->m_autoNavigation = new Marble::AutoNavigation( d->m_marbleWidget->model(), d->m_marbleWidget->viewport(), this );
        connect( d->m_autoNavigation, SIGNAL( zoomIn( FlyToMode ) ),
                 d->m_marbleWidget, SLOT( zoomIn() ) );
        connect( d->m_autoNavigation, SIGNAL( zoomOut( FlyToMode ) ),
                 d->m_marbleWidget, SLOT( zoomOut() ) );
        connect( d->m_autoNavigation, SIGNAL( centerOn( const GeoDataCoordinates &, bool ) ),
                 d->m_marbleWidget, SLOT( centerOn( const GeoDataCoordinates & ) ) );

        connect( d->m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged() ),
                 d->m_autoNavigation, SLOT( inhibitAutoAdjustments() ) );
        connect( d->m_marbleWidget->model()->positionTracking(), SIGNAL( statusChanged( PositionProviderStatus ) ),
                 &d->m_voiceNavigation, SLOT( handleTrackingStatusChange( PositionProviderStatus ) ) );

        d->m_marbleWidget->model()->routingManager()->setAutoNavigation( d->m_autoNavigation );
    }
}

bool Navigation::guidanceModeEnabled() const
{
    return d->m_marbleWidget ? d->m_marbleWidget->model()->routingManager()->guidanceModeEnabled() : false;
}

void Navigation::setGuidanceModeEnabled( bool enabled )
{
    if ( d->m_marbleWidget ) {
        d->m_marbleWidget->model()->routingManager()->setGuidanceModeEnabled( enabled );

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
    if ( d->m_marbleWidget ) {
        Marble::RoutingModel const * model = d->m_marbleWidget->model()->routingManager()->routingModel();
        return model->deviatedFromRoute();
    }

    return true;
}

void Navigation::update()
{
    Marble::RoutingModel const * model = d->m_marbleWidget->model()->routingManager()->routingModel();
    d->updateNextInstructionDistance( model );
    emit nextInstructionDistanceChanged();
    emit destinationDistanceChanged();
    Marble::RouteSegment segment = model->route().currentSegment();
    if ( !d->m_muted ) {
        d->m_voiceNavigation.update( model->route(), d->m_nextInstructionDistance, d->m_destinationDistance, model->deviatedFromRoute() );
    }
    if ( segment != d->m_currentSegment ) {
        d->m_currentSegment = segment;
        emit nextInstructionTextChanged();
        emit nextInstructionImageChanged();
        emit nextRoadChanged();
    }
}

#include "Navigation.moc"
