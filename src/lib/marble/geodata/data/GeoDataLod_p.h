// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_GEODATALODPRIVATE_H
#define MARBLE_GEODATALODPRIVATE_H

#include "GeoDataLod.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataLodPrivate
{
public:
    GeoDataLodPrivate()
        : m_minLodPixels(0)
        , m_maxLodPixels(-1)
        , m_minFadeExtent(0)
        , m_maxFadeExtent(0)
    {
    }

    qreal m_minLodPixels;
    qreal m_maxLodPixels;
    qreal m_minFadeExtent;
    qreal m_maxFadeExtent;
};

} // namespace Marble

#endif
