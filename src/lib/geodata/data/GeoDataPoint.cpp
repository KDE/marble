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

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include "global.h"



GeoDataPoint::GeoDataPoint( double _lon, double _lat, double _alt, 
                            GeoDataPoint::Unit unit, int _detail )
  : GeoDataCoordinates( _lon, _lat, _alt, 
                        static_cast<GeoDataCoordinates::Unit>( unit ), _detail )
{
}

GeoDataPoint::GeoDataPoint( const GeoDataPoint& other )
  : GeoDataGeometry( other ),
    GeoDataCoordinates( other )
{
}

GeoDataPoint::GeoDataPoint()
{
}

GeoDataPoint::~GeoDataPoint()
{
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
        double temp;
        if( cycles == 0 ) { // pi/2 < lat < pi
            temp = M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            return ( +M_PI - temp );
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            return ( -M_PI - temp );
        }
        return temp;
    } 
    if ( lat < ( -M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat - M_PI ) / ( 2 * M_PI ) );
        double temp;
        if( cycles == 0 ) { 
            temp = -M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            return ( +M_PI - temp );
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            return ( -M_PI - temp );
        }
        return temp;
    }
    return lat;
}

void GeoDataPoint::normalizeLonLat( double &lon, double &lat )
{
    if ( lon > +M_PI ) {
        int cycles = (int)( ( lon + M_PI ) / ( 2 * M_PI ) );
        lon = lon - ( cycles * 2 * M_PI );
    } 
    if ( lon < -M_PI ) {
        int cycles = (int)( ( lon - M_PI ) / ( 2 * M_PI ) );
        lon = lon - ( cycles * 2 * M_PI );
    }

    if ( lat > ( +M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat + M_PI ) / ( 2 * M_PI ) );
        double temp;
        if( cycles == 0 ) { // pi/2 < lat < pi
            temp = M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            lat =  +M_PI - temp;
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            lat =  -M_PI - temp;
        }
        lat = temp;
        if( lon > 0 ) { 
            lon = -M_PI + lon;
        } else {
            lon = M_PI + lon;
        }
    } 
    if ( lat < ( -M_PI / 2.0 ) ) {
        int cycles = (int)( ( lat - M_PI ) / ( 2 * M_PI ) );
        double temp;
        if( cycles == 0 ) { 
            temp = -M_PI - lat;
        } else {
            temp = lat - ( cycles * 2 * M_PI );
        }
        if ( temp > ( +M_PI / 2.0 ) ) {
            lat =  +M_PI - temp;
        }
        if ( temp < ( -M_PI / 2.0 ) ) {
            lat =  -M_PI - temp;
        }
        lat = temp;
        if( lon > 0 ) { 
            lon = -M_PI + lon;
        } else {
            lon = M_PI + lon;
        }
    } 
    return;
}

GeoDataPoint& GeoDataPoint::operator=( const GeoDataPoint &other )
{
    GeoDataCoordinates::operator=( other );
    return *this;
}

void GeoDataPoint::pack( QDataStream& stream ) const
{
    GeoDataCoordinates::pack( stream );
}

void GeoDataPoint::unpack( QDataStream& stream )
{
    GeoDataCoordinates::unpack( stream );
}
