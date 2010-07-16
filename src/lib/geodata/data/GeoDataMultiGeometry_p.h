//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAMULTIGEOMETRYPRIVATE_H
#define MARBLE_GEODATAMULTIGEOMETRYPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataMultiGeometryPrivate : public GeoDataGeometryPrivate
{
  public:
     GeoDataMultiGeometryPrivate()
    {
    }

     ~GeoDataMultiGeometryPrivate()
     {
         qDeleteAll(m_vector);
     }

    virtual GeoDataGeometryPrivate* copy()
    { 
         GeoDataMultiGeometryPrivate* copy = new GeoDataMultiGeometryPrivate;
        *copy = *this;
        return copy;
    }

    virtual QString nodeType() const
    {
        return GeoDataTypes::GeoDataMultiGeometryType;
    }

    virtual EnumGeometryId geometryId() const
    {
        return GeoDataMultiGeometryId;
    }
    QVector<GeoDataGeometry*>  m_vector;
};

} // namespace Marble

#endif
