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

void GeoDataPlaylist::addPrimitive(GeoDataTourPrimitive *primitive)
{
    m_primitives.push_back(primitive);
}

int GeoDataPlaylist::size() const
{
    return m_primitives.size();
}

} // namespace Marble
