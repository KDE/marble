// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMRELATION
#define MARBLE_OSMRELATION

#include "OsmNode.h"
#include "OsmWay.h"
#include <GeoDataLinearRing.h>
#include <osm/OsmPlacemarkData.h>

#include <QSet>
#include <QString>
#include <QXmlStreamAttributes>

namespace Marble
{

class GeoDataDocument;

class OsmRelation
{
public:
    OsmPlacemarkData &osmData();
    void parseMember(const QXmlStreamAttributes &attributes);
    void addMember(qint64 reference, const QString &role, const QString &type);
    void createMultipolygon(GeoDataDocument *document, OsmWays &ways, const OsmNodes &nodes, QSet<qint64> &usedNodes, QSet<qint64> &usedWays) const;
    void createRelation(GeoDataDocument *document, const QHash<qint64, GeoDataPlacemark *> &wayPlacemarks) const;

    const OsmPlacemarkData &osmData() const;

private:
    typedef QPair<GeoDataLinearRing, OsmPlacemarkData> OsmRing;
    using OsmRings = QList<OsmRing>;

    struct OsmMember {
        QString type;
        QString role;
        qint64 reference;

        OsmMember();
    };

    OsmRings rings(const QStringList &roles, const OsmWays &ways, const OsmNodes &nodes, QSet<qint64> &usedNodes, QSet<qint64> &usedWays) const;

    OsmPlacemarkData m_osmData;
    QList<OsmMember> m_members;
};

typedef QHash<qint64, OsmRelation> OsmRelations;

}

#endif
