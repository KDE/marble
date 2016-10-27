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
