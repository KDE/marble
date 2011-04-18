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
    m_travelTime( 0 ),
    m_closestSegmentIndex( -1 )
{
    // nothing to do
}

void Route::addRouteSegment( const RouteSegment &segment )
{
    if ( segment.isValid() ) {
        m_bounds = m_bounds.united( segment.bounds() );
        m_distance += segment.distance();
        m_path << segment.path();
        m_turnPoints << segment.maneuver().position();
        if ( segment.maneuver().hasWaypoint() ) {
            m_waypoints << segment.maneuver().waypoint();
        }
        m_segments.push_back( segment );

        for ( int i=1; i<m_segments.size(); ++i ) {
            m_segments[i-1].setNextRouteSegment(&m_segments[i]);
        }
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

int Route::size() const
{
    return m_segments.size();
}

const RouteSegment & Route::at( int index ) const
{
    return m_segments[index];
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
    foreach( const RouteSegment &segment, m_segments ) {
        qreal const distance = segment.distanceTo( point );
        if ( minDistance < 0.0 || distance < minDistance ) {
            minDistance = distance;
        }
    }
    return minDistance;
}

const RouteSegment & Route::closestSegmentTo( const GeoDataCoordinates &point, qreal &distance ) const
{
    if ( !m_segments.isEmpty() ) {
        if ( m_closestSegmentIndex < 0 || m_closestSegmentIndex >= m_segments.size() ) {
            m_closestSegmentIndex = 0;
        }

        distance = m_segments[m_closestSegmentIndex].distanceTo( point );
        QList<int> candidates;

        for ( int i=0; i<m_segments.size(); ++i ) {
            if ( i != m_closestSegmentIndex && m_segments[i].minimalDistanceTo( point ) <= distance ) {
                candidates << i;
            }
        }

        foreach( int i, candidates ) {
            qreal const dist = m_segments[i].distanceTo( point );
            if ( distance < 0.0 || dist < distance ) {
                distance = dist;
                m_closestSegmentIndex = i;
            }
        }

        return m_segments[m_closestSegmentIndex];
    }

    static RouteSegment invalid;
    return invalid;
}

}
