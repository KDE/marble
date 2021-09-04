/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlBrushTagHandler.h"

#include <QBrush>
#include <QColor>
#include <QList>
#include <QString>

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneGeodata.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Brush)

GeoNode* DgmlBrushTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Brush)));

    QString color = parser.attribute(dgmlAttr_color).trimmed();
    QString colorMap = parser.attribute(dgmlAttr_colorMap).trimmed();
    qreal alpha = parser.attribute(dgmlAttr_alpha).isEmpty() ? 1.0 : parser.attribute(dgmlAttr_alpha).toDouble();

    QBrush brush;

    if ( !color.isEmpty() && QColor( color ).isValid() ) {
        QColor brushColor( color );
        if (color.contains(QLatin1String("transparent"))) {
            brushColor.setAlphaF( 0.0 );
        }
        else {
            brushColor.setAlphaF( alpha );
        }
        brush.setColor( brushColor );
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( dgmlTag_Vector )
         || parentItem.represents( dgmlTag_Geodata ) ) {
        GeoSceneGeodata *geodata = parentItem.nodeAs<GeoSceneGeodata>();
        geodata->setBrush( brush );
        if ( !colorMap.isEmpty() ) {
            const QStringList colorString = colorMap.split(QLatin1Char(','));

            QVector<QColor> colorList;
            colorList.reserve(colorString.size());
            for(const QString& colorName: colorString) {
                colorList.append(QColor(colorName));
            }
            geodata->setColors( colorList );
        }
        geodata->setAlpha( alpha );
    }

    return nullptr;
}

}
}
