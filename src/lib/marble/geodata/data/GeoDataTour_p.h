// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// SPDX-FileCopyrightText: 2014 Calin Cruceru <calin@rosedu.org>
//

#ifndef MARBLE_GEODATATOURPRIVATE_H
#define MARBLE_GEODATATOURPRIVATE_H

#include "GeoDataFeature_p.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataTourPrivate : public GeoDataFeaturePrivate
{
public:
    GeoDataTourPrivate()
        : m_playlist(nullptr)
    {
        // nothing to do
    }
    GeoDataTourPrivate(const GeoDataTourPrivate &other)
        : GeoDataFeaturePrivate(other)
        , m_playlist(nullptr)
    {
        if (other.m_playlist) {
            m_playlist = new GeoDataPlaylist(*other.m_playlist);
        }
    }

    ~GeoDataTourPrivate() override
    {
        delete m_playlist;
    }

    GeoDataTourPrivate &operator=(const GeoDataTourPrivate &other)
    {
        if (this == &other) {
            return *this;
        }

        GeoDataFeaturePrivate::operator=(other);

        GeoDataPlaylist *newPlaylist = nullptr;
        if (other.m_playlist) {
            newPlaylist = new GeoDataPlaylist(*other.m_playlist);
        }
        delete m_playlist;
        m_playlist = newPlaylist;

        return *this;
    }

    GeoDataPlaylist *m_playlist;
};

}

#endif // MARBLE_GEODATATOURPRIVATE_H
