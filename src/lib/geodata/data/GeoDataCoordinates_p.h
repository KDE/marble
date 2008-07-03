//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#include "Quaternion.h"

class GeoDataCoordinatesPrivate
{
  public:
    GeoDataCoordinatesPrivate()
        : m_lon( 0 ),
          m_lat( 0 ),
          m_altitude( 0 )
    {
    }

    GeoDataCoordinatesPrivate( const GeoDataCoordinatesPrivate &other )
        : m_lon( other.m_lon ),
          m_lat( other.m_lat ),
          m_altitude( other.m_altitude )
    {
        m_q.set( m_lon, m_lat );
    }

    GeoDataCoordinatesPrivate& operator=( const GeoDataCoordinatesPrivate &other )
    {
        m_lon = other.m_lon;
        m_lat = other.m_lat;
        m_altitude = other.m_altitude;
        m_q.set( m_lon, m_lat );
        return *this;
    }

    Quaternion  m_q;
    double      m_lon;
    double      m_lat;
    double      m_altitude;     // in meters above sea level
};
