// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMCONVERTER_H
#define MARBLE_OSMCONVERTER_H

#include <GeoDataCoordinates.h>

namespace Marble
{

class GeoDataLineString;
class GeoDataDocument;
class GeoDataLinearRing;
class GeoDataPolygon;
class GeoDataPlacemark;
class GeoDataFeature;
class OsmPlacemarkData;

class OsmConverter
{
public:
    typedef QPair<QString, QString> Tag;
    typedef QPair<GeoDataCoordinates, OsmPlacemarkData> Node;
    typedef QPair<const GeoDataLineString *, OsmPlacemarkData> Way;
    typedef QPair<const GeoDataFeature *, OsmPlacemarkData> Relation;

    using Nodes = QVector<Node>;
    using Tags = QVector<Tag>;
    using Ways = QVector<Way>;
    using Relations = QVector<Relation>;

    void read(const GeoDataDocument *document);

    const Nodes &nodes() const;
    const Ways &ways() const;
    const Relations &relations() const;

private:
    Nodes m_nodes;
    Ways m_ways;
    Relations m_relations;

    void processLinearRing(GeoDataLinearRing *linearRing, const OsmPlacemarkData &osmData);
    void processPolygon(GeoDataPolygon *polygon, const OsmPlacemarkData &osmData, GeoDataPlacemark *placemark);
};

}

#endif
