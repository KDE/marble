// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmWayTagWriter.h"

// Marble
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoWriter.h"
#include "OsmElementDictionary.h"
#include "OsmObjectAttributeWriter.h"
#include "OsmTagTagWriter.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

void OsmWayTagWriter::writeWay(const GeoDataLineString &lineString, const OsmPlacemarkData &osmData, GeoWriter &writer)
{
    writer.writeStartElement(QString::fromUtf8(osm::osmTag_way));

    OsmObjectAttributeWriter::writeAttributes(osmData, writer);
    OsmTagTagWriter::writeTags(osmData, writer);

    // Writing all the component nodes ( Nd tags )
    QList<GeoDataCoordinates>::const_iterator it = lineString.constBegin();
    QList<GeoDataCoordinates>::ConstIterator const end = lineString.constEnd();

    for (; it != end; ++it) {
        QString ndId = QString::number(osmData.nodeReference(*it).id());
        writer.writeStartElement(QString::fromUtf8(osm::osmTag_nd));
        writer.writeAttribute("ref", ndId);
        writer.writeEndElement();
    }

    if (!lineString.isEmpty() && lineString.isClosed()) {
        auto const startId = osmData.nodeReference(lineString.first()).id();
        auto const endId = osmData.nodeReference(lineString.last()).id();
        if (startId != endId) {
            writer.writeStartElement(QString::fromUtf8(osm::osmTag_nd));
            writer.writeAttribute("ref", QString::number(startId));
            writer.writeEndElement();
        }
    }

    writer.writeEndElement();
}

}
