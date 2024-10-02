// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_GEODATAMULTITRACKPRIVATE_H
#define MARBLE_GEODATAMULTITRACKPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataTrack.h"

namespace Marble
{

class GeoDataMultiTrackPrivate : public GeoDataGeometryPrivate
{
public:
    GeoDataMultiTrackPrivate() = default;

    ~GeoDataMultiTrackPrivate() override
    {
        qDeleteAll(m_vector);
    }

    GeoDataMultiTrackPrivate &operator=(const GeoDataMultiTrackPrivate &other)
    {
        GeoDataGeometryPrivate::operator=(other);

        qDeleteAll(m_vector);
        m_vector.clear();

        m_vector.reserve(other.m_vector.size());
        for (GeoDataTrack *track : other.m_vector) {
            m_vector.append(new GeoDataTrack(*track));
        }
        return *this;
    }

    GeoDataGeometryPrivate *copy() const override
    {
        auto *copy = new GeoDataMultiTrackPrivate;
        *copy = *this;
        return copy;
    }

    QList<GeoDataTrack *> m_vector;
};

} // namespace Marble

#endif
