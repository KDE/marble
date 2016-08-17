/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "DgmlPenTagHandler.h"

#include <QPen>
#include <QColor>
#include <QString>

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGeodata.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Pen)

GeoNode* DgmlPenTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Pen)));

    QString color = parser.attribute(dgmlAttr_color).trimmed();
    QString style = parser.attribute(dgmlAttr_style).toLower().trimmed();
    QString widthString = parser.attribute(dgmlAttr_width);
    qreal  width = widthString.isEmpty() ? 1.0 : widthString.toDouble();

    QPen pen;

    if ( !color.isEmpty() && QColor( color ).isValid() ) {
        pen.setColor( QColor( color ) );
    }

    if ( !style.isEmpty() ) {
        if (style == QLatin1String("nopen")) {
            pen.setStyle(Qt::NoPen);
        } else if (style == QLatin1String("solidline")) {
            pen.setStyle(Qt::SolidLine);
        } else if (style == QLatin1String("dashline")) {
            pen.setStyle(Qt::DashLine);
        } else if (style == QLatin1String("dotline")) {
            pen.setStyle(Qt::DotLine);
        } else if (style == QLatin1String("dashdotline")) {
            pen.setStyle(Qt::DashDotLine);
        } else if (style == QLatin1String("dashdotdotline")) {
            pen.setStyle(Qt::DashDotDotLine);
        }
    }

    if ( width != 0.0 ) {
        pen.setWidthF( width );
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( dgmlTag_Vector )
         || parentItem.represents( dgmlTag_Geodata ) ) {
        GeoSceneGeodata *geodata = parentItem.nodeAs<GeoSceneGeodata>();
        geodata->setPen( pen );
    }

    return 0;
}

}
}
