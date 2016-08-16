/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2008 Torsten Rahn <tackat@kde.org>

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

#include "DgmlMapTagHandler.h"

#include <QColor>

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneDocument.h"
#include "GeoSceneMap.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Map)

GeoNode* DgmlMapTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Map)));

    QColor  labelColor  = parser.attribute(dgmlAttr_labelColor).trimmed();

    if ( !labelColor.isValid() )
        labelColor = Qt::black;

    QColor highlightBrushColor = QColor ( parser.attribute(dgmlAttr_highlightBrush).trimmed() );
    QColor highlightPenColor = QColor ( parser.attribute(dgmlAttr_highlightPen).trimmed() );

    GeoSceneMap* map = 0;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Document)) {
        map = parentItem.nodeAs<GeoSceneDocument>()->map();
        map->setBackgroundColor( QColor( parser.attribute( dgmlAttr_bgcolor ).trimmed() ) );
        map->setLabelColor( labelColor );
        map->setHighlightBrushColor( highlightBrushColor );
        map->setHighlightPenColor( highlightPenColor );
    }

    return map;
}

}
}
