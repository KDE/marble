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
#include <QtCore/QAtomicInt>

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
        : m_lon( 0 ),
          m_lat( 0 ),
          m_altitude( 0 ),
          m_detail( 0 ),
          ref( 1 )
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
        : m_altitude( _alt ),
          m_detail( _detail ),
          ref( 1 )
    {
        switch( unit ){
        default:
        case GeoDataCoordinates::Radian:
            m_q = Quaternion( _lon, _lat );
            m_lon = _lon;
            m_lat = _lat;
            break;
        case GeoDataCoordinates::Degree:
            m_q = Quaternion( _lon * DEG2RAD , _lat * DEG2RAD  );
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
        : m_lon( other.m_lon ),
          m_lat( other.m_lat ),
          m_altitude( other.m_altitude ),
          m_detail( other.m_detail ),
          ref( other.ref )
    {
        m_q.set( m_lon, m_lat );
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
        m_q.set( m_lon, m_lat );
        ref = other.ref;
        return *this;
    }

    bool operator==( const GeoDataCoordinatesPrivate &rhs ) const;

    Quaternion m_q;
    qreal      m_lon;
    qreal      m_lat;
    qreal      m_altitude;     // in meters above sea level
    int        m_detail;
    QAtomicInt ref;
};

inline bool GeoDataCoordinatesPrivate::operator==( const GeoDataCoordinatesPrivate &rhs ) const
{
    // do not compare the m_detail member as it does not really belong to
    // GeoDataCoordinates and should be removed
    return m_lon == rhs.m_lon && m_lat == rhs.m_lat && m_altitude == rhs.m_altitude;
}

}

#endif
