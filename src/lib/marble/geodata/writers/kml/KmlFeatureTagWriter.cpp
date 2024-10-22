// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlFeatureTagWriter.h"

#include "GeoDataCamera.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLookAt.h"
#include "GeoDataPlacemark.h"
#include "GeoDataRegion.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTimeStamp.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"
#include "KmlOsmPlacemarkDataTagWriter.h"
#include "OsmPlacemarkData.h"

#include <QDateTime>

namespace Marble
{

KmlFeatureTagWriter::KmlFeatureTagWriter(const QString &elementName)
    : m_elementName(elementName)
{
    // nothing to do
}

bool KmlFeatureTagWriter::write(const Marble::GeoNode *node, GeoWriter &writer) const
{
    const auto feature = static_cast<const GeoDataFeature *>(node);

    if (const auto document = geodata_cast<GeoDataDocument>(feature)) {
        // when a document has only one feature and no styling
        // the document tag is excused
        if ((document->id().isEmpty()) && (document->name().isEmpty()) && (document->targetId().isEmpty()) && (document->styles().isEmpty())
            && (document->styleMaps().isEmpty()) && (document->extendedData().isEmpty()) && (document->featureList().count() == 1)) {
            writeElement(document->featureList().constFirst(), writer);
            return true;
        }
    }

    writer.writeStartElement(m_elementName);

    KmlObjectTagWriter::writeIdentifiers(writer, feature);

    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_name), feature->name());
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_visibility), QString::number(feature->isVisible()), QStringLiteral("1"));
    writer.writeOptionalElement(QStringLiteral("address"), feature->address());

    if (!feature->description().isEmpty()) {
        writer.writeStartElement("description");
        if (feature->descriptionIsCDATA()) {
            writer.writeCDATA(feature->description());
        } else {
            writer.writeCharacters(feature->description());
        }
        writer.writeEndElement();
    }

    auto const *lookAt = dynamic_cast<const GeoDataLookAt *>(feature->abstractView());
    if (lookAt) {
        writeElement(lookAt, writer);
    }
    auto const *camera = dynamic_cast<const GeoDataCamera *>(feature->abstractView());
    if (camera) {
        writeElement(camera, writer);
    }

    if (feature->timeStamp().when().isValid()) {
        writeElement(&feature->timeStamp(), writer);
    }

    if (feature->timeSpan().isValid()) {
        writeElement(&feature->timeSpan(), writer);
    }

    if (!feature->region().latLonAltBox().isNull()) {
        writeElement(&feature->region(), writer);
    }

    bool const result = writeMid(node, writer);

    if (geodata_cast<GeoDataPlacemark>(feature)) {
        KmlOsmPlacemarkDataTagWriter::write(feature, writer);
    }

    if (!feature->extendedData().isEmpty()) {
        writeElement(&feature->extendedData(), writer);
    }

    writer.writeEndElement();
    return result;
}

}
