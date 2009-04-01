//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATALINEARRINGPRIVATE_H
#define GEODATALINEARRINGPRIVATE_H

#include "GeoDataLineString_p.h"

namespace Marble
{

class GeoDataLinearRingPrivate : public Marble::GeoDataLineStringPrivate
{
  public:
    GeoDataLinearRingPrivate( TessellationFlags f )
     : GeoDataLineStringPrivate( f )
    {
    }

    GeoDataLinearRingPrivate()
    {
    }

    virtual void* copy() 
    { 
        GeoDataLinearRingPrivate* copy = new GeoDataLinearRingPrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumGeometryId geometryId() const 
    {
        return GeoDataLinearRingId;
    }
};

} // namespace Marble

#endif //GeoDataLinearRingPRIVATE_H
