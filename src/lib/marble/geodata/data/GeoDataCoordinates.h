//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2015      Alejandro Garcia Montoro <alejandro.garciamontoro@gmail.com>
//


#ifndef MARBLE_GEODATACOORDINATES_H
#define MARBLE_GEODATACOORDINATES_H

#include <QCoreApplication>
#include <QMetaType>
#include <QVector>

#include <cmath>

/* M_PI is a #define that may or may not be handled in <cmath> */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419717
#endif

#include "geodata_export.h"
#include "MarbleGlobal.h"

class QString;

namespace Marble
{

const qreal TWOPI = 2 * M_PI;

class GeoDataCoordinatesPrivate;
class Quaternion;

/**
 * @short A 3d point representation
 *
 * GeoDataCoordinates is the simple representation of a single three
 * dimensional point. It can be used all through out marble as the data type
 * for three dimensional objects. it comprises of a Quaternion for speed issues.
 * This class was introduced to reflect the difference between a simple 3d point
 * and the GeoDataGeometry object containing such a point. The latter is a 
 * GeoDataPoint and is simply derived from GeoDataCoordinates.
 * @see GeoDataPoint
*/

class GEODATA_EXPORT GeoDataCoordinates
{
 Q_DECLARE_TR_FUNCTIONS(GeoDataCoordinates)

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
        Decimal, ///< "Decimal" notation (base-10)
        DMS,     ///< "Sexagesimal DMS" notation (base-60)
        DM,      ///< "Sexagesimal DM" notation (base-60)
        UTM,
        Astro    /// < "RA and DEC" notation (used for astronomical sky coordinates)
    };

    /**
     * @brief The BearingType enum specifies where to measure the bearing
     * along great circle arcs
     *
     * When traveling along a great circle arc defined by the two points
     * A and B, the bearing varies along the arc. The "InitialBearing" bearing
     * corresponds to the bearing value at A, the "FinalBearing" bearing to that
     * at B.
     */
    enum BearingType {
        InitialBearing,
        FinalBearing
    };

    // Type definitions
    typedef QVector<GeoDataCoordinates> Vector;
    typedef QVector<GeoDataCoordinates*> PtrVector;

    GeoDataCoordinates( const GeoDataCoordinates& other );

    /**
     * @brief constructs an invalid instance
     *
     * Constructs an invalid instance such that calling isValid()
     * on it will return @code false @endcode.
     */
    GeoDataCoordinates();

    /**
     * @brief create a geocoordinate from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param alt altitude in meters (default: 0)
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     * @param _detail detail (default: 0)
     */
    GeoDataCoordinates( qreal lon, qreal lat, qreal alt = 0,
                        GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian,
                        int detail = 0 );

    virtual ~GeoDataCoordinates();

    /**
     * @brief Returns @code true @endcode if the coordinate is valid, @code false @endcode otherwise.
     * @return whether the coordinate is valid
     *
     * A coordinate is valid, if at least one component has been set and the last
     * assignment was not an invalid GeoDataCoordinates object.
     */
    bool isValid() const;

    /**
    * @brief (re)set the coordinates in a GeoDataCoordinates object
    * @param _lon longitude
    * @param _lat latitude
    * @param alt altitude in meters (default: 0)
    * @param _unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    */
    void set( qreal lon, qreal lat, qreal alt = 0,
              GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
    * @brief use this function to get the longitude and latitude with one
    * call - use the unit parameter to switch between Radian and DMS
    * @param lon longitude
    * @param lat latitude
    * @param unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    */
    void geoCoordinates(qreal& lon, qreal& lat, GeoDataCoordinates::Unit unit) const;
    void geoCoordinates(qreal& lon, qreal& lat) const;

    /**
    * @brief use this function to get the longitude, latitude and altitude
    * with one call - use the unit parameter to switch between Radian and DMS
    * @param lon longitude
    * @param lat latitude
    * @param alt altitude in meters
    * @param unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    */
    void geoCoordinates(qreal& lon, qreal& lat, qreal& alt, GeoDataCoordinates::Unit unit) const;
    void geoCoordinates(qreal& lon, qreal& lat, qreal& alt) const;

    /**
    * @brief set the longitude in a GeoDataCoordinates object
    * @param _lon longitude
    * @param _unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    */
    void setLongitude( qreal lon,
              GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
    * @brief retrieves the longitude of the GeoDataCoordinates object
    * use the unit parameter to switch between Radian and DMS
    * @param unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    * @return longitude
    */
    qreal longitude(GeoDataCoordinates::Unit unit) const;
    qreal longitude() const;

    /**
    * @brief retrieves the latitude of the GeoDataCoordinates object
    * use the unit parameter to switch between Radian and DMS
    * @param unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    * @return latitude
    */
    qreal latitude( GeoDataCoordinates::Unit unit ) const;
    qreal latitude() const;

    /**
    * @brief set the longitude in a GeoDataCoordinates object
    * @param _lat longitude
    * @param _unit units that lon and lat get measured in
    * (default for Radian: north pole at pi/2, southpole at -pi/2)
    */
    void setLatitude( qreal lat,
              GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
        * @brief return the altitude of the Point in meters
        */
    qreal altitude() const;
    /**
    * @brief set the altitude of the Point in meters
    * @param altitude altitude
    */
    void setAltitude( const qreal altitude );

    /**
    * @brief retrieves the UTM zone of the GeoDataCoordinates object.
    * If the point is located on one of the poles (latitude < 80S or
    * latitude > 84N) there is no UTM zone associated; in this case,
    * 0 is returned.
    * @return UTM zone.
    */
    int utmZone() const;

    /**
    * @brief retrieves the UTM easting of the GeoDataCoordinates object,
    * in meters.
    * @return UTM easting
    */
    qreal utmEasting() const;

    /**
    * @brief retrieves the UTM latitude band of the GeoDataCoordinates object
    * @return UTM latitude band
    */
    QString utmLatitudeBand() const;

    /**
    * @brief retrieves the UTM northing of the GeoDataCoordinates object,
    * in meters
    * @return UTM northing
    */
    qreal utmNorthing() const;

    /**
    * @brief return the detail flag
    * detail range: 0 for most important points, 5 for least important
    */
    quint8 detail() const;

    /**
    * @brief set the detail flag
    * @param det detail
    */
    void setDetail(quint8 detail);

    /**
     * @brief Rotates one coordinate around another.
     * @param axis The coordinate that serves as a rotation axis
     * @param angle Rotation angle
     * @param unit Unit of the result
     * @return The coordinate rotated in anticlockwise direction
     */
    GeoDataCoordinates rotateAround( const GeoDataCoordinates &axis, qreal angle, Unit unit = Radian ) const;

    /**
     * @brief Returns the bearing (true bearing, the angle between the line defined
     * by this point and the other and the prime meridian)
     * @param other The second point that, together with this point, defines a line
     * @param unit Unit of the result
     * @return The true bearing in the requested unit, not range normalized,
     * in clockwise direction, with the value 0 corresponding to north
     */
    qreal bearing( const GeoDataCoordinates &other, Unit unit = Radian, BearingType type = InitialBearing ) const;

    /**
     * @brief Returns the coordinates of the resulting point after moving this point
     * according to the distance and bearing parameters
     * @param bearing the same as above
     * @param distance the distance on a unit sphere
     */
    GeoDataCoordinates moveByBearing( qreal bearing, qreal distance ) const;

    /**
    * @brief return a Quaternion with the used coordinates
    */
    const Quaternion &quaternion() const;

    /**
     * @brief slerp (spherical linear) interpolation between this coordinate and the given target coordinate
     * @param target Destination coordinate
     * @param t Fraction 0..1 to weight between this and target
     * @return Interpolated coordinate between this (t<=0.0) and target (t>=1.0)
     */
    GeoDataCoordinates interpolate( const GeoDataCoordinates &target, double t ) const;

    /**
     * @brief squad (spherical and quadrangle) interpolation between b and c
     * @param before First base point
     * @param target Third base point (second interpolation point)
     * @param after Fourth base point
     * @param t Offset between b (t<=0) and c (t>=1)
     */
    GeoDataCoordinates interpolate( const GeoDataCoordinates &before, const GeoDataCoordinates &target, const GeoDataCoordinates &after, double t ) const;

    /**
    * @brief return whether our coordinates represent a pole
    * This method can be used to check whether the coordinate equals one of 
    * the poles. 
    */
    bool isPole( Pole = AnyPole ) const;

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
     * @brief normalize the longitude to always be -M_PI <= lon <= +M_PI (Radian).
     * @param lon longitude
     */
    static qreal normalizeLon( qreal lon,
                               GeoDataCoordinates::Unit = GeoDataCoordinates::Radian );

    /**
     * @brief normalize latitude to always be in -M_PI / 2. <= lat <= +M_PI / 2 (Radian).
     * @param lat latitude
     */
    static qreal normalizeLat( qreal lat,
                               GeoDataCoordinates::Unit = GeoDataCoordinates::Radian );

    /**
     * @brief normalize both longitude and latitude at the same time
     * This method normalizes both latitude and longitude, so that the 
     * latitude and the longitude stay within the "usual" range.
     * NOTE: If the latitude exceeds M_PI/2 (+90.0 deg) or -M_PI/2 (-90.0 deg)
     * then this will be interpreted as a pole traversion where the point will  
     * end up on the opposite side of the globe. Therefore the longitude will 
     * change by M_PI (180 deg).
     * If you don't want this behaviour use both normalizeLat() and
     * normalizeLon() instead.  
     * @param lon the longitude value
     * @param lat the latitude value
     */
    static void normalizeLonLat( qreal &lon, qreal &lat,
                                 GeoDataCoordinates::Unit = GeoDataCoordinates::Radian );
    
    /**
     * @brief try to parse the string into a coordinate pair
     * @param successful becomes true if the conversion succeeds
     * @return the geodatacoordinates
     */     
    static GeoDataCoordinates fromString( const QString &string, bool& successful );
     
    /**
    * @brief return a string representation of the coordinate
    * this is a convenience function which uses the default notation
    */
    QString toString() const;

    /**
    * @brief return a string with the notation given by notation
    *
    * @param notation set a notation different from the default one
    * @param precision set the number of digits below degrees.
    * The precision depends on the current notation: 
    * For Decimal representation the precision is the number of 
    * digits after the decimal point.
    * In DMS a precision of 1 or 2 shows the arc minutes; a precision
    * of 3 or 4 will show arc seconds. A precision beyond that will 
    * increase the number of digits after the arc second decimal point. 
    */
    QString toString( GeoDataCoordinates::Notation notation, int precision = -1 ) const;
    
    static QString lonToString( qreal lon, GeoDataCoordinates::Notation notation,   
                                           GeoDataCoordinates::Unit unit = Radian, 
                                           int precision = -1, 
                                           char format = 'f' );
    /**
     * @brief return a string representation of longitude of the coordinate
     * convenience function that uses the default notation
     */
    QString lonToString() const;

    static QString latToString( qreal lat, GeoDataCoordinates::Notation notation,
                                           GeoDataCoordinates::Unit unit = Radian,
                                           int precision = -1,
                                           char format = 'f' );
    /**
     * @brief return a string representation of latitude of the coordinate
     * convenience function that uses the default notation
     */
    QString latToString() const;

    bool operator==(const GeoDataCoordinates &other) const;
    bool operator!=(const GeoDataCoordinates &other) const;

    GeoDataCoordinates& operator=( const GeoDataCoordinates &other );

    /** Serialize the contents of the feature to @p stream. */
    void pack(QDataStream &stream) const;
    /** Unserialize the contents of the feature from @p stream. */
    void unpack(QDataStream &stream);

 private:
    void detach();

    GeoDataCoordinatesPrivate *d;

    static GeoDataCoordinates::Notation s_notation;
    static const GeoDataCoordinates null;
};

uint qHash(const GeoDataCoordinates& coordinates );


}

Q_DECLARE_METATYPE( Marble::GeoDataCoordinates )

#endif
