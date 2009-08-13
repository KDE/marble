//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Torsten Rahn <rahn@kde.org>
//

#ifndef GEODATAREGIONPRIVATE_H
#define GEODATAREGIONPRIVATE_H

#include "GeoDataRegion.h"

namespace Marble
{

class GeoDataRegionPrivate
{
  public:
    GeoDataRegionPrivate()
         : m_parent( 0 ),
           m_latLonAltBox( 0 ),
           m_lod( 0 )
    {
    }


    explicit GeoDataRegionPrivate( GeoDataFeature * feature )
         : m_parent( feature ),
           m_latLonAltBox( 0 ),
           m_lod( 0 )
    {
    }


    QString nodeType() const
    {
        return GeoDataTypes::GeoDataRegionType;
    }

    GeoDataFeature * m_parent;
    GeoDataLatLonAltBox * m_latLonAltBox;
    GeoDataLod * m_lod;
};

} // namespace Marble

#endif //GEODATAREGIONPRIVATE_H
