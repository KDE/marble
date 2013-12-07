//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataPlaylist.h"
#include "GeoDataSoundCue.h"

#include "GeoDataTypes.h"

namespace Marble
{

GeoDataPlaylist::GeoDataPlaylist()
{
}

GeoDataPlaylist::~GeoDataPlaylist()
{
}

const char *GeoDataPlaylist::nodeType() const
{
    return GeoDataTypes::GeoDataPlaylistType;
}

GeoDataTourPrimitive* GeoDataPlaylist::primitive(int id)
{
    if (size() <= id || id < 0) {
        return 0;
    }
    return m_primitives.at(id);
}

const GeoDataTourPrimitive* GeoDataPlaylist::primitive(int id) const
{
    if (size() <= id || id < 0) {
        return 0;
    }
    return m_primitives.at(id);
}

void GeoDataPlaylist::addPrimitive( GeoDataTourPrimitive *primitive, int position )
{
    primitive->setParent( this );
    if ( position == -1 ) {
        m_primitives.push_back( primitive );
    } else {
        m_primitives.insert( position, primitive );
    }
}

void GeoDataPlaylist::removePrimitive(int position)
{
    m_primitives.removeAt( position );
}

int GeoDataPlaylist::size() const
{
    return m_primitives.size();
}

void GeoDataPlaylist::moveUp(int position)
{
    if ( position > 0 && position <= m_primitives.size()-1 ) {
        m_primitives.insert( position-1, m_primitives.at( position ) );
        m_primitives.removeAt( position+1 );
    }
}

void GeoDataPlaylist::moveDown(int position)
{
    if ( position >= 0 && position < m_primitives.size()-1 ) {
        m_primitives.insert( position+2, m_primitives.at( position ) );
        m_primitives.removeAt( position );
    }
}

} // namespace Marble
