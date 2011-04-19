//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_MANEUVER_H
#define MARBLE_DECLARATIVE_MANEUVER_H

#include "GeoDataCoordinates.h"
#include "routing/instructions/RoutingInstruction.h"

namespace Marble
{

class MARBLE_EXPORT Maneuver
{

public:
    Maneuver();

    RoutingInstruction::TurnType direction() const;

    void setDirection( RoutingInstruction::TurnType direction );

    GeoDataCoordinates position() const;

    void setPosition( const GeoDataCoordinates &position );

    GeoDataCoordinates waypoint() const;

    bool hasWaypoint() const;

    void setWaypoint( const GeoDataCoordinates &waypoint );

    QString instructionText() const;

    void setInstructionText( const QString &text );

    QString directionPixmap() const;

private:
    RoutingInstruction::TurnType m_direction;

    GeoDataCoordinates m_position;

    GeoDataCoordinates m_waypoint;

    bool m_hasWaypoint;

    QString m_instructionText;

    static QMap<RoutingInstruction::TurnType,QString> m_turnTypePixmaps;
};

}

#endif
