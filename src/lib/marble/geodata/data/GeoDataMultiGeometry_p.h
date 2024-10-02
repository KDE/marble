// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAMULTIGEOMETRYPRIVATE_H
#define MARBLE_GEODATAMULTIGEOMETRYPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataModel.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTrack.h"

namespace Marble
{

class GeoDataMultiGeometryPrivate : public GeoDataGeometryPrivate
{
public:
    GeoDataMultiGeometryPrivate() = default;

    ~GeoDataMultiGeometryPrivate() override
    {
        qDeleteAll(m_vector);
    }

    GeoDataMultiGeometryPrivate &operator=(const GeoDataMultiGeometryPrivate &other)
    {
        GeoDataGeometryPrivate::operator=(other);

        qDeleteAll(m_vector);
        m_vector.clear();

        m_vector.reserve(other.m_vector.size());

        for (const GeoDataGeometry *geometry : other.m_vector) {
            m_vector.append(geometry->copy());
        }
        return *this;
    }

    GeoDataGeometryPrivate *copy() const override
    {
        auto *copy = new GeoDataMultiGeometryPrivate;
        *copy = *this;
        return copy;
    }

    QList<GeoDataGeometry *> m_vector;
};

} // namespace Marble

#endif
