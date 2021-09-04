// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTESEGMENT_H
#define MARBLE_ROUTESEGMENT_H

#include "Maneuver.h"
#include "GeoDataLineString.h"
#include "GeoDataLatLonBox.h"

#include <QtGlobal>

namespace Marble
{

class MARBLE_EXPORT RouteSegment
{
public:
    RouteSegment();

    bool isValid() const;

    qreal distance() const;

    const Maneuver & maneuver() const;

    void setManeuver( const Maneuver &maneuver );

    const GeoDataLineString & path() const;

    void setPath( const GeoDataLineString &path );

    int travelTime() const;

    void setTravelTime( int seconds );

    GeoDataLatLonBox bounds() const;

    const RouteSegment & nextRouteSegment() const;

    void setNextRouteSegment( const RouteSegment* segment );

    qreal distanceTo( const GeoDataCoordinates &point, GeoDataCoordinates &closest, GeoDataCoordinates &interpolated ) const;

    qreal minimalDistanceTo( const GeoDataCoordinates &point ) const;

    qreal projectedDirection(const GeoDataCoordinates &point) const;

    bool operator==( const RouteSegment &other ) const;

    bool operator!=( const RouteSegment &other ) const;

private:
    static qreal distancePointToLine(const GeoDataCoordinates &p, const GeoDataCoordinates &a, const GeoDataCoordinates &b);

    static GeoDataCoordinates projected(const GeoDataCoordinates &p, const GeoDataCoordinates &a, const GeoDataCoordinates &b);

    bool m_valid;

    qreal m_distance;

    Maneuver m_maneuver;

    GeoDataLineString m_path;

    int m_travelTime;

    GeoDataLatLonBox m_bounds;

    const RouteSegment *m_nextRouteSegment;
};


}

#endif
