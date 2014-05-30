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
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Brush));

    QString color = parser.attribute(dgmlAttr_color).trimmed();
    QString colorMap = parser.attribute(dgmlAttr_colorMap).trimmed();
    qreal alpha = parser.attribute(dgmlAttr_alpha).isEmpty() ? 1.0 : parser.attribute(dgmlAttr_alpha).toDouble();

    QBrush brush;

    if ( !color.isEmpty() && QColor( color ).isValid() ) {
        QColor brushColor( color );
        brushColor.setAlphaF( alpha );
        brush.setColor( brushColor );
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( dgmlTag_Vector )
         || parentItem.represents( dgmlTag_Geodata ) ) {
        GeoSceneGeodata *geodata = parentItem.nodeAs<GeoSceneGeodata>();
        geodata->setBrush( brush );
        QList<QColor> colorList;
        if ( !colorMap.isEmpty() ) {
            QStringList colorString = colorMap.split(",");
            for ( int i = 0; i < colorString.size(); ++i ) {
                colorList.append( QColor( colorString[i] ) );
           }
            geodata->setColors( colorList );
        }
        geodata->setAlpha( alpha );
    }

    return 0;
}

}
}
