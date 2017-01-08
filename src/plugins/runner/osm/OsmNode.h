//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMNODE
#define MARBLE_OSMNODE

#include <osm/OsmPlacemarkData.h>
#include <GeoDataPlacemark.h>

#include <QString>

class QXmlStreamAttributes;

namespace Marble {

class GeoDataDocument;

class OsmNode {
public:
    OsmPlacemarkData & osmData();
    void parseCoordinates(const QXmlStreamAttributes &attributes);
    void setCoordinates(const GeoDataCoordinates &coordinates);

    const GeoDataCoordinates & coordinates() const;
    const OsmPlacemarkData & osmData() const;

    GeoDataPlacemark* create() const;

private:
    int populationIndex(qint64 population) const;

    OsmPlacemarkData m_osmData;
    GeoDataCoordinates m_coordinates;
};

typedef QHash<qint64,OsmNode> OsmNodes;

}

#endif
