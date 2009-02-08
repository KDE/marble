//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATAPOINTPRIVATE_H
#define GEODATAPOINTPRIVATE_H

#include "GeoDataGeometry_p.h"
#include "GeoDataCoordinates_p.h"

namespace Marble
{

class  GeoDataPointPrivate : public Marble::GeoDataGeometryPrivate,
                             public Marble::GeoDataCoordinatesPrivate
{
  public:
     GeoDataPointPrivate()
    {
    }

    virtual void* copy() 
    { 
         GeoDataPointPrivate* copy = new  GeoDataPointPrivate;
        *copy = *this;
        return copy;
    };
};

} // namespace Marble

#endif // GeoDataPointPRIVATE_H
