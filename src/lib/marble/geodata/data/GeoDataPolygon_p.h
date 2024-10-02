// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAPOLYGONPRIVATE_H
#define MARBLE_GEODATAPOLYGONPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataLinearRing.h"

namespace Marble
{

class GeoDataPolygonPrivate : public GeoDataGeometryPrivate
{
public:
    explicit GeoDataPolygonPrivate(TessellationFlags f)
        : m_dirtyBox(true)
        , m_tessellationFlags(f)
        , m_renderOrder(0)
    {
    }

    GeoDataPolygonPrivate()
        : m_dirtyBox(true)
    {
    }

    GeoDataGeometryPrivate *copy() const override
    {
        auto *copy = new GeoDataPolygonPrivate;
        *copy = *this;
        return copy;
    }

    GeoDataLinearRing outer;
    QList<GeoDataLinearRing> inner;
    bool m_dirtyBox; // tells whether there have been changes to the
                     // GeoDataPoints since the LatLonAltBox has
                     // been calculated. Saves performance.
    TessellationFlags m_tessellationFlags;
    int m_renderOrder;
};

} // namespace Marble

#endif
