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

#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QtCore/QString>

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneVector.h"
#include "GeoSceneGeodata.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Pen)

GeoNode* DgmlPenTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_Pen));

    QString color = parser.attribute(dgmlAttr_color).trimmed();
    QString style = parser.attribute(dgmlAttr_style).toLower().trimmed();
    qreal  width = parser.attribute(dgmlAttr_width).toDouble();

    GeoSceneVector *vector = 0;
    GeoSceneGeodata *geodata = 0;
    QPen pen;

    if ( !color.isEmpty() && QColor( color ).isValid() ) {
        pen.setColor( QColor( color ) ); 
    }

    if ( !style.isEmpty() ) {
        if ( style == "nopen" ) pen.setStyle( Qt::NoPen ); 
        if ( style == "solidline" ) pen.setStyle( Qt::SolidLine ); 
        if ( style == "dashline" ) pen.setStyle( Qt::DashLine ); 
        if ( style == "dotline" ) pen.setStyle( Qt::DotLine ); 
        if ( style == "dashdotline" ) pen.setStyle( Qt::DashDotLine ); 
        if ( style == "dashdotdotline" ) pen.setStyle( Qt::DashDotDotLine ); 
    }

    if ( width != 0.0 ) {
        pen.setWidthF( width );
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.represents( dgmlTag_Vector ) ) {
        vector = parentItem.nodeAs<GeoSceneVector>();
        vector->setPen( pen );
    } else if ( parentItem.represents( dgmlTag_Geodata ) ) {
        geodata = parentItem.nodeAs<GeoSceneGeodata>();
        geodata->setPen( pen );
    }

    return 0;
}

}
}
