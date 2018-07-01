//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGWAYPOINT_H
#define MARBLE_ROUTINGWAYPOINT_H

#include "RoutingPoint.h"
#include "marble_export.h"

#include <QVector>
#include <QString>

namespace Marble
{

/**
  * Stores one line of gosmore/routino output
  */
class MARBLE_EXPORT RoutingWaypoint
{
public:
    /** Junction types that affect instructions */
    enum JunctionType
    {
      Roundabout,
      Other,
      None
    };

    /** Constructor */
    RoutingWaypoint();

    /** Convenience constructor to initialize members */
    RoutingWaypoint( const RoutingPoint &point, JunctionType junctionType, const QString &junctionTypeRaw,
                     const QString &roadType, int secondsRemaining, const QString &roadName );

    /** Associated geo point */
    RoutingPoint point() const;

    /** Parsed junction type */
    JunctionType junctionType() const;

    /** Junction type originally passed */
    QString junctionTypeRaw() const;

    /** OSM type of the road */
    QString roadType() const;

    /** Estimated seconds remaining until the route destination is reached */
    int secondsRemaining() const;

    /** OSM name of the road */
    QString roadName() const;

private:
    RoutingPoint m_point;

    JunctionType m_junctionType;

    QString m_junctionTypeRaw;

    QString m_roadType;

    int m_secondsRemaining;

    QString m_roadName;
};

using RoutingWaypoints = QVector<RoutingWaypoint>;

} // namespace Marble

#endif
