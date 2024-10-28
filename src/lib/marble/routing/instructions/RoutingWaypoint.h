// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGWAYPOINT_H
#define MARBLE_ROUTINGWAYPOINT_H

#include "RoutingPoint.h"
#include "marble_export.h"

#include <QList>
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
    enum JunctionType {
        Roundabout,
        Other,
        None
    };

    /** Constructor */
    RoutingWaypoint();

    /** Convenience constructor to initialize members */
    RoutingWaypoint(const RoutingPoint &point,
                    JunctionType junctionType,
                    const QString &junctionTypeRaw,
                    const QString &roadType,
                    int secondsRemaining,
                    const QString &roadName);

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

using RoutingWaypoints = QList<RoutingWaypoint>;

} // namespace Marble

#endif
