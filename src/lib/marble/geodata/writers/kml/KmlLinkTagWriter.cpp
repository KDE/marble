// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#include "KmlLinkTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLink(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataLinkType),
                                                                      QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                          new KmlLinkTagWriter);

bool KmlLinkTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto link = static_cast<const GeoDataLink *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Link));
    KmlObjectTagWriter::writeIdentifiers(writer, link);

    writer.writeElement(QString::fromLatin1(kml::kmlTag_href), link->href());

    QString const refreshMode = refreshModeToString(link->refreshMode());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_refreshMode), refreshMode, QStringLiteral("onChange"));

    writer.writeElement(QString::fromLatin1(kml::kmlTag_refreshInterval), QString::number(link->refreshInterval()));

    QString const viewRefreshMode = viewRefreshModeToString(link->viewRefreshMode());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_viewRefreshMode), viewRefreshMode, QStringLiteral("never"));

    writer.writeElement(QString::fromLatin1(kml::kmlTag_viewRefreshTime), QString::number(link->viewRefreshTime()));

    writer.writeElement(QString::fromLatin1(kml::kmlTag_viewBoundScale), QString::number(link->viewBoundScale()));

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_viewFormat), link->viewFormat());

    writer.writeOptionalElement(QLatin1String(kml::kmlTag_httpQuery), link->httpQuery());

    writer.writeEndElement();

    return true;
}

QString KmlLinkTagWriter::refreshModeToString(GeoDataLink::RefreshMode refreshMode)
{
    switch (refreshMode) {
    case GeoDataLink::OnInterval:
        return QStringLiteral("onInterval");
    case GeoDataLink::OnExpire:
        return QStringLiteral("onExpire");
    default:
        return QStringLiteral("onChange");
    }
}

QString KmlLinkTagWriter::viewRefreshModeToString(GeoDataLink::ViewRefreshMode viewRefreshMode)
{
    switch (viewRefreshMode) {
    case GeoDataLink::OnStop:
        return QStringLiteral("onStop");
    case GeoDataLink::OnRequest:
        return QStringLiteral("onRequest");
    case GeoDataLink::OnRegion:
        return QStringLiteral("onRegion");
    default:
        return QStringLiteral("never");
    }
}

}
