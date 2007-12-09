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


#include "GeoPoint.h"

#include <cmath>

#include <QtCore/QDebug>

#include "global.h"


GeoPoint::GeoPoint(int _detail, int _lon, int _lat)
  : m_detail(_detail ),
    m_lon( (double)(_lon) / RAD2INT * 2 ),
    m_lat( (double)(_lat) / RAD2INT * 2 ),
    m_altitude(0.0)
{
    m_q = Quaternion( (double)(_lon) / RAD2INT * 2,
		      (double)(_lat) / RAD2INT * 2 );
}

GeoPoint::GeoPoint(int _lon, int _lat)
  : m_lon( (double)(_lon) / RAD2INT * 2 ),
    m_lat( (double)(_lat) / RAD2INT * 2 ),
    m_altitude(0.0)
{
    m_q = Quaternion( (double)(_lon) / RAD2INT * 2,
		      (double)(_lat) / RAD2INT * 2 );
}

GeoPoint::GeoPoint( double _lon, double _lat, double alt, GeoPoint::Unit unit)
{
    m_altitude = alt;

    switch(unit){
    case( GeoPoint::Radian ):
        m_q = Quaternion( _lon, _lat );
        m_lon = _lon;
        m_lat = _lat;
        break;
    case( GeoPoint::Degree ):
        m_q = Quaternion( _lon * DEG2RAD , _lat * -DEG2RAD  );
        m_lon = _lon * DEG2RAD;
        m_lat = _lat * -DEG2RAD;
    }
}

void GeoPoint::setAltitude( const double altitude )
{
    m_altitude = altitude;
}

void GeoPoint::geoCoordinates( double& lon, double& lat, 
                               GeoPoint::Unit unit ) const
{
    switch ( unit ) 
    {
    case Radian:
            lon = m_lon;
            lat = m_lat;
        break;
    case Degree:
            lon = m_lon * RAD2DEG;
            lat = m_lat * -RAD2DEG;
        break;
    }
}

QString GeoPoint::toString( GeoPoint::Notation notation )
{
    QString nsstring = ( m_lat < 0 ) ? "N" : "S";  
    QString westring = ( m_lon < 0 ) ? "W" : "E";  

    double lat, lon;
    lon = fabs( m_lon * RAD2DEG );
    lat = fabs( -m_lat * RAD2DEG );

    if ( notation == GeoPoint::DMS )
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
    else // notation = GeoPoint::Decimal
    {
        return QString("%L1\xb0%2, %L3\xb0%4")
        .arg(lon, 6, 'f', 3, QChar(' ') ).arg(westring)
        .arg(lat, 6, 'f', 3, QChar(' ') ).arg(nsstring);
    }
}

bool GeoPoint::operator==( const GeoPoint &test ) const
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
