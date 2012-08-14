/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "OsmRelationFactory.h"
#include "GeoDataPolygon.h"

namespace Marble
{
namespace osm
{
QMap<quint64, GeoDataPolygon *> OsmRelationFactory::m_polygons;

// This is a class for keeping all the relations accesible
// for when needed by other relations. As OSM detail level
// increases its getting more common to have relations as
// members of other relations

void OsmRelationFactory::appendPolygon( quint64 id, GeoDataPolygon* p )
{
    m_polygons[id] = p;
}

GeoDataPolygon* OsmRelationFactory::polygon( quint64 id )
{
    return m_polygons.value( id );
}

void OsmRelationFactory::clear()
{
    m_polygons.clear();
}

}
}

