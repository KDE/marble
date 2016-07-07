//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutingPoint.h"

#include <QTextStream>

#include <cmath>

namespace Marble
{

RoutingPoint::RoutingPoint( qreal lon, qreal lat ) :
        m_lon( lon), m_lonRad( lon * M_PI / 180.0 ),
        m_lat( lat ), m_latRad( lat * M_PI / 180.0 )
{
    // nothing to do
}

qreal RoutingPoint::lon() const
{
    return m_lon;
}

qreal RoutingPoint::lat() const
{
    return m_lat;
}

// Code based on http://www.movable-type.co.uk/scripts/latlong.html
qreal RoutingPoint::bearing( const RoutingPoint &other ) const
{
    qreal deltaLon = other.m_lonRad - m_lonRad;
    qreal y = sin( deltaLon ) * cos( other.m_latRad );
    qreal x = cos( m_latRad ) * sin( other.m_latRad ) -
              sin( m_latRad ) * cos( other.m_latRad ) * cos( deltaLon );
    return atan2( y, x );
}

// From MarbleMath::distanceSphere
qreal RoutingPoint::distance( const RoutingPoint &other ) const
{
    qreal h1 = sin( 0.5 * ( other.m_latRad - m_latRad ) );
    qreal h2 = sin( 0.5 * ( other.m_lonRad - m_lonRad ) );
    qreal d = h1 * h1 + cos( m_latRad ) * cos( other.m_latRad ) * h2 * h2;

    return 6378137.0 * 2.0 * atan2( sqrt( d ), sqrt( 1.0 - d ) );
}

QTextStream& operator<<( QTextStream& stream, const RoutingPoint &p )
{
    stream << "(" << p.lon() << ", " << p.lat() << ")";
    return stream;
}

} // namespace Marble
