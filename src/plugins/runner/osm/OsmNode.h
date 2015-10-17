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
#include <GeoDataDocument.h>

#include <QString>
#include <QXmlStreamAttributes>

namespace Marble {

class OsmNode {
public:
    OsmPlacemarkData & osmData();
    void parseCoordinates(const QXmlStreamAttributes &attributes);

    GeoDataCoordinates coordinates() const;
    const OsmPlacemarkData & osmData() const;

    void create(GeoDataDocument* document) const;

private:
    int populationIndex(qint64 population) const;

    OsmPlacemarkData m_osmData;
    GeoDataCoordinates m_coordinates;
};

typedef QMap<qint64,OsmNode> OsmNodes;

}

#endif
