//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmNodeFactory.h"
#include "GeoDataPoint.h"

namespace Marble
{
namespace osm
{
QMap<quint64, GeoDataPoint *> OsmNodeFactory::m_points;

// This is a class for keeping all the nodes accesible
// for when needed by ways. Ways have only the ids of
// nodes so with that id the GeoDataPoint is returned

void OsmNodeFactory::appendPoint( quint64 id, GeoDataPoint* p )
{
    m_points[id] = p;
}

GeoDataPoint* OsmNodeFactory::getPoint( quint64 id )
{
    return m_points.value( id );
}

void OsmNodeFactory::cleanUp()
{
    foreach( GeoDataPoint *p, m_points )
    {
        delete p;
    }
    m_points.clear();
}

}
}
