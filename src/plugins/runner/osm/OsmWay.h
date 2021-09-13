// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMWAY
#define MARBLE_OSMWAY

#include "OsmNode.h"
#include "GeoDataBuilding.h"
#include <osm/OsmPlacemarkData.h>
#include <StyleBuilder.h>

#include <QSet>
#include <QString>

namespace Marble {

class GeoDataDocument;

class OsmWay
{
public:
    OsmPlacemarkData & osmData();
    void addReference(qint64 id);

    const OsmPlacemarkData & osmData() const;
    const QVector<qint64> &references() const;

    GeoDataPlacemark* create(const OsmNodes &nodes, QSet<qint64> &usedNodes) const;

private:
    bool isArea() const;

    static bool isAreaTag(const StyleBuilder::OsmTag &keyValue);

    bool isBuilding() const;

    static bool isBuildingTag(const StyleBuilder::OsmTag &keyValue);

    OsmPlacemarkData m_osmData;
    QVector<qint64> m_references;

    static QSet<StyleBuilder::OsmTag> s_areaTags;
    static QSet<StyleBuilder::OsmTag> s_buildingTags;

    QString extractBuildingName() const;
    double extractBuildingHeight() const;
    QVector<GeoDataBuilding::NamedEntry> extractNamedEntries() const;
};

typedef QHash<qint64,OsmWay> OsmWays;

}

#endif
