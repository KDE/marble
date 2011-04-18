//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_ROUTE_H
#define MARBLE_DECLARATIVE_ROUTE_H

#include "RouteSegment.h"
#include "GeoDataLatLonBox.h"

namespace Marble
{

class MARBLE_EXPORT Route
{
public:
    Route();

    void addRouteSegment( const RouteSegment &segment );

    GeoDataLatLonBox bounds() const;

    qreal distance() const;

    const RouteSegment & at( int index ) const;

    int size() const;

    GeoDataLineString path() const;

    int travelTime() const;

    GeoDataLineString turnPoints() const;

    GeoDataLineString waypoints() const;

    qreal distanceTo( const GeoDataCoordinates &point ) const;

    const RouteSegment & closestSegmentTo( const GeoDataCoordinates &point, qreal &distance ) const;

private:
    GeoDataLatLonBox m_bounds;

    qreal m_distance;

    QVector<RouteSegment> m_segments;

    GeoDataLineString m_path;

    GeoDataLineString m_turnPoints;

    GeoDataLineString m_waypoints;

    int m_travelTime;
};

}

#endif
