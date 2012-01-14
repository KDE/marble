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

QMap<Maneuver::Direction,QString> Maneuver::m_turnTypePixmaps;

Maneuver::Maneuver() :
    m_direction( Unknown ),
    m_hasWaypoint( false )
{
    m_turnTypePixmaps[Unknown] = ":/data/bitmaps/routing_step.png";
    m_turnTypePixmaps[Straight] = ":/data/bitmaps/turn-continue.png";
    m_turnTypePixmaps[SlightRight] = ":/data/bitmaps/turn-slight-right.png";
    m_turnTypePixmaps[Right] = ":/data/bitmaps/turn-right.png";
    m_turnTypePixmaps[SharpRight] = ":/data/bitmaps/turn-sharp-right.png";
    m_turnTypePixmaps[TurnAround] = ":/data/bitmaps/turn-around.png";
    m_turnTypePixmaps[SharpLeft] = ":/data/bitmaps/turn-sharp-left.png";
    m_turnTypePixmaps[Left] = ":/data/bitmaps/turn-left.png";
    m_turnTypePixmaps[SlightLeft] = ":/data/bitmaps/turn-slight-left.png";
    m_turnTypePixmaps[RoundaboutFirstExit] = ":/data/bitmaps/turn-roundabout-first.png";
    m_turnTypePixmaps[RoundaboutSecondExit] = ":/data/bitmaps/turn-roundabout-second.png";
    m_turnTypePixmaps[RoundaboutThirdExit] = ":/data/bitmaps/turn-roundabout-third.png";
    m_turnTypePixmaps[RoundaboutExit] = ":/data/bitmaps/turn-roundabout-far.png";
}

Maneuver::Direction Maneuver::direction() const
{
    return m_direction;
}

QString Maneuver::directionPixmap() const
{
    return m_turnTypePixmaps[ direction() ];
}

bool Maneuver::operator ==(const Maneuver &other) const
{
    return  m_direction == other.m_direction &&
            m_hasWaypoint == other.m_hasWaypoint &&
            m_position == other.m_position &&
            m_waypoint == other.m_waypoint &&
            m_instructionText == other.m_instructionText;
}

bool Maneuver::operator !=(const Maneuver &other) const
{
    return !(other == *this);
}

void Maneuver::setDirection( Direction direction )
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

QString Maneuver::roadName() const
{
    return m_roadName;
}

void Maneuver::setRoadName(const QString &roadName)
{
    m_roadName = roadName;
}

}
