// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlLatLonQuadWriter.h"

#include "GeoDataLatLonQuad.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLatLonQuad(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataLatLonQuadType),
                                                                            QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                                new KmlLatLonQuadWriter);

bool KmlLatLonQuadWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto latLonQuad = static_cast<const GeoDataLatLonQuad *>(node);

    if (latLonQuad->isValid()) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_LatLonQuad));
        KmlObjectTagWriter::writeIdentifiers(writer, latLonQuad);

        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_coordinates));

        writer.writeCharacters(QString::number(latLonQuad->bottomLeftLongitude(GeoDataCoordinates::Degree)));
        writer.writeCharacters(QLatin1Char(','));
        writer.writeCharacters(QString::number(latLonQuad->bottomLeftLatitude(GeoDataCoordinates::Degree)));
        writer.writeCharacters(QLatin1Char(' '));

        writer.writeCharacters(QString::number(latLonQuad->bottomRightLongitude(GeoDataCoordinates::Degree)));
        writer.writeCharacters(QLatin1Char(','));
        writer.writeCharacters(QString::number(latLonQuad->bottomRightLatitude(GeoDataCoordinates::Degree)));
        writer.writeCharacters(QLatin1Char(' '));

        writer.writeCharacters(QString::number(latLonQuad->topRightLongitude(GeoDataCoordinates::Degree)));
        writer.writeCharacters(QLatin1Char(','));
        writer.writeCharacters(QString::number(latLonQuad->topRightLatitude(GeoDataCoordinates::Degree)));
        writer.writeCharacters(QLatin1Char(' '));

        writer.writeCharacters(QString::number(latLonQuad->topLeftLongitude(GeoDataCoordinates::Degree)));
        writer.writeCharacters(QLatin1Char(','));
        writer.writeCharacters(QString::number(latLonQuad->topLeftLatitude(GeoDataCoordinates::Degree)));

        writer.writeEndElement();

        writer.writeEndElement();
    }

    return true;
}

}
