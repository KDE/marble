//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "Maneuver.h"
#include "MarbleDirs.h"

#include <QHash>

namespace Marble
{

static QHash<Maneuver::Direction, QString> createTurnTypePixmapMap()
{
    QHash<Maneuver::Direction, QString> turnTypePixmaps;

    turnTypePixmaps.insert(Maneuver::Unknown,              QStringLiteral(":/data/bitmaps/routing_step.png"));
    turnTypePixmaps.insert(Maneuver::Straight,             QStringLiteral(":/data/bitmaps/turn-continue.png"));
    turnTypePixmaps.insert(Maneuver::Continue,             QStringLiteral(":/data/bitmaps/turn-continue.png"));
    turnTypePixmaps.insert(Maneuver::Merge,                QStringLiteral(":/data/bitmaps/turn-merge.png"));
    turnTypePixmaps.insert(Maneuver::SlightRight,          QStringLiteral(":/data/bitmaps/turn-slight-right.png"));
    turnTypePixmaps.insert(Maneuver::Right,                QStringLiteral(":/data/bitmaps/turn-right.png"));
    turnTypePixmaps.insert(Maneuver::SharpRight,           QStringLiteral(":/data/bitmaps/turn-sharp-right.png"));
    turnTypePixmaps.insert(Maneuver::TurnAround,           QStringLiteral(":/data/bitmaps/turn-around.png"));
    turnTypePixmaps.insert(Maneuver::SharpLeft,            QStringLiteral(":/data/bitmaps/turn-sharp-left.png"));
    turnTypePixmaps.insert(Maneuver::Left,                 QStringLiteral(":/data/bitmaps/turn-left.png"));
    turnTypePixmaps.insert(Maneuver::SlightLeft,           QStringLiteral(":/data/bitmaps/turn-slight-left.png"));
    turnTypePixmaps.insert(Maneuver::RoundaboutFirstExit,  QStringLiteral(":/data/bitmaps/turn-roundabout-ccw-first.png"));
    turnTypePixmaps.insert(Maneuver::RoundaboutSecondExit, QStringLiteral(":/data/bitmaps/turn-roundabout-ccw-second.png"));
    turnTypePixmaps.insert(Maneuver::RoundaboutThirdExit,  QStringLiteral(":/data/bitmaps/turn-roundabout-ccw-third.png"));
    turnTypePixmaps.insert(Maneuver::RoundaboutExit,       QStringLiteral(":/data/bitmaps/turn-roundabout-ccw-far.png"));
    turnTypePixmaps.insert(Maneuver::ExitLeft,             QStringLiteral(":/data/bitmaps/turn-exit-left.png"));
    turnTypePixmaps.insert(Maneuver::ExitRight,            QStringLiteral(":/data/bitmaps/turn-exit-right.png"));

    return turnTypePixmaps;
}


Maneuver::Maneuver() :
    m_direction( Unknown ),
    m_waypointIndex( -1 )
{
}

Maneuver::Direction Maneuver::direction() const
{
    return m_direction;
}

QString Maneuver::directionPixmap() const
{
    static const QHash<Maneuver::Direction, QString> turnTypePixmaps = createTurnTypePixmapMap();

    return turnTypePixmaps.value(direction());
}

bool Maneuver::operator ==(const Maneuver &other) const
{
    return  m_direction == other.m_direction &&
            m_waypointIndex == other.m_waypointIndex &&
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
    return m_waypointIndex >= 0;
}

void Maneuver::setWaypoint( const GeoDataCoordinates &waypoint, int index )
{
    m_waypoint = waypoint;
    m_waypointIndex = index;
}

int Maneuver::waypointIndex() const
{
    return m_waypointIndex;
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
