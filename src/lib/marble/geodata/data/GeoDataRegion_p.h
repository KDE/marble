// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_GEODATAREGIONPRIVATE_H
#define MARBLE_GEODATAREGIONPRIVATE_H

#include "GeoDataRegion.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataLod.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataRegionPrivate
{
public:
    GeoDataRegionPrivate()
        : m_parent(nullptr)
        , m_latLonAltBox(nullptr)
        , m_lod(nullptr)
    {
    }

    GeoDataRegionPrivate(const GeoDataRegionPrivate &other)
        : m_parent(other.m_parent)
    {
        if (other.m_latLonAltBox) {
            m_latLonAltBox = new GeoDataLatLonAltBox(*other.m_latLonAltBox);
        } else {
            m_latLonAltBox = nullptr;
        }

        if (other.m_lod) {
            m_lod = new GeoDataLod(*other.m_lod);
        } else {
            m_lod = nullptr;
        }
    }

    explicit GeoDataRegionPrivate(GeoDataFeature *feature)
        : m_parent(feature)
        , m_latLonAltBox(nullptr)
        , m_lod(nullptr)
    {
    }

    ~GeoDataRegionPrivate()
    {
        delete m_latLonAltBox;
        delete m_lod;
    }

    GeoDataFeature *m_parent;
    GeoDataLatLonAltBox *m_latLonAltBox;
    GeoDataLod *m_lod;

private:
    // Preventing usage of operator=
    GeoDataRegionPrivate &operator=(const GeoDataRegionPrivate &) = delete;
};

} // namespace Marble

#endif
