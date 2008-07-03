//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#include "GeoDataCoordinates_p.h"
#include "Quaternion.h"

class GeoDataPointPrivate : public GeoDataCoordinatesPrivate
{
  public:
    GeoDataPointPrivate()
        : m_detail( 0 )
    {
    }

    GeoDataPointPrivate( const GeoDataPointPrivate& other )
    {
        m_detail = other.m_detail;
    }

    int         m_detail;
};
