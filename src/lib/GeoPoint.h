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

#include <cmath>
#include "Quaternion.h"

const double deg2rad = M_PI/180.0;
const double TWOPI = 2 * M_PI;

class GeoPoint {
 public:
    /**
     * @brief enum used constructor to specify the units used
     */
    enum Unit{Radian, Degree};
    
    GeoPoint(){}
    GeoPoint(int, int);
    /**
     * @brief create a geopoint from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param _unit units that lat and lon are in
     * (north pole at -pi/2, southpole at pi/2)
     */
    GeoPoint(double _lat, double _lon, 
             GeoPoint::Unit _unit = GeoPoint::Radian);
    GeoPoint(int, int, int);
    ~GeoPoint(){}

    int detail()  const { return m_detail; }

    void geoCoordinates( double& lon, double& lat ) const {
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
