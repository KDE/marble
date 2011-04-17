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

namespace Marble
{

Maneuver::Maneuver() :
    m_direction( RoutingInstruction::Unknown ),
    m_hasWaypoint( false )
{
    // nothing to do
}

RoutingInstruction::TurnType Maneuver::direction() const
{
    return m_direction;
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
