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

const double TWOPI = 2 * M_PI;

class GeoPoint {
 public:
    /**
     * @brief enum used constructor to specify the units used
     *
     * Internally we always use radian for mathematical convenience.
     * However the Marble's interfaces to the outside should default 
     * to degrees.
     */
    enum Unit{Radian, Degree};
    /**
     * @brief enum used to specify the notation / numerical system
     *
     * For degrees there exist two notations: 
     * "Decimal" (base-10) and the "Sexagesimal DMS" (base-60) which is 
     * traditionally used in cartography. Decimal notation 
     * uses floating point numbers to specify parts of a degree. The 
     * Sexagesimal DMS notation uses integer based 
     * Degrees-(Arc)Minutes-(Arc)Seconds to describe parts of a degree. 
     */
    enum Notation{Decimal, DMS};

    GeoPoint(){}
    GeoPoint(int _lon, int _lat);
    /**
     * @brief create a geopoint from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at -pi/2, southpole at pi/2)
     */
    GeoPoint(double _lon, double _lat, 
             GeoPoint::Unit _unit = GeoPoint::Radian);

    GeoPoint(int _detail, int _lon, int _lat);

    ~GeoPoint(){}

    int detail()  const { return m_detail; }

    void geoCoordinates( double& lon, double& lat, 
                         GeoPoint::Unit unit = GeoPoint::Radian )
                                                                const;
 
    const Quaternion &quaternion() const { return m_q; }
    
    QString toString( GeoPoint::Notation notation = GeoPoint::DMS );
    bool        operator==(const GeoPoint&);

    // Type definitions
    typedef QVector<GeoPoint> Vector;

 private:
    Quaternion  m_q;
    int         m_detail;
};


#endif // GEOPOINT_H
