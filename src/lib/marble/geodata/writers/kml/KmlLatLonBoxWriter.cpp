// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyatf@gmail.com>
//

#include "KmlLatLonBoxWriter.h"

#include "GeoDataLatLonBox.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataLatLonBoxType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlLatLonBoxWriter);

bool KmlLatLonBoxWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto lat_lon_box = static_cast<const GeoDataLatLonBox *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_LatLonBox));
    KmlObjectTagWriter::writeIdentifiers(writer, lat_lon_box);

    writer.writeTextElement("north", QString::number(lat_lon_box->north(GeoDataCoordinates::Degree)));
    writer.writeTextElement("south", QString::number(lat_lon_box->south(GeoDataCoordinates::Degree)));
    writer.writeTextElement("east", QString::number(lat_lon_box->east(GeoDataCoordinates::Degree)));
    writer.writeTextElement("west", QString::number(lat_lon_box->west(GeoDataCoordinates::Degree)));
    writer.writeOptionalElement(QStringLiteral("rotation"), QString::number(lat_lon_box->rotation(GeoDataCoordinates::Degree)), QStringLiteral("0"));

    writer.writeEndElement();

    return true;
}

}
