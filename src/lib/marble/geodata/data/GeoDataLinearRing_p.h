// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATALINEARRINGPRIVATE_H
#define MARBLE_GEODATALINEARRINGPRIVATE_H

#include "GeoDataLineString_p.h"

namespace Marble
{

class GeoDataLinearRingPrivate : public GeoDataLineStringPrivate
{
public:
    explicit GeoDataLinearRingPrivate(TessellationFlags f)
        : GeoDataLineStringPrivate(f)
    {
    }

    GeoDataLinearRingPrivate() = default;

    GeoDataGeometryPrivate *copy() const override
    {
        auto *copy = new GeoDataLinearRingPrivate;
        *copy = *this;
        return copy;
    }
};

} // namespace Marble

#endif
