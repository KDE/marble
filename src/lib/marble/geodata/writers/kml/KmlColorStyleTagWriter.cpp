// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlColorStyleTagWriter.h"

#include "GeoDataColorStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

KmlColorStyleTagWriter::KmlColorStyleTagWriter(const QString &elementName)
    : m_elementName(elementName)
{
    // nothing to do
}

bool KmlColorStyleTagWriter::write(const Marble::GeoNode *node, GeoWriter &writer) const
{
    auto const *colorStyle = static_cast<const GeoDataColorStyle *>(node);

    if (colorStyle->id().isEmpty() && colorStyle->targetId().isEmpty() && colorStyle->color() == defaultColor()
        && colorStyle->colorMode() == GeoDataColorStyle::Normal && isEmpty(node)) {
        return true;
    }

    writer.writeStartElement(m_elementName);

    KmlObjectTagWriter::writeIdentifiers(writer, colorStyle);
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_color), formatColor(colorStyle->color()), formatColor(defaultColor()));
    QString const colorMode = colorStyle->colorMode() == GeoDataColorStyle::Random ? QStringLiteral("random") : QStringLiteral("normal");
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_colorMode), colorMode, QStringLiteral("normal"));

    bool const result = writeMid(node, writer);
    writer.writeEndElement();
    return result;
}

QString KmlColorStyleTagWriter::formatColor(const QColor &color)
{
    QChar const fill = QLatin1Char('0');
    return QStringLiteral("%1%2%3%4")
        .arg(color.alpha(), 2, 16, fill)
        .arg(color.blue(), 2, 16, fill)
        .arg(color.green(), 2, 16, fill)
        .arg(color.red(), 2, 16, fill);
}

QColor KmlColorStyleTagWriter::defaultColor() const
{
    return {Qt::white};
}

}
