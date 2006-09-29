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
#include <QDebug>

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

