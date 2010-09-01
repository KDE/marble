//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGPOINT_H
#define MARBLE_ROUTINGPOINT_H

#include "marble_export.h"

#include <QtCore/QTextStream>

namespace Marble
{

/**
  * There are many Point classes, but this is mine.
  */
class MARBLE_EXPORT RoutingPoint
{
public:
    explicit RoutingPoint( qreal lon = 0.0, qreal lat = 0.0 );

    /** Longitude of the point */
    qreal lon() const;

    /** Latitude of the point */
    qreal lat() const;

    /**
      * Calculates the bearing of the line defined by this point
      * and the given other point.
      * Code based on http://www.movable-type.co.uk/scripts/latlong.html
      */
    qreal bearing( const RoutingPoint &other ) const;

    /**
      * Calculates the distance in meter between this point and the
      * given other point.
      * Code based on http://www.movable-type.co.uk/scripts/latlong.html
      */
    qreal distance( const RoutingPoint &other ) const;

private:
    qreal m_lon;

    qreal m_lonRad;

    qreal m_lat;

    qreal m_latRad;
};

QTextStream& operator<<( QTextStream& stream, const RoutingPoint &i );

} // namespace Marble

#endif // MARBLE_ROUTINGPOINT_H
