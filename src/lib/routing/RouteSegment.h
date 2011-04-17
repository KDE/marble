//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_ROUTESEGMENT_H
#define MARBLE_DECLARATIVE_ROUTESEGMENT_H

#include "Maneuver.h"
#include "GeoDataLineString.h"
#include "GeoDataLatLonBox.h"

#include <QtCore/Qt>

namespace Marble
{

class RouteSegment
{
public:
    RouteSegment();

    RouteSegment( const RouteSegment &other );

    RouteSegment& operator=(const RouteSegment &other );

    ~RouteSegment();

    bool isValid() const;

    qreal distance() const;

    Maneuver maneuver() const;

    void setManeuver( const Maneuver &maneuver );

    GeoDataLineString path() const;

    void setPath( const GeoDataLineString &path );

    int travelTime() const;

    void setTravelTime( int seconds );

    GeoDataLatLonBox bounds() const;

    RouteSegment nextRouteSegment() const;

    void setNextRouteSegment( const RouteSegment &segment );

    qreal distanceTo( const GeoDataCoordinates &point ) const;

private:
    qreal distancePointToLine(const GeoDataCoordinates &p, const GeoDataCoordinates &a, const GeoDataCoordinates &b) const;

    bool m_valid;

    qreal m_distance;

    Maneuver m_maneuver;

    GeoDataLineString m_path;

    int m_travelTime;

    GeoDataLatLonBox m_bounds;

    RouteSegment *m_nextRouteSegment;
};


}

#endif
