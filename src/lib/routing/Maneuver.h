//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_MANEUVER_H
#define MARBLE_MANEUVER_H

#include <QtCore/QString>
#include <QtCore/QMap>

#include "GeoDataCoordinates.h"

namespace Marble
{

class MARBLE_EXPORT Maneuver
{

public:
    enum Direction {
        Unknown = 0,
        Continue = 13,
        Merge = 14,
        Straight = 1,
        SlightRight = 2,
        Right = 3,
        SharpRight = 4,
        TurnAround = 5,
        SharpLeft = 6,
        Left = 7,
        SlightLeft = 8,
        RoundaboutFirstExit = 9,
        RoundaboutSecondExit = 10,
        RoundaboutThirdExit = 11,
        RoundaboutExit = 12,
        ExitLeft = 15,
        ExitRight = 16
    };

    Maneuver();

    Direction direction() const;

    void setDirection( Direction direction );

    GeoDataCoordinates position() const;

    void setPosition( const GeoDataCoordinates &position );

    GeoDataCoordinates waypoint() const;

    bool hasWaypoint() const;

    void setWaypoint( const GeoDataCoordinates &waypoint, int index );

    int waypointIndex() const;

    QString instructionText() const;

    void setInstructionText( const QString &text );

    QString roadName() const;

    void setRoadName( const QString &roadName );

    QString directionPixmap() const;

    bool operator==( const Maneuver &other ) const;

    bool operator!=( const Maneuver &other ) const;

private:
    Direction m_direction;

    GeoDataCoordinates m_position;

    GeoDataCoordinates m_waypoint;

    int m_waypointIndex;

    QString m_instructionText;

    QString m_roadName;

    static QMap<Direction,QString> m_turnTypePixmaps;
};

}

#endif
