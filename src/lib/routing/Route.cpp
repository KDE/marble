//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Route.h"

namespace Marble
{

Route::Route() :
    m_distance( 0.0 ),
    m_travelTime( 0 )
{
    // nothing to do
}

void Route::setFirstRouteSegment( const RouteSegment &routeSegment )
{
    m_firstSegment = routeSegment;

    m_bounds = GeoDataLatLonBox();
    m_distance = 0.0;
    m_travelTime = 0;

    RouteSegment segment = m_firstSegment;
    while ( segment.isValid() ) {
        m_bounds = m_bounds.united( segment.bounds() );
        m_distance += segment.distance();
        m_path << segment.path();
        m_turnPoints << segment.maneuver().position();
        if ( segment.maneuver().hasWaypoint() ) {
            m_waypoints << segment.maneuver().waypoint();
        }

        segment = segment.nextRouteSegment();
    }
}

GeoDataLatLonBox Route::bounds() const
{
    return m_bounds;
}

qreal Route::distance() const
{
    return m_distance;
}

RouteSegment Route::firstRouteSegment() const
{
    return m_firstSegment;
}

GeoDataLineString Route::path() const
{
    return m_path;
}

int Route::travelTime() const
{
    return m_travelTime;
}

GeoDataLineString Route::turnPoints() const
{
    return m_turnPoints;
}

GeoDataLineString Route::waypoints() const
{
    return m_waypoints;
}

qreal Route::distanceTo( const GeoDataCoordinates &point ) const
{
    qreal minDistance = -1.0;
    RouteSegment closestSegment;
    distanceTo( point, m_firstSegment, closestSegment, minDistance );
    return minDistance;
}

void Route::distanceTo( const GeoDataCoordinates &point, const RouteSegment &segment, RouteSegment &closestSegment, qreal &minDistance ) const
{
    if ( !segment.isValid() ) {
        return;
    }

    qreal const distance = segment.distanceTo( point );
    if ( minDistance < 0.0 || distance < minDistance ) {
        closestSegment = segment;
        minDistance = distance;
    }

    return distanceTo( point, segment.nextRouteSegment(), closestSegment, minDistance );
}

RouteSegment Route::closestSegmentTo( const GeoDataCoordinates &point, qreal &distance ) const
{
    distance = -1.0;
    RouteSegment closestSegment = m_firstSegment;
    distanceTo( point, m_firstSegment, closestSegment, distance );
    return closestSegment;
}

}
