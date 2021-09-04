/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

    GeoSceneMap* map = nullptr;

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
