//
// C++ Implementation: gpgeopoint
//
// Description: 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <cmath>

#include <QtCore/QDebug>

#include "GeoPoint.h"


const float rad2int = 21600.0 / M_PI;

GeoPoint::GeoPoint(int _detail, int _lon, int _lat)
  : m_detail(_detail)
{
    m_q = Quaternion( (float)(_lon) / rad2int * 2,
		      (float)(_lat) / rad2int * 2 );
}

GeoPoint::GeoPoint(int _lon, int _lat)
{
    m_q = Quaternion( (float)(_lon) / rad2int * 2,
		      (float)(_lat) / rad2int * 2 );
}

GeoPoint::GeoPoint( float _lon, float _lat )
{
    m_q = Quaternion( _lon, _lat );
}


QString GeoPoint::toString()
{
    float lat, lon;
    geoCoordinates( lon, lat );

    QString nsstring = ( lat < 0 ) ? "N" : "S";  
    QString westring = ( lon < 0 ) ? "W" : "E";  

    lon = fabs( lon * 180.0 / M_PI );

    int londeg = (int) lon;
    int lonmin = (int) ( 60 * (lon - londeg) );
    int lonsec = (int) ( 3600 * (lon - londeg - ((float)(lonmin) / 60) ) );

    lat = fabs( lat * 180.0 / M_PI );

    int latdeg = (int) lat;
    int latmin = (int) ( 60 * (lat - latdeg) );
    int latsec = (int) ( 3600 * (lat - latdeg - ((float)(latmin) / 60) ) );

    return QString("%1\xb0 %2\' %3\" %4, %5\xb0 %6\' %7\" %8")
	.arg(latdeg).arg(latmin).arg(latsec).arg(nsstring)
	.arg(londeg).arg(lonmin).arg(lonsec).arg(westring);
}
