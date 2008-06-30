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


#ifndef GEODATAPOINT_H
#define GEODATAPOINT_H

#include <QtCore/QMetaType>
#include <QtCore/QVector>

#include <cmath>

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataCoordinates.h"
#include "Quaternion.h"

class GeoDataPointPrivate;

class GEODATA_EXPORT GeoDataPoint : public GeoDataCoordinates, public GeoDataGeometry {
 public:
    // Type definitions
    typedef QVector<GeoDataPoint> Vector;
//    typedef GeoDataCoordinates::Notation GeoDataPoint::Notation;
//    typedef GeoDataCoordinates::Unit GeoDataPoint::Unit;

    GeoDataPoint(const GeoDataPoint& other);
    GeoDataPoint();

    /**
     * @brief create a geopoint from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param alt altitude (default: 0)
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    GeoDataPoint(double _lon, double _lat, double alt = 0,
             GeoDataPoint::Unit _unit = GeoDataPoint::Radian, int _detail = 0);

    ~GeoDataPoint();

    int    detail()   const;

    bool operator==(const GeoDataPoint&) const;
    GeoDataPoint& operator=(const GeoDataPoint &other);

    static double normalizeLon( double lon );

    static double normalizeLat( double lat );

 private:
    GeoDataPointPrivate * const d;
};

Q_DECLARE_METATYPE( GeoDataPoint )

#endif // GEODATAPOINT_H
