// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMRELATIONTAGWRITER_H
#define MARBLE_OSMRELATIONTAGWRITER_H

#include <QMap>

namespace Marble
{

class GeoDataPolygon;
class GeoWriter;
class GeoDataPlacemark;
class OsmPlacemarkData;

class OsmRelationTagWriter
{
public:
    static void writeRelation(const QList<GeoDataPlacemark *> &relation, const QString &id, const QMap<QString, QString> &tags, GeoWriter &writer);

    static void writeMultipolygon(const GeoDataPolygon &polygon, const OsmPlacemarkData &osmData, GeoWriter &writer);
};

}

#endif
