//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATALINESTRINGPRIVATE_H
#define GEODATALINESTRINGPRIVATE_H

#include "GeoDataGeometry_p.h"

namespace Marble
{

class GeoDataLineStringPrivate : public Marble::GeoDataGeometryPrivate
{
  public:
    GeoDataLineStringPrivate( TessellationFlags f )
         : m_poleCorrected( 0 ),
           m_dirtyBox( true ),
           m_tessellationFlags( f )
    {
    }

    GeoDataLineStringPrivate()
         : m_poleCorrected( 0 ),
           m_dirtyBox( true )
    {
    }

    virtual GeoDataGeometryPrivate* copy()
    { 
        GeoDataLineStringPrivate* copy = new GeoDataLineStringPrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumGeometryId geometryId() const 
    {
        return GeoDataLineStringId;
    }

    GeoDataLineString *         m_poleCorrected;
    GeoDataLatLonAltBox         m_latLonAltBox;

    QVector<GeoDataCoordinates> m_vector;
    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 
    TessellationFlags           m_tessellationFlags;
};

} // namespace Marble

#endif //GEODATALINESTRINGPRIVATE_H
