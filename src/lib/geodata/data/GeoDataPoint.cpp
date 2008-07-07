//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#include "GeoDataPoint.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPoint_p.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include "global.h"



GeoDataPoint::GeoDataPoint( double _lon, double _lat, double _alt, 
                            GeoDataPoint::Unit unit, int _detail )
  : GeoDataCoordinates( _lon, _lat, _alt, 
                        static_cast<GeoDataCoordinates::Unit>( unit ) ), 
    d( new GeoDataPointPrivate() )
{
    d->m_detail = _detail;
}

GeoDataPoint::GeoDataPoint( const GeoDataPoint& other )
  : GeoDataGeometry( other ), 
    GeoDataCoordinates( other ), 
    d( new GeoDataPointPrivate( *other.d ) )
{
}

GeoDataPoint::GeoDataPoint()
  : d( new GeoDataPointPrivate() )
{
}

GeoDataPoint::~GeoDataPoint()
{
    delete d;
}

double GeoDataPoint::normalizeLon( double lon )
{
    if ( lon > +M_PI ) {
        int cycles = (int)( ( lon + M_PI ) / ( 2 * M_PI ) );
        return lon - ( cycles * 2 * M_PI );
    } 
    if ( lon < -M_PI ) {
        int cycles = (int)( ( lon - M_PI ) / ( 2 * M_PI ) );
        return lon - ( cycles * 2 * M_PI );
    }

    return lon;
}

double GeoDataPoint::normalizeLat( double lat )
{
    if ( lat > ( +M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat + M_PI ) / ( 2 * M_PI ) );
        double temp = lat - ( cycles * 2 * M_PI );
        if ( temp > ( +M_PI / 2.0 ) ) {
            return +M_PI - temp;
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            return -M_PI - temp;
        }
    } 
    if ( lat < ( -M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat - M_PI ) / ( 2 * M_PI ) );
        double temp = lat - ( cycles * 2 * M_PI );
        if ( temp > ( +M_PI / 2.0 ) ) {
            return +M_PI - temp;
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            return -M_PI - temp;
        }
    } 

    return lat;
}

int GeoDataPoint::detail() const
{
    return d->m_detail;
}

void GeoDataPoint::setDetail( int det )
{
    d->m_detail = det;
}

const Quaternion& GeoDataPoint::quaternion() const
{
        return GeoDataCoordinates::quaternion();
}

GeoDataPoint& GeoDataPoint::operator=( const GeoDataPoint &other )
{
    GeoDataCoordinates::operator=( other );
    *d = *other.d;
    return *this;
}

void GeoDataPoint::pack( QDataStream& stream ) const
{
    GeoDataCoordinates::pack( stream );
    
    stream << d->m_detail;
}

void GeoDataPoint::unpack( QDataStream& stream )
{
    GeoDataCoordinates::unpack( stream );
    
    stream >> d->m_detail;
}
