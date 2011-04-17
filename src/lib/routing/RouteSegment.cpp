//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RouteSegment.h"

#include "MarbleMath.h"

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

RouteSegment::RouteSegment( const RouteSegment &other ) :
    m_valid( other.m_valid ),
    m_distance( other.m_distance ),
    m_maneuver( other.m_maneuver ),
    m_path( other.m_path ),
    m_travelTime( other.m_travelTime ),
    m_bounds( other.m_bounds ),
    m_nextRouteSegment( other.m_nextRouteSegment ? new RouteSegment( *other.m_nextRouteSegment ) : 0 )
{
    // nothing to do
}

RouteSegment& RouteSegment::operator=(const RouteSegment &other )
{
    m_valid = other.m_valid;
    m_distance = other.m_distance;
    m_maneuver = other.m_maneuver;
    m_path = other.m_path;
    m_travelTime = other.m_travelTime;
    m_bounds = other.m_bounds;
    m_nextRouteSegment = other.m_nextRouteSegment ? new RouteSegment( *other.m_nextRouteSegment ) : 0;

    return *this;
}

RouteSegment::~RouteSegment()
{
    delete m_nextRouteSegment;
}

qreal RouteSegment::distance() const
{
    return m_distance;
}

Maneuver RouteSegment::maneuver() const
{
    return m_maneuver;
}

void RouteSegment::setManeuver( const Maneuver &maneuver )
{
    m_maneuver = maneuver;
    m_valid = true;
}

GeoDataLineString RouteSegment::path() const
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

RouteSegment RouteSegment::nextRouteSegment() const
{
    return m_nextRouteSegment ? *m_nextRouteSegment : RouteSegment();
}

void RouteSegment::setNextRouteSegment( const RouteSegment &segment )
{
    if ( !m_nextRouteSegment ) {
        m_nextRouteSegment = new RouteSegment( segment );
    } else {
        *m_nextRouteSegment = segment;
    }

    m_valid = true;
}

bool RouteSegment::isValid() const
{
    return m_valid;
}

qreal RouteSegment::distancePointToLine(const GeoDataCoordinates &p, const GeoDataCoordinates &a, const GeoDataCoordinates &b) const
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
        return EARTH_RADIUS * distanceSphere(p, b);
    } else if ( t > 1.0 ) {
        return EARTH_RADIUS * distanceSphere(p, a);
    } else {
        qreal const nom = qAbs( x21 * y10 - x10 * y21 );
        qreal const den = sqrt( x21 * x21 + y21 * y21 );
        return EARTH_RADIUS * nom / den;
    }
}

qreal RouteSegment::distanceTo( const GeoDataCoordinates &point ) const
{
    Q_ASSERT( !m_path.isEmpty() );

    if ( m_path.size() == 1 ) {
        return EARTH_RADIUS * distanceSphere( m_path.first(), point );
    }

    qreal minDistance = -1.0;
    for ( int i=1; i<m_path.size(); ++i ) {
        qreal const distance = distancePointToLine( point, m_path[i-1], m_path[i] );
        if ( minDistance < 0.0 || distance < minDistance ) {
            minDistance = distance;
        }
    }

    return minDistance;
}

}
