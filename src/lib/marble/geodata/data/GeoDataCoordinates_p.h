//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATACOORDINATES_P_H
#define MARBLE_GEODATACOORDINATES_P_H

#include "Quaternion.h"
#include <QAtomicInt>

namespace Marble
{

class GeoDataCoordinatesPrivate
{
  public:
    /*
    * if this ctor is called there exists exactly one GeoDataCoordinates object
    * with this data.
    * changes will be made in the GeoDataCoordinates class
    * ref must be called ref as qAtomicAssign used in GeoDataCoordinates::operator=
    * needs this name. Maybe we can rename it to our scheme later on.
    */
    GeoDataCoordinatesPrivate()
        : m_q( 0 ),
          m_lon( 0 ),
          m_lat( 0 ),
          m_altitude( 0 ),
          m_detail( 0 ),
          ref( 0 )
    {
    }

    /*
    * if this ctor is called there exists exactly one GeoDataCoordinates object
    * with this data.
    * changes will be made in the GeoDataCoordinates class
    * ref must be called ref as qAtomicAssign used in GeoDataCoordinates::operator=
    * needs this name. Maybe we can rename it to our scheme later on.
    */
    GeoDataCoordinatesPrivate( qreal _lon, qreal _lat, qreal _alt,
                        GeoDataCoordinates::Unit unit,
                        int _detail )
        : m_q( 0 ),
          m_altitude( _alt ),
          m_detail( _detail ),
          ref( 0 )
    {
        switch( unit ){
        default:
        case GeoDataCoordinates::Radian:
            m_lon = _lon;
            m_lat = _lat;
            break;
        case GeoDataCoordinates::Degree:
            m_lon = _lon * DEG2RAD;
            m_lat = _lat * DEG2RAD;
            break;
        }
    }

    /*
    * this constructor is needed as Quaternion doesn't define a copy ctor
    * initialize the reference with the value of the other
    */
    GeoDataCoordinatesPrivate( const GeoDataCoordinatesPrivate &other )
        : m_q( 0 ),
          m_lon( other.m_lon ),
          m_lat( other.m_lat ),
          m_altitude( other.m_altitude ),
          m_detail( other.m_detail ),
          ref( 0 )
    {
    }

    /*
    * return this instead of &other
    */
    GeoDataCoordinatesPrivate& operator=( const GeoDataCoordinatesPrivate &other )
    {
        m_lon = other.m_lon;
        m_lat = other.m_lat;
        m_altitude = other.m_altitude;
        m_detail = other.m_detail;
        ref = 0;
        delete m_q;
        m_q = 0;
        return *this;
    }

    bool operator==( const GeoDataCoordinatesPrivate &rhs ) const;
    bool operator!=( const GeoDataCoordinatesPrivate &rhs ) const;

    static Quaternion basePoint( const Quaternion &q1, const Quaternion &q2, const Quaternion &q3 );

    // Helper functions for UTM-related development.
    // Based on Chuck Taylor work:
    // http://home.hiwaay.net/~taylorc/toolbox/geography/geoutm.html

    /**
    * Computes the ellipsoidal distance from the equator to a point at a
    * given latitude.
    *
    * Reference: Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
    * GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
    *
    * @param phi Latitude of the point, in radians.
    * @return The ellipsoidal distance of the point from the equator, in meters.
    */
    static qreal arcLengthOfMeridian( qreal phi );

    /**
    * Determines the central meridian for the given UTM zone.
    *
    * @param zone An integer value designating the UTM zone, range [1,60].
    * @return The central meridian for the given UTM zone, in radians, or zero
    * if the UTM zone parameter is outside the range [1,60].
    * Range of the central meridian is the radian equivalent of [-177,+177].
    */
    static qreal centralMeridianUTM( qreal zone );


    /**
    * Computes the footpoint latitude for use in converting transverse
    * Mercator coordinates to ellipsoidal coordinates.
    *
    * Reference: Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
    *   GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
    *
    * @param northing The UTM northing coordinate, in meters.
    * @return The footpoint latitude, in radians.
    */
    static qreal footpointLatitude( qreal northing );

    /**
    * Converts a latitude/longitude pair to x and y coordinates in the
    * Transverse Mercator projection.  Note that Transverse Mercator is not
    * the same as UTM; a scale factor is required to convert between them.
    *
    * Reference: Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
    * GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
    *
    * @param lambda Longitude of the point, in radians.
    * @param phi Latitude of the point, in radians.
    * @param lambda0 Longitude of the central meridian to be used, in radians.
    * @return The computed point with its x and y coordinates
    */
    static QPointF mapLonLatToXY( qreal lambda, qreal phi, qreal lambda0 );

    /**
     * Converts a latitude/longitude pair to x and y coordinates in the
     * Universal Transverse Mercator projection.
     *
     * @param lon Longitude of the point, in radians.
     * @param lat Latitude of the point, in radians.
     * @param zone UTM zone between 1 and 60 to be used for calculating
     * values for x and y.
     * @return A point with its x and y coordinates representing
     * easting and northing of the UTM coordinates computed.
     */
    static QPointF lonLatToUTMXY( qreal lon, qreal lat, qreal zone );

    /**
    * @brief retrieves the UTM latitude band of a longitude/latitude
    * pair
    * @param lon longitude, in radians
    * @param lat latitude, in radians
    * @return latitude band
    */
    static QString lonLatToLatitudeBand( qreal lon, qreal lat );

    /**
    * @brief retrieves the northing value of a longitude/latitude
    * pair
    * @param lon longitude, in radians
    * @param lat latitude, in radians
    * @return UTM northing value
    */
    static qreal lonLatToNorthing( qreal lon, qreal lat );

    /**
    * @brief retrieves the UTM zone number of a longitude/latitude
    * pair
    * @param lon longitude, in radians
    * @param lat latitude, in radians
    * @return UTM zone number
    */
    static int lonLatToZone( qreal lon, qreal lat );

    /**
    * @brief  retrieves the easting value of a longitude/latitude
    * pair
    * @param lon longitude, in radians
    * @param lat latitude, in radians
    * @return UTM easting value
    */
    static qreal lonLatToEasting( qreal lon, qreal lat );

    Quaternion * m_q;
    qreal      m_lon;
    qreal      m_lat;
    qreal      m_altitude;     // in meters above sea level
    quint8     m_detail;
    QAtomicInt ref;

    /* UTM Ellipsoid model constants (actual values here are for WGS84) */
    static const qreal sm_semiMajorAxis;
    static const qreal sm_semiMinorAxis;
    static const qreal sm_eccentricitySquared;
    static const qreal sm_utmScaleFactor;

};

inline bool GeoDataCoordinatesPrivate::operator==( const GeoDataCoordinatesPrivate &rhs ) const
{
    // do not compare the m_detail member as it does not really belong to
    // GeoDataCoordinates and should be removed
    return m_lon == rhs.m_lon && m_lat == rhs.m_lat && m_altitude == rhs.m_altitude;
}

inline bool GeoDataCoordinatesPrivate::operator!=( const GeoDataCoordinatesPrivate &rhs ) const
{
    // do not compare the m_detail member as it does not really belong to
    // GeoDataCoordinates and should be removed
    return ! (*this == rhs);
}

}

#endif
