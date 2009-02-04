//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATAMULTIGEOMETRYPRIVATE_H
#define GEODATAMULTIGEOMETRYPRIVATE_H

#include "GeoDataGeometry_p.h"

namespace Marble
{

class  GeoDataMultiGeometryPrivate : public Marble::GeoDataGeometryPrivate
{
  public:
     GeoDataMultiGeometryPrivate()
    {
    }

    virtual void* copy() 
    { 
         GeoDataMultiGeometryPrivate* copy = new  GeoDataMultiGeometryPrivate;
        *copy = *this;
        return copy;
    };

    QVector<GeoDataGeometry>  m_vector;
};

} // namespace Marble

#endif // GeoDataMultiGeometryPRIVATE_H
