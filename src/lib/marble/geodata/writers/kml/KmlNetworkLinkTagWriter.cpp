// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#include "KmlNetworkLinkTagWriter.h"

#include "GeoDataLink.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerNetworkLink(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataNetworkLinkType),
                                                                             QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                                 new KmlNetworkLinkTagWriter);

bool KmlNetworkLinkTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto networkLink = static_cast<const GeoDataNetworkLink *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_NetworkLink));

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_name), networkLink->name());

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_visibility), QString::number(networkLink->isVisible()), QStringLiteral("1"));

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_refreshVisibility), QString::number(networkLink->refreshVisibility()), QStringLiteral("0"));

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_flyToView), QString::number(networkLink->flyToView()), QStringLiteral("0"));

    writeElement(&networkLink->link(), writer);

    writer.writeEndElement();

    return true;
}

}
