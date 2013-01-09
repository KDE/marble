/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "OsmWayFactory.h"
#include "GeoDataLineString.h"

namespace Marble
{
namespace osm
{
QMap<quint64, GeoDataLineString *> OsmWayFactory::m_lines;

// This is a class for keeping all the ways accessible
// for when needed by relations. Relations have only the ids of
// ways so with that id the GeoDataLineString is returned

void OsmWayFactory::appendLine( quint64 id, GeoDataLineString* l )
{
    m_lines[id] = l;
}

GeoDataLineString* OsmWayFactory::line( quint64 id )
{
    return m_lines.value( id );
}

void OsmWayFactory::clear()
{
    //foreach( GeoDataLineString *l, m_lines )
    //{
    //delete l;
    //}
    m_lines.clear();
}

}
}
