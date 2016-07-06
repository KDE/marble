//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAGEOMETRYPRIVATE_H
#define MARBLE_GEODATAGEOMETRYPRIVATE_H

#include <QAtomicInt>

#include "GeoDataGeometry.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataGeometryPrivate
{
  public:
    GeoDataGeometryPrivate()
        : m_extrude( false ),
          m_altitudeMode( ClampToGround ),
          ref( 0 )
    {
    }

    GeoDataGeometryPrivate( const GeoDataGeometryPrivate& other )
        : m_extrude( other.m_extrude ),
          m_altitudeMode( other.m_altitudeMode ),
          m_latLonAltBox(),
          ref( 0 )
    {
    }

    virtual ~GeoDataGeometryPrivate()
    {
    }

    GeoDataGeometryPrivate& operator=( const GeoDataGeometryPrivate &other )
    {
        m_extrude = other.m_extrude;
        m_altitudeMode = other.m_altitudeMode;
        m_latLonAltBox = other.m_latLonAltBox;
        return *this;
    }

    virtual GeoDataGeometryPrivate* copy()
    { 
        GeoDataGeometryPrivate* copy = new GeoDataGeometryPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataGeometryType;
    }

    virtual EnumGeometryId geometryId() const
    {
        return InvalidGeometryId;
    }

    bool         m_extrude;
    AltitudeMode m_altitudeMode;
    mutable GeoDataLatLonAltBox m_latLonAltBox;

    QAtomicInt  ref;
};

} // namespace Marble

#endif
