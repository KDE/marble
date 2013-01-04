//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_OSMNODEFACTORY_H
#define MARBLE_OSMNODEFACTORY_H

#include <QMap>

namespace Marble
{

class GeoDataPoint;

namespace osm
{

// This is a class for keeping all the nodes accessible
// for when needed by ways. Ways have only the ids of
// nodes so with that id the GeoDataPoint is returned

class OsmNodeFactory
{
public:
    static void appendPoint( quint64 id, GeoDataPoint *p );
    static GeoDataPoint *getPoint( quint64 id );

    /**
     * @brief Clean up nodes
     * Removes all nodes from factory.
     * This function must be called only after file loaded.
     */
    static void clear();

private:
    static QMap<quint64, GeoDataPoint *> m_points;
};

}
}

#endif // MARBLE_OSMNODEFACTORY_H
