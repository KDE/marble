//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMCONVERTER_H
#define MARBLE_OSMCONVERTER_H

#include <GeoDataCoordinates.h>

namespace Marble
{

class GeoDataLineString;
class GeoDataDocument;
class GeoDataPolygon;
class OsmPlacemarkData;

class OsmConverter
{
public:
    typedef QPair<QString, QString> Tag;
    typedef QPair<GeoDataCoordinates, OsmPlacemarkData > Node;
    typedef QPair<const GeoDataLineString*, OsmPlacemarkData > Way;
    typedef QPair<const GeoDataPolygon*, OsmPlacemarkData > Polygon;

    typedef QVector<Node> Nodes;
    typedef QVector<Tag> Tags;
    typedef QVector<Way> Ways;
    typedef QVector<Polygon> Polygons;

    void read(const GeoDataDocument* document);

    const Nodes & nodes() const;
    const Ways & ways() const;
    const Polygons & polygons() const;

private:
    Nodes m_nodes;
    Ways m_ways;
    Polygons m_polygons;
};

}

#endif

