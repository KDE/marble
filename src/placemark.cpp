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

PlaceMark::PlaceMark() : PlaceMarkStorage(), m_coordinate() {
	m_selected = false;
	m_symbol = 0;
	m_symbolPixmap = QPixmap();
	m_labelPixmap = QPixmap();

}

PlaceMark::PlaceMark( QString name ) : PlaceMarkStorage(name), m_coordinate() {
	m_symbol = 0;
	m_symbolPixmap = QPixmap();
	m_labelPixmap = QPixmap();
}

void PlaceMark::coordinate( float& lng, float& lat ){
	m_coordinate.geoCoordinates( lng, lat );
}

void PlaceMark::setCoordinate( float lng, float lat ){
	m_coordinate = GeoPoint( lng, lat );

}

const QPixmap PlaceMark::symbolPixmap()const{
	return m_symbolPixmap;
}
