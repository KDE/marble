// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGPOINT_H
#define MARBLE_ROUTINGPOINT_H

#include "marble_export.h"

#include <QtGlobal>

class QTextStream;

namespace Marble
{

/**
 * There are many Point classes, but this is mine.
 */
class MARBLE_EXPORT RoutingPoint
{
public:
    explicit RoutingPoint(qreal lon = 0.0, qreal lat = 0.0);

    /** Longitude of the point */
    qreal lon() const;

    /** Latitude of the point */
    qreal lat() const;

    /**
     * Calculates the bearing of the line defined by this point
     * and the given other point.
     * Code based on https://www.movable-type.co.uk/scripts/latlong.html
     */
    qreal bearing(const RoutingPoint &other) const;

    /**
     * Calculates the distance in meter between this point and the
     * given other point.
     * Code based on https://www.movable-type.co.uk/scripts/latlong.html
     */
    qreal distance(const RoutingPoint &other) const;

private:
    qreal m_lon;

    qreal m_lonRad;

    qreal m_lat;

    qreal m_latRad;
};

QTextStream &operator<<(QTextStream &stream, const RoutingPoint &i);

} // namespace Marble

#endif // MARBLE_ROUTINGPOINT_H
