//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#ifndef GEODATACOORDINATES_H
#define GEODATACOORDINATES_H

#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QString>

#include <cmath>

#include "geodata_export.h"
#include "Quaternion.h"

/* M_PI is a #define that may or may not be handled in <cmath> */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419717
#endif

/*
*/

const double TWOPI = 2 * M_PI;

class GeoDataCoordinatesPrivate;

/** GeoDataCoordinates is the simple representation of a single three
 * dimensional point. It can be used all through out marble as the data type
 * for three dimensional objects. it comprises of a Quaternion for speed issues.
 * This class was introduced to reflect the difference between a simple 3d point
 * and the GeoDataGeometry object containing such a point. The latter is a 
 * GeoDataPoint and is simply derived from GeoDataCoordinates.
 * @see GeoDataPoint
*/

class GEODATA_EXPORT GeoDataCoordinates {

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

    // Type definitions
    typedef QVector<GeoDataCoordinates> Vector;
    typedef QVector<GeoDataCoordinates*> PtrVector;

    GeoDataCoordinates( const GeoDataCoordinates& other );
    GeoDataCoordinates();

    /**
     * @brief create a geocoordinate from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param alt altitude (default: 0)
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     * @param _detail detail (default: 0)
     */
    GeoDataCoordinates( double _lon, double _lat, double alt = 0,
             GeoDataCoordinates::Unit _unit = GeoDataCoordinates::Radian, int _detail = 0 );

    virtual ~GeoDataCoordinates();

    /**
        * @brief return the altitude of the Point
        */
    double altitude() const;
    /**
    * @brief set the altitude of the Point
    * @param altitude altitude
    */
    void   setAltitude( const double altitude );

    /**
    * @brief return the detail flag
    */
    int detail()   const;
    /**
    * @brief set the detail flag
    * @param det detail
    */
    void setDetail( const int det );

    /**
    * @brief (re)set the coordinates in a GeoDataCoordinates
    * @param _lon longitude
    * @param _lat latitude
    * @param alt altitude (default: 0)
    * @param _unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    */
    void set(double _lon, double _lat, double alt = 0,
             GeoDataCoordinates::Unit _unit = GeoDataCoordinates::Radian );

    /**
    * @brief use this function to get the longitude and latitude with one
    * call - use the unit parameter to switch between Radian and DMS
    * @param lon longitude
    * @param lat latitude
    * @param unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    */
    void geoCoordinates( double& lon, double& lat,
                         GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian )
                                                                const;

    /**
    * @brief return a Quaternion with the used coordinates
    */
    const Quaternion &quaternion() const;

    /**
    * @brief return Notation of string representation
    */
    static GeoDataCoordinates::Notation defaultNotation();
    /**
    * @brief set the Notation of the string representation
    * @param notation Notation
    */
    static void setDefaultNotation( GeoDataCoordinates::Notation notation );

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

    /**
     * @brief normalize both longitude and latitude at the same time
     * @param lon the longitude value
     * @param lat the latitude value
     */
    static void normalizeLonLat( double &lon, double &lat );

    /**
    * @brief return a string representation of the coordinate
    * this is a convenience function which uses the default notation
    */
    QString toString();
    /**
    * @brief return a string with the notation given by notation
    * @param notation set a notation different from the default one
    */
    QString toString( GeoDataCoordinates::Notation notation );
    
    
    bool operator==( const GeoDataCoordinates& ) const;
    GeoDataCoordinates& operator=( const GeoDataCoordinates &other );

    /** Serialize the contents of the feature to @p stream. */
    virtual void pack( QDataStream& stream ) const;
    /** Unserialize the contents of the feature from @p stream. */
    virtual void unpack( QDataStream& stream );

 protected:
    GeoDataCoordinatesPrivate* const d;

 private:
    static GeoDataCoordinates::Notation s_notation;
};

Q_DECLARE_METATYPE( GeoDataCoordinates )

#endif // GEODATACOORDINATES_H
