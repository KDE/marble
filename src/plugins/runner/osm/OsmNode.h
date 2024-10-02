// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMNODE
#define MARBLE_OSMNODE

#include <GeoDataPlacemark.h>
#include <osm/OsmPlacemarkData.h>

#include <QString>

class QXmlStreamAttributes;

namespace Marble
{

class GeoDataDocument;

class OsmNode
{
public:
    OsmPlacemarkData &osmData();
    void parseCoordinates(const QXmlStreamAttributes &attributes);
    void setCoordinates(const GeoDataCoordinates &coordinates);

    const GeoDataCoordinates &coordinates() const;
    const OsmPlacemarkData &osmData() const;

    GeoDataPlacemark *create() const;

private:
    int populationIndex(qint64 population) const;

    OsmPlacemarkData m_osmData;
    GeoDataCoordinates m_coordinates;
};

using OsmNodes = QHash<qint64, OsmNode>;
}

#endif
