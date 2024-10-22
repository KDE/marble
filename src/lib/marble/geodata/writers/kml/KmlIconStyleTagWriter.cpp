// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlIconStyleTagWriter.h"

#include "GeoDataIconStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerIconStyle(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataIconStyleType),
                                                                           QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                               new KmlIconStyleTagWriter);

KmlIconStyleTagWriter::KmlIconStyleTagWriter()
    : KmlColorStyleTagWriter(QString::fromLatin1(kml::kmlTag_IconStyle))
{
    // nothing to do
}

bool KmlIconStyleTagWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    const auto style = static_cast<const GeoDataIconStyle *>(node);

    if (style->scale() != 1.0) {
        writer.writeElement(QString::fromLatin1(kml::kmlTag_scale), QString::number(style->scale(), 'f'));
    }

    if (!style->size().isEmpty()) {
        writer.writeNamespace(QString::fromUtf8(kml::kmlTag_nameSpaceMx), QStringLiteral("mx"));
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceMx), QString::fromUtf8(kml::kmlTag_size));
        writer.writeAttribute(QString::fromUtf8(kml::kmlTag_width), QString::number(style->size().width()));
        writer.writeAttribute(QString::fromUtf8(kml::kmlTag_height), QString::number(style->size().height()));
        writer.writeEndElement();
    }

    if (!style->iconPath().isEmpty()) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_Icon));
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_href));
        writer.writeCharacters(style->iconPath());
        writer.writeEndElement();
        writer.writeEndElement();
    }

    GeoDataHotSpot::Units xunits, yunits;
    QPointF const hotSpot = style->hotSpot(xunits, yunits);
    bool const emptyHotSpot = hotSpot.x() == 0.5 && hotSpot.y() == 0.5 && xunits == GeoDataHotSpot::Fraction && yunits == GeoDataHotSpot::Fraction;
    if (!emptyHotSpot) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_hotSpot));
        if (hotSpot.x() != 0.5 || xunits != GeoDataHotSpot::Fraction) {
            writer.writeAttribute("x", QString::number(hotSpot.x(), 'f'));
        }
        if (hotSpot.y() != 0.5 || yunits != GeoDataHotSpot::Fraction) {
            writer.writeAttribute("y", QString::number(hotSpot.y(), 'f'));
        }

        if (xunits != GeoDataHotSpot::Fraction) {
            writer.writeAttribute("xunits", unitString(xunits));
        }
        if (yunits != GeoDataHotSpot::Fraction) {
            writer.writeAttribute("yunits", unitString(yunits));
        }
        writer.writeEndElement();
    }

    return true;
}

bool KmlIconStyleTagWriter::isEmpty(const GeoNode *node) const
{
    const auto style = static_cast<const GeoDataIconStyle *>(node);
    GeoDataHotSpot::Units xunits, yunits;
    QPointF const hotSpot = style->hotSpot(xunits, yunits);
    return style->iconPath().isEmpty() && hotSpot.x() == 0.5 && hotSpot.y() == 0.5 && xunits == GeoDataHotSpot::Fraction && yunits == GeoDataHotSpot::Fraction;
}

QString KmlIconStyleTagWriter::unitString(GeoDataHotSpot::Units unit)
{
    switch (unit) {
    case GeoDataHotSpot::Pixels:
        return QStringLiteral("pixels");
    case GeoDataHotSpot::InsetPixels:
        return QStringLiteral("insetPixels");
    case GeoDataHotSpot::Fraction:
        return QStringLiteral("fraction");
    }

    return QStringLiteral("fraction");
}

}
