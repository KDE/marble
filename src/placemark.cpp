//
// C++ Implementation: gpplacemark
//
// Description: 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "placemark.h"

#include <cmath>
#include "geopoint.h"
#include "katlasdirs.h"

#include <QDebug>

const float deg2rad = M_PI/180.0f;

PlaceMark::PlaceMark() : PlaceMarkStorage(), m_coordinate() {
	m_selected = false;
	m_symbol = 0;
	m_textPixmap = QPixmap();

}

PlaceMark::PlaceMark( QString name ) : PlaceMarkStorage(name), m_coordinate() {
	m_symbol = 0;
	m_textPixmap = QPixmap();
}

void PlaceMark::coordinate( float& lng, float& lat ){
	m_coordinate.geoCoordinates( lng, lat );
}

void PlaceMark::setCoordinate( float lng, float lat ){
	m_coordinate = GeoPoint( deg2rad * lng, -deg2rad * lat );

}

