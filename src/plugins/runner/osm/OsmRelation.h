//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMRELATION
#define MARBLE_OSMRELATION

#include "OsmNode.h"
#include "OsmWay.h"
#include <osm/OsmPlacemarkData.h>
#include <GeoDataLinearRing.h>
#include <GeoDataDocument.h>

#include <QString>
#include <QXmlStreamAttributes>

namespace Marble {

class OsmRelation
{
public:
    OsmPlacemarkData & osmData();
    void parseMember(const QXmlStreamAttributes &attributes);

    const OsmPlacemarkData & osmData() const;

    void create(GeoDataDocument* document, const OsmWays &ways, const OsmNodes &nodes) const;

private:
    struct OsmMember
    {
        QString type;
        QString role;
        qint64 reference;

        OsmMember();
    };

    QList<GeoDataLinearRing> rings(const QStringList &roles, const OsmWays &ways, const OsmNodes &nodes) const;

    OsmPlacemarkData m_osmData;
    QVector<OsmMember> m_members;
};

typedef QMap<qint64,OsmRelation> OsmRelations;

}

#endif
