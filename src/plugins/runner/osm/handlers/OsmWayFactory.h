//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_OSMWAYFACTORY_H
#define MARBLE_OSMWAYFACTORY_H

#include <QMap>

namespace Marble
{

class GeoDataLineString;

namespace osm
{

// This is a class for keeping all the nodes accesible
// for when needed by ways. Ways have only the ids of
// nodes so with that id the GeoDataPoint is returned

class OsmWayFactory
{
public:
    static void appendLine( quint64 id, GeoDataLineString *l );
    static GeoDataLineString *getLine( quint64 id );

    /**
     * @brief Clean up ways
     * Removes all ways from factory.
     * This function must be called only after file loaded.
     */
    static void cleanUp();

private:
    static QMap<quint64, GeoDataLineString *> m_lines;
};

}
}

#endif // MARBLE_OSMWAYFACTORY_H
