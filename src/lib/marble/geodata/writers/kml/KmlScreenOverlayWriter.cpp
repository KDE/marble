// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlScreenOverlayWriter.h"

#include "GeoDataScreenOverlay.h"
#include "GeoDataTypes.h"
#include "GeoTagWriter.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{
static GeoTagWriterRegistrar s_writerLookAt(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataScreenOverlayType),
                                                                        QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                            new KmlScreenOverlayWriter);

KmlScreenOverlayWriter::KmlScreenOverlayWriter()
    : KmlOverlayTagWriter(QString::fromLatin1(kml::kmlTag_ScreenOverlay))
{
    // nothing to do
}

bool KmlScreenOverlayWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    KmlOverlayTagWriter::writeMid(node, writer);

    const auto screenOverlay = static_cast<const GeoDataScreenOverlay *>(node);
    writeVec2(QString::fromLatin1(kml::kmlTag_overlayXY), screenOverlay->overlayXY(), writer);
    writeVec2(QString::fromLatin1(kml::kmlTag_screenXY), screenOverlay->screenXY(), writer);
    writeVec2(QString::fromLatin1(kml::kmlTag_rotationXY), screenOverlay->rotationXY(), writer);
    writeVec2(QString::fromLatin1(kml::kmlTag_size), screenOverlay->size(), writer);
    QString const rotation = QString::number(screenOverlay->rotation());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_rotation), rotation, QStringLiteral("0"));
    return true;
}

void KmlScreenOverlayWriter::writeVec2(const QString &element, const GeoDataVec2 &vec2, GeoWriter &writer)
{
    writer.writeStartElement(element);
    writer.writeAttribute("x", QString::number(vec2.x()));
    writer.writeAttribute("xunits", unitToString(vec2.xunit()));
    writer.writeAttribute("y", QString::number(vec2.y()));
    writer.writeAttribute("yunits", unitToString(vec2.yunit()));
    writer.writeEndElement();
}

QString KmlScreenOverlayWriter::unitToString(GeoDataVec2::Unit unit)
{
    switch (unit) {
    case GeoDataVec2::Fraction:
        return QStringLiteral("fraction");
    case GeoDataVec2::Pixels:
        return QStringLiteral("pixels");
    case GeoDataVec2::InsetPixels:
        return QStringLiteral("insetPixels");
    }

    Q_ASSERT(false);
    return QStringLiteral("fraction");
}

}
