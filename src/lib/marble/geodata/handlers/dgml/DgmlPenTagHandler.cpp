/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlPenTagHandler.h"

#include <QColor>
#include <QPen>
#include <QString>

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGeodata.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Pen)

GeoNode *DgmlPenTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Pen)));

    QString color = parser.attribute(dgmlAttr_color).trimmed();
    QString style = parser.attribute(dgmlAttr_style).toLower().trimmed();
    QString widthString = parser.attribute(dgmlAttr_width);
    qreal width = widthString.isEmpty() ? 1.0 : widthString.toDouble();

    QPen pen;

    if (!color.isEmpty() && QColor(color).isValid()) {
        pen.setColor(QColor(color));
    }

    if (!style.isEmpty()) {
        if (style == QLatin1StringView("nopen")) {
            pen.setStyle(Qt::NoPen);
        } else if (style == QLatin1StringView("solidline")) {
            pen.setStyle(Qt::SolidLine);
        } else if (style == QLatin1StringView("dashline")) {
            pen.setStyle(Qt::DashLine);
        } else if (style == QLatin1StringView("dotline")) {
            pen.setStyle(Qt::DotLine);
        } else if (style == QLatin1StringView("dashdotline")) {
            pen.setStyle(Qt::DashDotLine);
        } else if (style == QLatin1StringView("dashdotdotline")) {
            pen.setStyle(Qt::DashDotDotLine);
        }
    }

    if (width != 0.0) {
        pen.setWidthF(width);
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Vector) || parentItem.represents(dgmlTag_Geodata)) {
        auto geodata = parentItem.nodeAs<GeoSceneGeodata>();
        geodata->setPen(pen);
    }

    return nullptr;
}

}
}
