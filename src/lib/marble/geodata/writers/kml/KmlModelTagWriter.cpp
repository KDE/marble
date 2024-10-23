// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlModelTagWriter.h"

#include "GeoDataAlias.h"
#include "GeoDataLocation.h"
#include "GeoDataModel.h"
#include "GeoDataOrientation.h"
#include "GeoDataResourceMap.h"
#include "GeoDataScale.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlLinkTagWriter.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerModel(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataModelType),
                                                                       QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                           new KmlModelTagWriter);

bool KmlModelTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto model = static_cast<const GeoDataModel *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Model));
    KmlObjectTagWriter::writeIdentifiers(writer, model);

    KmlGroundOverlayWriter::writeAltitudeMode(writer, model->altitudeMode());

    const GeoDataLocation location = model->location();

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Location));

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_longitude), QString::number(location.longitude(GeoDataCoordinates::Degree)), QStringLiteral("0"));
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_latitude), QString::number(location.latitude(GeoDataCoordinates::Degree)), QStringLiteral("0"));
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_altitude), QString::number(location.altitude()), QStringLiteral("0"));

    writer.writeEndElement();

    const GeoDataOrientation orientation = model->orientation();

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Orientation));

    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_heading), QString::number(orientation.heading()), QStringLiteral("0"));
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_tilt), QString::number(orientation.tilt()), QStringLiteral("0"));
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_roll), QString::number(orientation.roll()), QStringLiteral("0"));

    writer.writeEndElement();

    const GeoDataScale scale = model->scale();

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Scale));

    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_x), QString::number(scale.x()), QStringLiteral("1"));
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_y), QString::number(scale.y()), QStringLiteral("1"));
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_z), QString::number(scale.z()), QStringLiteral("1"));

    writer.writeEndElement();

    writeElement(&model->link(), writer);

    const GeoDataResourceMap map = model->resourceMap();

    writer.writeStartElement(QString::fromLatin1(kml::kmlTag_ResourceMap));

    const GeoDataAlias alias = map.alias();
    writer.writeStartElement(QString::fromLatin1(kml::kmlTag_Alias));

    writer.writeTextElement(QString::fromLatin1(kml::kmlTag_targetHref), alias.targetHref());
    writer.writeTextElement(QString::fromLatin1(kml::kmlTag_sourceHref), alias.sourceHref());

    writer.writeEndElement();

    writer.writeEndElement();

    writer.writeEndElement();
    return true;
}

}
