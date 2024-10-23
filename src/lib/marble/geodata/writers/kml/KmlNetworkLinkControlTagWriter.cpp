// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlNetworkLinkControlTagWriter.h"

#include "GeoDataNetworkLinkControl.h"
#include "GeoDataTypes.h"
#include "GeoDataUpdate.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

#include <QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerNetworkLinkControl(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataNetworkLinkControlType),
                                                                                    QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                                        new KmlNetworkLinkControlTagWriter);

bool KmlNetworkLinkControlTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto networkLinkControl = static_cast<const GeoDataNetworkLinkControl *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_NetworkLinkControl));
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_minRefreshPeriod), QString::number(networkLinkControl->minRefreshPeriod()), QStringLiteral("1"));
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_maxSessionLength), QString::number(networkLinkControl->maxSessionLength()), QStringLiteral("2"));
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_cookie), networkLinkControl->cookie());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_message), networkLinkControl->message());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_linkName), networkLinkControl->linkName());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_linkDescription), networkLinkControl->linkDescription());

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_linkSnippet));

    if (networkLinkControl->maxLines() > 0) {
        writer.writeAttribute("maxLines", QString::number(networkLinkControl->maxLines()));
    }

    writer.writeCharacters(networkLinkControl->linkSnippet());
    writer.writeEndElement();

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_expires), networkLinkControl->expires().toString(Qt::ISODate));
    writeElement(&networkLinkControl->update(), writer);
    writer.writeEndElement();

    return true;
}

}
