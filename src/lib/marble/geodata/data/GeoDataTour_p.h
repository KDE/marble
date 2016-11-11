//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// Copyright 2014 Calin Cruceru <calin@rosedu.org>
//

#ifndef MARBLE_GEODATATOURPRIVATE_H
#define MARBLE_GEODATATOURPRIVATE_H

#include "GeoDataFeature_p.h"
#include "GeoDataTypes.h"
#include "GeoDataPlaylist.h"


namespace Marble
{

class GeoDataTourPrivate : public GeoDataFeaturePrivate
{
public:
    GeoDataTourPrivate()
        : m_playlist( 0 )
    {
        // nothing to do
    }
    GeoDataTourPrivate(const GeoDataTourPrivate &other)
      : GeoDataFeaturePrivate(other),
        m_playlist(nullptr)
    {
        if (other.m_playlist) {
            m_playlist = new GeoDataPlaylist(*other.m_playlist);
        }
    }

    virtual ~GeoDataTourPrivate()
    {
        delete m_playlist;
    }

    GeoDataTourPrivate& operator=( const GeoDataTourPrivate &other )
    {
        if ( this == &other ) {
            return *this;
        }

        GeoDataFeaturePrivate::operator=( other );

        GeoDataPlaylist *newPlaylist = 0;
        if ( other.m_playlist ) {
            newPlaylist = new GeoDataPlaylist( *other.m_playlist );
        }
        delete m_playlist;
        m_playlist = newPlaylist;

        return *this;
    }

    GeoDataPlaylist *m_playlist;
};

}

#endif // MARBLE_GEODATATOURPRIVATE_H
