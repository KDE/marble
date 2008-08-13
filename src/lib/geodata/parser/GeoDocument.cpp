/*
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "GeoDocument.h"

#if DUMP_GEONODE_LEAKS > 0
#include <cstdio>
#endif

namespace Marble
{

#if DUMP_GEONODE_LEAKS > 0
unsigned long GeoDocument::s_leakProtector = 0;
#endif

GeoDocument::GeoDocument()
{
}

GeoDocument::~GeoDocument()
{
#if DUMP_GEONODE_LEAKS > 0
    if ( s_leakProtector != 0 ) {
        std::fprintf( stderr, "Found %lu GeoNode object LEAKS!\n", s_leakProtector );
        s_leakProtector = 0;
    }
#endif
}

bool GeoDocument::isGeoDataDocument() const
{
    return false;
}

bool GeoDocument::isGeoSceneDocument() const
{
    return false;
}


GeoNode::GeoNode()
{
#if DUMP_GEONODE_LEAKS > 0
    GeoDocument::s_leakProtector++;

#if DUMP_GEONODE_LEAKS > 1
    fprintf( stderr, "Constructed new GeoNode object, leak protection count: %lu\n",
             GeoDocument::s_leakProtector );
#endif
#endif
}

GeoNode::~GeoNode()
{
#if DUMP_GEONODE_LEAKS > 0
    --GeoDocument::s_leakProtector;

#if DUMP_GEONODE_LEAKS > 1
    fprintf( stderr, "Destructed GeoNode object, leak protection count: %lu\n",
             GeoDocument::s_leakProtector );
#endif
#endif
}

}
