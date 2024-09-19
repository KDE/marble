// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmRelationTagWriter.h"

// Marble
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoWriter.h"
#include "OsmElementDictionary.h"
#include "OsmObjectAttributeWriter.h"
#include "OsmTagTagWriter.h"
#include "osm/OsmObjectManager.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

void OsmRelationTagWriter::writeMultipolygon(const GeoDataPolygon &polygon, const OsmPlacemarkData &osmData, GeoWriter &writer)
{
    writer.writeStartElement(QString::fromUtf8(osm::osmTag_relation));

    OsmObjectAttributeWriter::writeAttributes(osmData, writer);
    OsmTagTagWriter::writeTags(osmData, writer);

    writer.writeStartElement(QString::fromUtf8(osm::osmTag_member));
    QString memberId = QString::number(osmData.memberReference(-1).id());
    writer.writeAttribute("type", "way");
    writer.writeAttribute("ref", memberId);
    writer.writeAttribute("role", "outer");
    writer.writeEndElement();

    for (int index = 0; index < polygon.innerBoundaries().size(); ++index) {
        writer.writeStartElement(QString::fromUtf8(osm::osmTag_member));
        QString memberId = QString::number(osmData.memberReference(index).id());
        writer.writeAttribute("type", "way");
        writer.writeAttribute("ref", memberId);
        writer.writeAttribute("role", "inner");
        writer.writeEndElement();
    }
    writer.writeEndElement();
}

void OsmRelationTagWriter::writeRelation(const QVector<GeoDataPlacemark *> &members, const QString &id, const QMap<QString, QString> &tags, GeoWriter &writer)
{
    Q_UNUSED(members);
    Q_UNUSED(id);
    Q_UNUSED(tags);
    Q_UNUSED(writer);

    // Not implemented yet ( need to implement the custom relations first )
}

}
