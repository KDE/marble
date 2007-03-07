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

#include "geopoint.h"

const float rad2int = 21600.0 / M_PI;

GeoPoint::GeoPoint(int detail, int lng, int lat):m_Detail(detail), m_Lat(lat), m_Lng(lng){
	m_q = Quaternion((float)(lng) / rad2int * 2, (float)(lat) / rad2int * 2);
}

GeoPoint::GeoPoint(int lng, int lat){
	m_q = Quaternion((float)(lng) / rad2int * 2, (float)(lat) / rad2int * 2);
}

GeoPoint::GeoPoint( float lng, float lat ){
	m_q = Quaternion( lng, lat );
}

QString GeoPoint::toString(){
	float lat, lng;
	geoCoordinates( lng, lat );

	QString nsstring = ( lat < 0 ) ? "N" : "S";  
	QString westring = ( lng < 0 ) ? "W" : "E";  

	lng = fabs( lng * 180.0 / M_PI );

	int lngdeg = (int) lng;
	int lngmin = (int) ( 60 * (lng - lngdeg) );
	int lngsec = (int) ( 3600 * (lng - lngdeg - ((float)(lngmin) / 60) ) );

	lat = fabs( lat * 180.0 / M_PI );

	int latdeg = (int) lat;
	int latmin = (int) ( 60 * (lat - latdeg) );
	int latsec = (int) ( 3600 * (lat - latdeg - ((float)(latmin) / 60) ) );

	return QString("%1\xb0 %2\' %3\" %4, %5\xb0 %6\' %7\" %8").arg(latdeg).arg(latmin).arg(latsec).arg(nsstring).arg(lngdeg).arg(lngmin).arg(lngsec).arg(westring);

}
