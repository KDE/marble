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


const double rad2int = 21600.0 / M_PI;

GeoPoint::GeoPoint(int _detail, int _lon, int _lat)
  : m_detail(_detail)
{
    m_q = Quaternion( (double)(_lon) / rad2int * 2,
		      (double)(_lat) / rad2int * 2 );
}

GeoPoint::GeoPoint(int _lon, int _lat)
{
    m_q = Quaternion( (double)(_lon) / rad2int * 2,
		      (double)(_lat) / rad2int * 2 );
}

GeoPoint::GeoPoint( double _lon, double _lat, GeoPoint::Unit unit)
{
    switch(unit){
    case( GeoPoint::Radian ):
        m_q = Quaternion( _lon, _lat );
        break;
    case( GeoPoint::Degree ):
        m_q = Quaternion( _lon * deg2rad , _lat * -deg2rad  );
    }
}


QString GeoPoint::toString()
{
    double lat, lon;
    geoCoordinates( lon, lat );

    QString nsstring = ( lat < 0 ) ? "N" : "S";  
    QString westring = ( lon < 0 ) ? "W" : "E";  

    lon = fabs( lon * 180.0 / M_PI );

    int londeg = (int) lon;
    int lonmin = (int) ( 60 * (lon - londeg) );
    int lonsec = (int) ( 3600 * (lon - londeg - ((double)(lonmin) / 60) ) );

    lat = fabs( lat * 180.0 / M_PI );

    int latdeg = (int) lat;
    int latmin = (int) ( 60 * (lat - latdeg) );
    int latsec = (int) ( 3600 * (lat - latdeg - ((double)(latmin) / 60) ) );

    return QString("%1\xb0 %2\' %3\" %4, %5\xb0 %6\' %7\" %8")
	.arg(latdeg).arg(latmin).arg(latsec).arg(nsstring)
	.arg(londeg).arg(lonmin).arg(lonsec).arg(westring);
}
