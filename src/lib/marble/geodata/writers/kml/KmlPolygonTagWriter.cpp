// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyatf@gmail.com>
// SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlPolygonTagWriter.h"

#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataPolygonType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlPolygonTagWriter);

bool KmlPolygonTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto polygon = static_cast<const GeoDataPolygon *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Polygon));
    KmlObjectTagWriter::writeIdentifiers(writer, polygon);
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_extrude), QString::number(polygon->extrude()), QStringLiteral("0"));

    writer.writeStartElement("outerBoundaryIs");
    writeElement(&polygon->outerBoundary(), writer);
    writer.writeEndElement();

    const QList<GeoDataLinearRing> &linearRings = polygon->innerBoundaries();
    if (!linearRings.isEmpty()) {
        writer.writeStartElement("innerBoundaryIs");
        for (int i = 0; i < linearRings.size(); ++i) {
            const GeoDataLinearRing &ring = linearRings[i];
            writeElement(&ring, writer);
        }
        writer.writeEndElement();
    }

    writer.writeEndElement();

    return true;
}

}
