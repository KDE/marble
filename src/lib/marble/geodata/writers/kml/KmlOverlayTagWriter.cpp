// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlOverlayTagWriter.h"

#include "GeoDataOverlay.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlColorStyleTagWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

KmlOverlayTagWriter::KmlOverlayTagWriter(const QString &elementName)
    : KmlFeatureTagWriter(elementName)
{
    // nothing to do
}

bool KmlOverlayTagWriter::writeMid(const Marble::GeoNode *node, GeoWriter &writer) const
{
    auto const *overlay = static_cast<const GeoDataOverlay *>(node);

    QString const color = KmlColorStyleTagWriter::formatColor(overlay->color());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_color), color, QStringLiteral("ffffffff"));
    QString const drawOrder = QString::number(overlay->drawOrder());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_drawOrder), drawOrder, QStringLiteral("0"));

    if (!overlay->iconFile().isEmpty()) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Icon));
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_href));
        writer.writeCharacters(overlay->iconFile());
        writer.writeEndElement();
        writer.writeEndElement();
    }

    return true;
}

}
