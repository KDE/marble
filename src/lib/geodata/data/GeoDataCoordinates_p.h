//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATACOORDINATES_P_H
#define GEODATACOORDINATES_P_H

#include "Quaternion.h"
#if QT_VERSION < 0x040400
# include <qatomic.h>
#else
# include <QtCore/QAtomicInt>
#endif

namespace Marble
{
#if QT_VERSION < 0x040400
    typedef QAtomic QAtomicInt;
#endif

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

    Quaternion m_q;
    qreal      m_lon;
    qreal      m_lat;
    qreal      m_altitude;     // in meters above sea level
    int        m_detail;
    QAtomicInt ref;
};

}

#endif

