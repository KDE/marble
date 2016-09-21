//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RouteSegment.h"

#include "MarbleMath.h"
#include "GeoDataLatLonAltBox.h"

namespace Marble
{

RouteSegment::RouteSegment() :
    m_valid( false ),
    m_distance( 0.0 ),
    m_travelTime( 0 ),
    m_nextRouteSegment( 0 )
{
    // nothing to do
}

qreal RouteSegment::distance() const
{
    return m_distance;
}

const Maneuver & RouteSegment::maneuver() const
{
    return m_maneuver;
}

void RouteSegment::setManeuver( const Maneuver &maneuver )
{
    m_maneuver = maneuver;
    m_valid = true;
}

const GeoDataLineString & RouteSegment::path() const
{
    return m_path;
}

void RouteSegment::setPath( const GeoDataLineString &path )
{
    m_path = path;
    m_distance = m_path.length( EARTH_RADIUS );
    m_bounds = m_path.latLonAltBox();
    m_valid = true;
}

int RouteSegment::travelTime() const
{
    return m_travelTime;
}

void RouteSegment::setTravelTime( int seconds )
{
    m_travelTime = seconds;
    m_valid = true;
}

GeoDataLatLonBox RouteSegment::bounds() const
{
    return m_bounds;
}

const RouteSegment & RouteSegment::nextRouteSegment() const
{
    if ( m_nextRouteSegment ) {
        return *m_nextRouteSegment;
    }

    static RouteSegment invalid;
    return invalid;
}

void RouteSegment::setNextRouteSegment( const RouteSegment* segment )
{
    m_nextRouteSegment = segment;
    if ( segment ) {
        m_valid = true;
    }
}

bool RouteSegment::isValid() const
{
    return m_valid;
}

qreal RouteSegment::distancePointToLine(const GeoDataCoordinates &p, const GeoDataCoordinates &a, const GeoDataCoordinates &b)
{
    qreal const y0 = p.latitude();
    qreal const x0 = p.longitude();
    qreal const y1 = a.latitude();
    qreal const x1 = a.longitude();
    qreal const y2 = b.latitude();
    qreal const x2 = b.longitude();
    qreal const y01 = x0 - x1;
    qreal const x01 = y0 - y1;
    qreal const y10 = x1 - x0;
    qreal const x10 = y1 - y0;
    qreal const y21 = x2 - x1;
    qreal const x21 = y2 - y1;
    qreal const len =(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
    qreal const t = (x01*x21 + y01*y21) / len;
    if ( t<0.0 ) {
        return EARTH_RADIUS * distanceSphere(p, a);
    } else if ( t > 1.0 ) {
        return EARTH_RADIUS * distanceSphere(p, b);
    } else {
        qreal const nom = qAbs( x21 * y10 - x10 * y21 );
        qreal const den = sqrt( x21 * x21 + y21 * y21 );
        return EARTH_RADIUS * nom / den;
    }
}

GeoDataCoordinates RouteSegment::projected(const GeoDataCoordinates &p, const GeoDataCoordinates &a, const GeoDataCoordinates &b)
{
    qreal const y0 = p.latitude();
    qreal const x0 = p.longitude();
    qreal const y1 = a.latitude();
    qreal const x1 = a.longitude();
    qreal const y2 = b.latitude();
    qreal const x2 = b.longitude();
    qreal const y01 = x0 - x1;
    qreal const x01 = y0 - y1;
    qreal const y21 = x2 - x1;
    qreal const x21 = y2 - y1;
    qreal const len =(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
    qreal const t = (x01*x21 + y01*y21) / len;
    if ( t<0.0 ) {
        return a;
    } else if ( t > 1.0 ) {
        return b;
    } else {
        // a + t (b - a);
        qreal const lon = x1 + t * ( x2 - x1 );
        qreal const lat = y1 + t * ( y2 - y1 );
        return GeoDataCoordinates( lon, lat );
    }

}

qreal RouteSegment::distanceTo( const GeoDataCoordinates &point, GeoDataCoordinates &closest, GeoDataCoordinates &interpolated ) const
{
    Q_ASSERT( !m_path.isEmpty() );

    if ( m_path.size() == 1 ) {
        closest = m_path.first();
        return EARTH_RADIUS * distanceSphere( m_path.first(), point );
    }

    qreal minDistance = -1.0;
    int minIndex = 0;
    for ( int i=1; i<m_path.size(); ++i ) {
        qreal const distance = distancePointToLine( point, m_path[i-1], m_path[i] );
        if ( minDistance < 0.0 || distance < minDistance ) {
            minDistance = distance;
            minIndex = i;
        }
    }

    closest = m_path[minIndex];
    if ( minIndex == 0 ) {
        interpolated = closest;
    } else {
        interpolated = projected( point, m_path[minIndex-1], m_path[minIndex] );
    }

    return minDistance;
}

qreal RouteSegment::minimalDistanceTo( const GeoDataCoordinates &point ) const
{
    if ( bounds().contains( point) ) {
        return 0.0;
    }

    qreal north(0.0), east(0.0), south(0.0), west(0.0);
    bounds().boundaries( north, south, east, west );
    GeoDataCoordinates const northWest( west, north );
    GeoDataCoordinates const northEast( east, north );
    GeoDataCoordinates const southhWest( west, south );
    GeoDataCoordinates const southEast( east, south );

    qreal distNorth = distancePointToLine( point, northWest, northEast );
    qreal distEast = distancePointToLine( point, northEast, southEast );
    qreal distSouth = distancePointToLine( point, southhWest, southEast );
    qreal distWest = distancePointToLine( point, northWest, southhWest );
    return qMin( qMin( distNorth, distEast ), qMin( distWest, distSouth ) );
}

qreal RouteSegment::projectedDirection(const GeoDataCoordinates &point) const
{
    if (m_path.size() < 2){
        return 0;
    }

    qreal minDistance = -1.0;
    int minIndex = 0;
    for ( int i=1; i<m_path.size(); ++i ) {
        qreal const distance = distancePointToLine( point, m_path[i-1], m_path[i] );
        if ( minDistance < 0.0 || distance < minDistance ) {
            minDistance = distance;
            minIndex = i;
        }
    }

    if ( minIndex == 0 ) {
        return m_path[0].bearing( m_path[1], GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
    } else {
        return m_path[minIndex-1].bearing( m_path[minIndex], GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
    }
}

bool RouteSegment::operator ==(const RouteSegment &other) const
{
    return  m_valid == other.m_valid &&
            m_distance == other.m_distance &&
            m_maneuver == other.m_maneuver &&
            m_travelTime == other.m_travelTime &&
            m_bounds == other.m_bounds &&
            m_nextRouteSegment == other.m_nextRouteSegment;
}

bool RouteSegment::operator !=(const RouteSegment &other) const
{
    return !(other == *this);
}

}
