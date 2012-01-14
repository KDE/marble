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

namespace Marble
{
namespace Declarative
{

class NavigationPrivate
{
public:
    NavigationPrivate();

    Marble::Declarative::MarbleWidget* m_marbleWidget;

    bool m_muted;

    RouteSegment m_currentSegment;

    RouteSegment nextRouteSegment();
};

NavigationPrivate::NavigationPrivate() :
    m_marbleWidget( 0 ), m_muted( false )
{
    // nothing to do
}

RouteSegment NavigationPrivate::nextRouteSegment()
{
    if ( m_marbleWidget ) {
        // Not using m_currentSegment on purpose
        return m_marbleWidget->model()->routingManager()->routingModel()->route().currentSegment().nextRouteSegment();
    }

    return RouteSegment();
}

Navigation::Navigation( QObject* parent) :
    QObject( parent ), d( new NavigationPrivate )
{
    // nothing to do
}

Navigation::~Navigation()
{
    delete d;
}

void Navigation::setMarbleWidget( Marble::Declarative::MarbleWidget* widget )
{
    d->m_marbleWidget = widget;
    if ( d->m_marbleWidget ) {
        // Avoid the QWidget based warning
        d->m_marbleWidget->model()->routingManager()->setShowGuidanceModeStartupWarning( false );
        connect( d->m_marbleWidget->model()->routingManager()->routingModel(),
                SIGNAL( positionChanged() ), this, SLOT( update() ) );
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
    case Maneuver::Unknown: return "";
    case Maneuver::Straight: return "qrc:/marble/turn-continue.svg";
    case Maneuver::SlightRight: return "qrc:/marble/turn-slight-right.svg";
    case Maneuver::Right: return "qrc:/marble/turn-right.svg";
    case Maneuver::SharpRight: return "qrc:/marble/turn-sharp-right.svg";
    case Maneuver::TurnAround: return "qrc:/marble/turn-around.svg";
    case Maneuver::SharpLeft: return "qrc:/marble/turn-sharp-left.svg";
    case Maneuver::Left: return "qrc:/marble/turn-left.svg";
    case Maneuver::SlightLeft: return "qrc:/marble/turn-slight-left.svg";
    case Maneuver::RoundaboutFirstExit: return "qrc:/marble/turn-roundabout-first.svg";
    case Maneuver::RoundaboutSecondExit: return "qrc:/marble/turn-roundabout-second.svg";
    case Maneuver::RoundaboutThirdExit: return "qrc:/marble/turn-roundabout-third.svg";
    case Maneuver::RoundaboutExit: return "qrc:/marble/turn-roundabout-far.svg";
    }

    return "";
}

qreal Navigation::nextInstructionDistance() const
{
    if ( !d->m_marbleWidget || !d->nextRouteSegment().isValid() ) {
        return 0.0;
    }

    GeoDataCoordinates const position = d->m_marbleWidget->model()->positionTracking()->currentLocation();
    GeoDataCoordinates closest, interpolated;
    return d->nextRouteSegment().distanceTo( position, closest, interpolated );
}

void Navigation::update()
{
    emit nextInstructionDistanceChanged();
    RoutingModel const * model = d->m_marbleWidget->model()->routingManager()->routingModel();
    RouteSegment segment = model->route().currentSegment();
    if ( segment != d->m_currentSegment ) {
        d->m_currentSegment = segment;
        emit nextInstructionTextChanged();
        emit nextInstructionImageChanged();
        emit nextRoadChanged();
    }
}

}
}

#include "Navigation.moc"
