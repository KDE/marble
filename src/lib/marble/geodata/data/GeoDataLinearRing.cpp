//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#include "GeoDataLinearRing.h"
#include "GeoDataLinearRing_p.h"

#include "MarbleMath.h"
#include "MarbleDebug.h"

namespace Marble
{

GeoDataLinearRing::GeoDataLinearRing( TessellationFlags f )
    : GeoDataLineString( new GeoDataLinearRingPrivate( f ) )
{
}

GeoDataLinearRing::GeoDataLinearRing( const GeoDataGeometry & other )
  : GeoDataLineString( other )
{
}

GeoDataLinearRing::~GeoDataLinearRing()
{
}

bool GeoDataLinearRing::operator==( const GeoDataLinearRing &other ) const
{
    return isClosed() == other.isClosed() &&
           GeoDataLineString::operator==( other );
}

bool GeoDataLinearRing::operator!=( const GeoDataLinearRing &other ) const
{
    return !this->operator==(other);
}

bool GeoDataLinearRing::isClosed() const
{
    return true;
}

qreal GeoDataLinearRing::length( qreal planetRadius, int offset ) const
{
    qreal  length = GeoDataLineString::length( planetRadius, offset );

    return length + planetRadius * distanceSphere( last(), first() );
}

bool GeoDataLinearRing::contains( const GeoDataCoordinates &coordinates ) const
{
    // Quick bounding box check
    if ( !latLonAltBox().contains( coordinates ) ) {
        return false;
    }

    int const points = size();
    bool inside = false; // also true for points = 0
    int j = points - 1;

    for ( int i=0; i<points; ++i ) {
        GeoDataCoordinates const & one = operator[]( i );
        GeoDataCoordinates const & two = operator[]( j );

        if ( ( one.longitude() < coordinates.longitude() && two.longitude() >= coordinates.longitude() ) ||
             ( two.longitude() < coordinates.longitude() && one.longitude() >= coordinates.longitude() ) ) {
            if ( one.latitude() + ( coordinates.longitude() - one.longitude()) / ( two.longitude() - one.longitude()) * ( two.latitude()-one.latitude() ) < coordinates.latitude() ) {
                inside = !inside;
            }
        }

        j = i;
    }

    return inside;
}

qreal GeoDataLinearRing::area(qreal planetRadius) const
{
    qreal theta1 = 0.0;
    qreal n = size();

    if (n < 3) {
        return 0.0;
    }

    for (int segmentIndex = 1; segmentIndex < n-1; ++segmentIndex) {
        // todo FIXME one bearing call per coordinate would be enough to calculate them all
        GeoDataCoordinates const & current = operator[](segmentIndex);
        qreal prevBearing = current.bearing(operator[](segmentIndex-1));
        qreal nextBearing = current.bearing(operator[](segmentIndex+1));
        if (nextBearing < prevBearing) {
            nextBearing += 2 * M_PI;
        }

        qreal angle = nextBearing - prevBearing;
        theta1 += angle;
    }

    // Traversing first vertex
    GeoDataCoordinates current = operator[](0);
    qreal prevBearing = current.bearing(operator[](n-1));
    qreal nextBearing = current.bearing(operator[](1));
    if (nextBearing < prevBearing) {
        nextBearing += 2 * M_PI;
    }
    qreal angle = nextBearing - prevBearing;
    theta1 += angle;

    // And the last one
    current = operator[](n-1);
    prevBearing = current.bearing(operator[](n-2));
    nextBearing = current.bearing(operator[](0));
    if (nextBearing < prevBearing) {
        nextBearing += 2 * M_PI;
    }
    angle = nextBearing - prevBearing;
    theta1 += angle;

    qreal theta2 = 2 * M_PI * n - theta1;

    // theta = smaller of theta1 and theta2
    qreal theta = (theta1 < theta2) ? theta1 : theta2;

    return qAbs((theta - (n-2) * M_PI) * planetRadius * planetRadius);
}

bool GeoDataLinearRing::isClockwise() const
{
    int const n = size();
    qreal area = 0;
    for ( int i = 1; i < n; ++i ){
        area += ( operator[]( i ).longitude() - operator[]( i - 1 ).longitude() ) * ( operator[]( i ).latitude() + operator[]( i - 1 ).latitude() );
    }
    area += ( operator[]( 0 ).longitude() - operator[]( n - 1 ).longitude() ) * ( operator[] ( 0 ).latitude() + operator[]( n - 1 ).latitude() );

    return area > 0;
}

}
