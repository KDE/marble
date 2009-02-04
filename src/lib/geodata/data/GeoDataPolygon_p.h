//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATAPOLYGONPRIVATE_H
#define GEODATAPOLYGONPRIVATE_H

#include "GeoDataGeometry_p.h"

namespace Marble
{

class  GeoDataPolygonPrivate : public Marble::GeoDataGeometryPrivate
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

    virtual void* copy() 
    { 
         GeoDataPolygonPrivate* copy = new  GeoDataPolygonPrivate;
        *copy = *this;
        return copy;
    };

    GeoDataLinearRing           outer;
    QVector<GeoDataLinearRing>  inner;
    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 
    TessellationFlags           m_tessellationFlags;
};

} // namespace Marble

#endif // GeoDataPolygonPRIVATE_H
