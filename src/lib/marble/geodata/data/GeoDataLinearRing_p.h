//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATALINEARRINGPRIVATE_H
#define MARBLE_GEODATALINEARRINGPRIVATE_H

#include "GeoDataLineString_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataLinearRingPrivate : public GeoDataLineStringPrivate
{
  public:
    explicit GeoDataLinearRingPrivate( TessellationFlags f )
     : GeoDataLineStringPrivate( f )
    {
    }

    GeoDataLinearRingPrivate()
    {
    }

    GeoDataGeometryPrivate* copy() override
    { 
        GeoDataLinearRingPrivate* copy = new GeoDataLinearRingPrivate;
        *copy = *this;
        return copy;
    }

    const char* nodeType() const override
    {
        return GeoDataTypes::GeoDataLinearRingType;
    }

    EnumGeometryId geometryId() const override
    {
        return GeoDataLinearRingId;
    }
};

} // namespace Marble

#endif
