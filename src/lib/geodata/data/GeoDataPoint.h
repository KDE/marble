//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
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

class GEODATA_EXPORT GeoDataPoint : public GeoDataCoordinates,
                                    public GeoDataGeometry {
 public:
    typedef GeoDataCoordinates::Notation Notation;
    typedef GeoDataCoordinates::Unit Unit;

    GeoDataPoint( const GeoDataPoint& other );
    GeoDataPoint();

    /**
     * @brief create a geopoint from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param alt altitude (default: 0)
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    GeoDataPoint( double _lon, double _lat, double alt = 0,
                  GeoDataPoint::Unit _unit = GeoDataPoint::Radian, int _detail = 0 );

    ~GeoDataPoint();

    int detail()   const;
    void setDetail( int det );

    const Quaternion &quaternion() const;

    GeoDataPoint& operator=( const GeoDataPoint &other );

    // Type definitions
    typedef QVector<GeoDataPoint> Vector;

    /**
     * @brief normalize the longitude to always be -M_PI <= lon <= +M_PI
     * @param lon longitude
     */
    static double normalizeLon( double lon );

    /**
     * @brief normalize latitude to always be in -M_PI / 2. <= lat <= +M_PI / 2.
     * @param lat latitude
     */
    static double normalizeLat( double lat );

 private:
    GeoDataPointPrivate* const d;
};

Q_DECLARE_METATYPE( GeoDataPoint )

#endif // GEODATAPOINT_H
