//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef GEOPOINT_H
#define GEOPOINT_H

#include <QtCore/QVector>

#include "Quaternion.h"


class GeoPoint {
 public:
    GeoPoint(){}
    GeoPoint(int, int);
    GeoPoint(float, float);
    GeoPoint(int, int, int);
    ~GeoPoint(){}
	
    int detail()  const { return m_detail; }

    void geoCoordinates( float& lon, float& lat ) const {
	m_q.getSpherical( lon, lat ); 
    }

    const Quaternion &quaternion() const { return m_q; }
    QString toString();

    // Type definitions
    typedef QVector<GeoPoint> Vector;

 private:	
    Quaternion  m_q;
    int         m_detail;
};


#endif // GEOPOINT_H
