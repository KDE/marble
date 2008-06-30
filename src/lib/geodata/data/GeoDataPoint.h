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
#include "Quaternion.h"

/* M_PI is a #define that may or may not be handled in <cmath> */
#ifndef M_PI 
#define M_PI 3.14159265358979323846264338327950288419717
#endif

const double TWOPI = 2 * M_PI;

class GeoDataPointPrivate;

class GEODATA_EXPORT GeoDataPoint : public GeoDataGeometry {
 public:
    /**
     * @brief enum used constructor to specify the units used
     *
     * Internally we always use radian for mathematical convenience.
     * However the Marble's interfaces to the outside should default 
     * to degrees.
     */
    enum Unit{
        Radian,
        Degree
    };
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
    enum Notation{
        Decimal,
        DMS
    };

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

    double altitude() const;
    void   setAltitude( const double altitude );

    int    detail()   const;

    void geoCoordinates( double& lon, double& lat, 
                         GeoDataPoint::Unit unit = GeoDataPoint::Radian )
                                                                const;

    const Quaternion &quaternion() const;

    QString toString( GeoDataPoint::Notation notation = GeoDataPoint::DMS );
    bool operator==(const GeoDataPoint&) const;
    GeoDataPoint& operator=(const GeoDataPoint &other);

    // Type definitions
    typedef QVector<GeoDataPoint> Vector;

    static double normalizeLon( double lon );

    static double normalizeLat( double lat );

 private:
    GeoDataPointPrivate * const d;
};

Q_DECLARE_METATYPE( GeoDataPoint )

#endif // GEODATAPOINT_H
