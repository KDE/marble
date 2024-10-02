// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef MARBLE_GEODATABUILDING_P_H
#define MARBLE_GEODATABUILDING_P_H

#include "GeoDataGeometry_p.h"
#include "GeoDataMultiGeometry.h"

namespace Marble
{

class GeoDataBuildingPrivate : public GeoDataGeometryPrivate
{
public:
    GeoDataBuildingPrivate()
        : m_height(0.0)
        , m_minLevel(0)
        , m_maxLevel(0)
    {
    }

    GeoDataGeometryPrivate *copy() const override
    {
        auto *copy = new GeoDataBuildingPrivate;
        *copy = *this;
        return copy;
    }

    double m_height;
    int m_minLevel;
    int m_maxLevel;
    QList<int> m_nonExistentLevels;
    GeoDataMultiGeometry m_multiGeometry;
    QString m_name;
    QList<GeoDataBuilding::NamedEntry> m_entries;
};

}

#endif
