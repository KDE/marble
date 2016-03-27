//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMWAY
#define MARBLE_OSMWAY

#include "OsmNode.h"
#include <osm/OsmPlacemarkData.h>
#include <GeoDataDocument.h>

#include <QString>
#include <QXmlStreamAttributes>

namespace Marble {

class OsmWay {
public:
    OsmPlacemarkData & osmData();
    void addReference(qint64 id);

    const OsmPlacemarkData & osmData() const;
    const QVector<qint64> &references() const;

    void create(GeoDataDocument* document, const OsmNodes &nodes) const;

private:
    bool isArea() const;

    OsmPlacemarkData m_osmData;
    QVector<qint64> m_references;
};
typedef QHash<qint64,OsmWay> OsmWays;

}

#endif
