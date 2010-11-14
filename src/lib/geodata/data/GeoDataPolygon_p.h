//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAPOLYGONPRIVATE_H
#define MARBLE_GEODATAPOLYGONPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataPolygonPrivate : public GeoDataGeometryPrivate
{
  public:
     GeoDataPolygonPrivate( TessellationFlags f )
         : m_dirtyBox( true ),
           m_tessellationFlags( f )
    {
    }

     GeoDataPolygonPrivate()
         : m_dirtyBox( true )
    {
    }

    virtual GeoDataGeometryPrivate* copy()
    { 
         GeoDataPolygonPrivate* copy = new  GeoDataPolygonPrivate;
        *copy = *this;
        return copy;
    }

    virtual QString nodeType() const
    {
        return GeoDataTypes::GeoDataPolygonType;
    }

    virtual EnumGeometryId geometryId() const
    {
        return GeoDataPolygonId;
    }

    GeoDataLinearRing           outer;
    QVector<GeoDataLinearRing>  inner;
    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 
    TessellationFlags           m_tessellationFlags;
};

} // namespace Marble

#endif
