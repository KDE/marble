// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlLinearRingTagWriter.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLinearRing.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataLinearRingType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlLinearRingTagWriter);

bool KmlLinearRingTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto ring = static_cast<const GeoDataLinearRing *>(node);

    if (ring->size() > 1) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_LinearRing));
        KmlObjectTagWriter::writeIdentifiers(writer, ring);
        writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_extrude), QString::number(ring->extrude()), QStringLiteral("0"));
        writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_tessellate), QString::number(ring->tessellate()), QStringLiteral("0"));
        writer.writeStartElement("coordinates");

        int size = ring->size() >= 3 && ring->first() != ring->last() ? ring->size() + 1 : ring->size();

        for (int i = 0; i < size; ++i) {
            GeoDataCoordinates coordinates = ring->at(i % ring->size());
            if (i > 0) {
                writer.writeCharacters(" ");
            }

            qreal lon = coordinates.longitude(GeoDataCoordinates::Degree);
            writer.writeCharacters(QString::number(lon, 'f', 10));
            writer.writeCharacters(",");
            qreal lat = coordinates.latitude(GeoDataCoordinates::Degree);
            writer.writeCharacters(QString::number(lat, 'f', 10));
        }

        writer.writeEndElement();
        writer.writeEndElement();

        return true;
    }

    return false;
}

}
