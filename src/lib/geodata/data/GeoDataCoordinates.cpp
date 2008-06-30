//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "GeoDataCoordinates.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include "global.h"

class GeoDataCoordinatesPrivate
{
  public:
    GeoDataCoordinatesPrivate()
        : m_lon( 0 ),
          m_lat( 0 ),
          m_altitude( 0 )
    {
    }

    Quaternion  m_q;
    double      m_lon;
    double      m_lat;
    double      m_altitude;     // in meters above sea level
};


GeoDataCoordinates::GeoDataCoordinates( double _lon, double _lat, double _alt, GeoDataCoordinates::Unit unit )
  : d( new GeoDataCoordinatesPrivate() )
{
    d->m_altitude = _alt;
    switch( unit ){
    case Radian:
        d->m_q = Quaternion( _lon, _lat );
        d->m_lon = _lon;
        d->m_lat = _lat;
        break;
    case Degree:
        d->m_q = Quaternion( _lon * DEG2RAD , _lat * DEG2RAD  );
        d->m_lon = _lon * DEG2RAD;
        d->m_lat = _lat * DEG2RAD;
        break;
    }
}

GeoDataCoordinates::GeoDataCoordinates(const GeoDataCoordinates& other)
  : d(new GeoDataCoordinatesPrivate(*other.d))
{
}

GeoDataCoordinates::GeoDataCoordinates()
  : d( new GeoDataCoordinatesPrivate() )
{
}

GeoDataCoordinates::~GeoDataCoordinates()
{
    delete d;
}

void GeoDataCoordinates::setAltitude( const double altitude )
{
    d->m_altitude = altitude;
}

void GeoDataCoordinates::geoCoordinates( double& lon, double& lat, 
                               GeoDataCoordinates::Unit unit ) const
{
    switch ( unit ) 
    {
    case Radian:
            lon = d->m_lon;
            lat = d->m_lat;
        break;
    case Degree:
            lon = d->m_lon * RAD2DEG;
            lat = d->m_lat * RAD2DEG;
        break;
    }
}

QString GeoDataCoordinates::toString( GeoDataCoordinates::Notation notation )
{
    QString nsstring = ( d->m_lat > 0 ) ? QCoreApplication::tr("N") : QCoreApplication::tr("S");  
    QString westring = ( d->m_lon < 0 ) ? QCoreApplication::tr("W") : QCoreApplication::tr("E");  

    double lat, lon;
    lon = fabs( d->m_lon * RAD2DEG );
    lat = fabs( d->m_lat * RAD2DEG );

    if ( notation == GeoDataCoordinates::DMS )
    {
        int londeg = (int) lon;
        int lonmin = (int) ( 60 * (lon - londeg) );
        int lonsec = (int) ( 3600 * (lon - londeg - ((double)(lonmin) / 60) ) );


        int latdeg = (int) lat;
        int latmin = (int) ( 60 * (lat - latdeg) );
        int latsec = (int) ( 3600 * (lat - latdeg - ((double)(latmin) / 60) ) );

        return QString("%1\xb0 %2\' %3\"%4, %5\xb0 %6\' %7\"%8")
        .arg(londeg, 3, 10, QChar(' ') ).arg(lonmin, 2, 10, QChar('0') )
        .arg(lonsec, 2, 10, QChar('0') ).arg(westring)
    	.arg(latdeg, 3, 10, QChar(' ') ).arg(latmin, 2, 10, QChar('0') )
        .arg(latsec, 2, 10, QChar('0') ).arg(nsstring);
    }
    else // notation = GeoDataCoordinates::Decimal
    {
        return QString("%L1\xb0%2, %L3\xb0%4")
        .arg(lon, 6, 'f', 3, QChar(' ') ).arg(westring)
        .arg(lat, 6, 'f', 3, QChar(' ') ).arg(nsstring);
    }
}

bool GeoDataCoordinates::operator==( const GeoDataCoordinates &test ) const
{
    // Comparing 2 ints is faster than comparing 4 ints
    // Therefore we compare the Lon-Lat coordinates instead 
    // of the Position-Quaternion.

    double lonTest;
    double latTest;
    double lonThis;
    double latThis;
    
    geoCoordinates( lonThis, latThis );
    test.geoCoordinates( lonTest, latTest );
    
    return ( lonThis == lonTest 
             && latTest == latThis );
}

double GeoDataCoordinates::altitude() const
{
    return d->m_altitude;
}

const Quaternion& GeoDataCoordinates::quaternion() const
{
    return d->m_q;
}

GeoDataCoordinates& GeoDataCoordinates::operator=( const GeoDataCoordinates &other )
{
    // FIXME: check for self assignment is not neccessary here, discuss
    // if it should stay or be removed
    if ( this == &other )
        return *this;
    *d = *other.d;
    return *this;
}
