//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_OSMRELATIONFACTORY_H
#define MARBLE_OSMRELATIONFACTORY_H

#include <QMap>

namespace Marble
{

class GeoDataPolygon;

namespace osm
{

// This is a class for keeping all the relations accesible
// for when needed by other relations. As OSM detail level
// increases its getting more common to have relations as
// members of other relations

class OsmRelationFactory
{
public:
    static void appendPolygon( quint64 id, GeoDataPolygon *p );
    static GeoDataPolygon *getPolygon( quint64 id );

    /**
     * @brief Clean up relations
     * Removes all relations from factory.
     * This function must be called only after file loaded.
     */
    static void cleanUp();

private:
    static QMap<quint64, GeoDataPolygon *> m_polygons;
};

}
}

#endif // MARBLE_OSMRELATIONFACTORY_H
