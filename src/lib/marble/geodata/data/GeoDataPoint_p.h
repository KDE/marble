//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAPOINTPRIVATE_H
#define MARBLE_GEODATAPOINTPRIVATE_H

#include "GeoDataGeometry_p.h"

namespace Marble
{

class GeoDataPointPrivate : public GeoDataGeometryPrivate
{
public:
    GeoDataCoordinates m_coordinates;

    GeoDataPointPrivate()
    {
    }

    GeoDataGeometryPrivate *copy() const override
    { 
        GeoDataPointPrivate* copy = new  GeoDataPointPrivate;
        *copy = *this;
        return copy;
    }
};

} // namespace Marble

#endif
