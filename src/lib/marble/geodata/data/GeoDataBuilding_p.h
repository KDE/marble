//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef MARBLE_GEODATABUILDING_P_H
#define MARBLE_GEODATABUILDING_P_H

#include "GeoDataGeometry_p.h"
#include "GeoDataMultiGeometry.h"

namespace Marble {

class GeoDataBuildingPrivate : public GeoDataGeometryPrivate
{
public:
    GeoDataBuildingPrivate()
        : m_height(0.0),
          m_minLevel(0),
          m_maxLevel(0)
    {
    }

    GeoDataGeometryPrivate *copy() const override
    {
        GeoDataBuildingPrivate* copy = new GeoDataBuildingPrivate;
        *copy = *this;
        return copy;
    }

    double m_height;
    int m_minLevel;
    int m_maxLevel;
    QVector<int> m_nonExistentLevels;
    GeoDataMultiGeometry m_multiGeometry;
};

}

#endif
