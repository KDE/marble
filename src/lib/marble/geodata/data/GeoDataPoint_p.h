// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
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

    GeoDataPointPrivate() = default;

    GeoDataGeometryPrivate *copy() const override
    {
        auto *copy = new GeoDataPointPrivate;
        *copy = *this;
        return copy;
    }
};

} // namespace Marble

#endif
