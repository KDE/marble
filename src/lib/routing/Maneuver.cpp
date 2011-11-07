//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Maneuver.h"
#include "MarbleDirs.h"

namespace Marble
{

QMap<RoutingInstruction::TurnType,QString> Maneuver::m_turnTypePixmaps;

Maneuver::Maneuver() :
    m_direction( RoutingInstruction::Unknown ),
    m_hasWaypoint( false )
{
    m_turnTypePixmaps[RoutingInstruction::Unknown] = ":/data/bitmaps/routing_step.png";
    m_turnTypePixmaps[RoutingInstruction::Straight] = ":/data/bitmaps/turn-continue.png";
    m_turnTypePixmaps[RoutingInstruction::SlightRight] = ":/data/bitmaps/turn-slight-right.png";
    m_turnTypePixmaps[RoutingInstruction::Right] = ":/data/bitmaps/turn-right.png";
    m_turnTypePixmaps[RoutingInstruction::SharpRight] = ":/data/bitmaps/turn-sharp-right.png";
    m_turnTypePixmaps[RoutingInstruction::TurnAround] = ":/data/bitmaps/turn-around.png";
    m_turnTypePixmaps[RoutingInstruction::SharpLeft] = ":/data/bitmaps/turn-sharp-left.png";
    m_turnTypePixmaps[RoutingInstruction::Left] = ":/data/bitmaps/turn-left.png";
    m_turnTypePixmaps[RoutingInstruction::SlightLeft] = ":/data/bitmaps/turn-slight-left.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutFirstExit] = ":/data/bitmaps/turn-roundabout-first.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutSecondExit] = ":/data/bitmaps/turn-roundabout-second.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutThirdExit] = ":/data/bitmaps/turn-roundabout-third.png";
    m_turnTypePixmaps[RoutingInstruction::RoundaboutExit] = ":/data/bitmaps/turn-roundabout-far.png";
}

RoutingInstruction::TurnType Maneuver::direction() const
{
    return m_direction;
}

QString Maneuver::directionPixmap() const
{
    return m_turnTypePixmaps[ direction() ];
}

void Maneuver::setDirection( RoutingInstruction::TurnType direction )
{
    m_direction = direction;
}

GeoDataCoordinates Maneuver::position() const
{
    return m_position;
}

void Maneuver::setPosition( const GeoDataCoordinates &position )
{
    m_position = position;
}

GeoDataCoordinates Maneuver::waypoint() const
{
    return m_waypoint;
}

bool Maneuver::hasWaypoint() const
{
    return m_hasWaypoint;
}

void Maneuver::setWaypoint( const GeoDataCoordinates &waypoint )
{
    m_waypoint = waypoint;
    m_hasWaypoint = true;
}

QString Maneuver::instructionText() const
{
    return m_instructionText;
}

void Maneuver::setInstructionText( const QString &text )
{
    m_instructionText = text;
}

}
