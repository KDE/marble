// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#include "KmlOsmPlacemarkDataTagWriter.h"

#include "GeoDataFeature.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataSchemaData.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

bool KmlOsmPlacemarkDataTagWriter::write(const GeoDataFeature *feature, GeoWriter &writer)
{
    const auto placemark = geodata_cast<GeoDataPlacemark>(feature);

    if (!placemark) {
        return false;
    }

    const OsmPlacemarkData &osmData = placemark->osmData();
    if (osmData.isNull()) {
        return true;
    }

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_ExtendedData));

    // We declare the "mx" namespace for the custom osmPlacemarkData XML schema
    writer.writeNamespace(QString::fromUtf8(kml::kmlTag_nameSpaceMx), "mx");

    KmlOsmPlacemarkDataTagWriter::writeOsmData(placemark->geometry(), osmData, writer);

    writer.writeEndElement();
    return true;
}

bool KmlOsmPlacemarkDataTagWriter::writeOsmData(const GeoDataGeometry *geometry, const OsmPlacemarkData &osmData, GeoWriter &writer)
{
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceMx), QString::fromUtf8(kml::kmlTag_OsmPlacemarkData));

    // Writing the attributes
    writer.writeAttribute("id", QString::number(osmData.id()));
    writer.writeOptionalAttribute(QStringLiteral("changeset"), osmData.changeset());
    writer.writeOptionalAttribute(QStringLiteral("timestamp"), osmData.timestamp());
    writer.writeOptionalAttribute(QStringLiteral("uid"), osmData.uid());
    writer.writeOptionalAttribute(QStringLiteral("user"), osmData.user());
    writer.writeOptionalAttribute(QStringLiteral("version"), osmData.version());
    writer.writeOptionalAttribute(QStringLiteral("visible"), osmData.isVisible());
    writer.writeOptionalAttribute(QStringLiteral("action"), osmData.action());

    // Writing the tags
    QHash<QString, QString>::const_iterator tagsIt = osmData.tagsBegin();
    QHash<QString, QString>::const_iterator tagsEnd = osmData.tagsEnd();
    for (; tagsIt != tagsEnd; ++tagsIt) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceMx), "tag");
        writer.writeAttribute("k", tagsIt.key());
        writer.writeAttribute("v", tagsIt.value());
        writer.writeEndElement();
    }
    if (geometry) {
        // Ways
        if (const auto lineString = dynamic_cast<const GeoDataLineString *>(geometry)) {
            int ndIndex = 0;

            // Writing the component nodes
            QList<GeoDataCoordinates>::const_iterator nodeIt = lineString->begin();
            QList<GeoDataCoordinates>::const_iterator nodeEnd = lineString->end();
            for (; nodeIt != nodeEnd; ++nodeIt) {
                const OsmPlacemarkData &nodeOsmData = osmData.nodeReference(*nodeIt);
                writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceMx), "nd");
                writer.writeAttribute("index", QString::number(ndIndex++));
                writeOsmData(nullptr, nodeOsmData, writer);
                writer.writeEndElement();
            }
        }
        // Polygons
        else if (const auto polygon = geodata_cast<GeoDataPolygon>(geometry)) {
            int memberIndex = -1;

            // Writing the outerBoundary osmData
            const GeoDataLinearRing &outerRing = polygon->outerBoundary();
            const OsmPlacemarkData &outerRingOsmData = osmData.memberReference(memberIndex);
            writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceMx), QString::fromUtf8(kml::kmlTag_member));
            writer.writeAttribute("index", QString::number(memberIndex));
            writeOsmData(&outerRing, outerRingOsmData, writer);
            writer.writeEndElement();

            // Writing the innerBoundaries
            for (const GeoDataLinearRing &innerRing : polygon->innerBoundaries()) {
                const OsmPlacemarkData &innerRingOsmData = osmData.memberReference(++memberIndex);
                writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceMx), QString::fromUtf8(kml::kmlTag_member));
                writer.writeAttribute("index", QString::number(memberIndex));
                writeOsmData(&innerRing, innerRingOsmData, writer);
                writer.writeEndElement();
            }
        }
    }
    writer.writeEndElement();
    return true;
}

}
