//
// This file is part of the Marble Virtual Globe.
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

#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTrack.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataModel.h"


namespace Marble
{

class GeoDataMultiGeometryPrivate : public GeoDataGeometryPrivate
{
  public:
    GeoDataMultiGeometryPrivate()
    {
    }

    ~GeoDataMultiGeometryPrivate() override
    {
        qDeleteAll(m_vector);
    }

    GeoDataMultiGeometryPrivate& operator=( const GeoDataMultiGeometryPrivate &other)
    {
        GeoDataGeometryPrivate::operator=( other );

        qDeleteAll( m_vector );
        m_vector.clear();

        m_vector.reserve(other.m_vector.size());

        for (const GeoDataGeometry *geometry: other.m_vector) {

            m_vector.append(geometry->copy());
        }
        return *this;
    }

    GeoDataGeometryPrivate *copy() const override
    { 
        GeoDataMultiGeometryPrivate* copy = new GeoDataMultiGeometryPrivate;
        *copy = *this;
        return copy;
    }

    QVector<GeoDataGeometry*>  m_vector;
};

} // namespace Marble

#endif
