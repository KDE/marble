// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAGEOMETRYPRIVATE_H
#define MARBLE_GEODATAGEOMETRYPRIVATE_H

#include <QAtomicInt>

#include "GeoDataGeometry.h"

#include "GeoDataLatLonAltBox.h"

namespace Marble
{

class GeoDataGeometryPrivate
{
public:
    GeoDataGeometryPrivate()
        : m_extrude(false)
        , m_altitudeMode(ClampToGround)
        , ref(0)
    {
    }

    GeoDataGeometryPrivate(const GeoDataGeometryPrivate &other)
        : m_extrude(other.m_extrude)
        , m_altitudeMode(other.m_altitudeMode)
        , m_latLonAltBox()
        , ref(0)
    {
    }

    virtual ~GeoDataGeometryPrivate() = default;

    GeoDataGeometryPrivate &operator=(const GeoDataGeometryPrivate &other)
    {
        m_extrude = other.m_extrude;
        m_altitudeMode = other.m_altitudeMode;
        m_latLonAltBox = other.m_latLonAltBox;
        return *this;
    }

    virtual GeoDataGeometryPrivate *copy() const = 0;

    bool m_extrude;
    AltitudeMode m_altitudeMode;
    mutable GeoDataLatLonAltBox m_latLonAltBox;

    QAtomicInt ref;
};

} // namespace Marble

#endif
