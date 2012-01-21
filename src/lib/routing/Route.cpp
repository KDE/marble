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
    m_positionDirty( true ),
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
        if ( segment.maneuver().position().longitude() != 0.0 && segment.maneuver().position().latitude() != 0.0 ) {
            m_turnPoints << segment.maneuver().position();
        }
        if ( segment.maneuver().hasWaypoint() ) {
            m_waypoints << segment.maneuver().waypoint();
        }
        m_segments.push_back( segment );
        m_positionDirty = true;

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

const GeoDataLineString & Route::path() const
{
    return m_path;
}

int Route::travelTime() const
{
    return m_travelTime;
}

const GeoDataLineString & Route::turnPoints() const
{
    return m_turnPoints;
}

const GeoDataLineString & Route::waypoints() const
{
    return m_waypoints;
}

void Route::setPosition( const GeoDataCoordinates &position )
{
    m_position = position;
    m_positionDirty = true;
}

GeoDataCoordinates Route::position() const
{
    return m_position;
}

void Route::updatePosition() const
{
    if ( !m_segments.isEmpty() ) {
        if ( m_closestSegmentIndex < 0 || m_closestSegmentIndex >= m_segments.size() ) {
            m_closestSegmentIndex = 0;
        }

        qreal distance = m_segments[m_closestSegmentIndex].distanceTo( m_position, m_currentWaypoint, m_positionOnRoute );
        QList<int> candidates;

        for ( int i=0; i<m_segments.size(); ++i ) {
            if ( i != m_closestSegmentIndex && m_segments[i].minimalDistanceTo( m_position ) <= distance ) {
                candidates << i;
            }
        }

        GeoDataCoordinates closest, interpolated;
        foreach( int i, candidates ) {
            qreal const dist = m_segments[i].distanceTo( m_position, closest, interpolated );
            if ( distance < 0.0 || dist < distance ) {
                distance = dist;
                m_closestSegmentIndex = i;
                m_positionOnRoute = interpolated;
                m_currentWaypoint = closest;
            }
        }
    }

    m_positionDirty = false;
}

const RouteSegment & Route::currentSegment() const
{
    if ( m_positionDirty ) {
        updatePosition();
    }

    if ( m_closestSegmentIndex < 0 || m_closestSegmentIndex >= m_segments.size() ) {
        static RouteSegment invalid;
        return invalid;
    }

    return m_segments[m_closestSegmentIndex];
}

GeoDataCoordinates Route::positionOnRoute() const
{
    if ( m_positionDirty ) {
        updatePosition();
    }

    return m_positionOnRoute;
}

GeoDataCoordinates Route::currentWaypoint() const
{
    if ( m_positionDirty ) {
        updatePosition();
    }

    return m_currentWaypoint;
}

}
