/*
    SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlStyleMapTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataStyleMap.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(StyleMap)

GeoNode *KmlStyleMapTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_StyleMap)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Document)) {
        GeoDataStyleMap styleMap;
        KmlObjectTagHandler::parseIdentifiers(parser, &styleMap);
        parentItem.nodeAs<GeoDataDocument>()->addStyleMap(styleMap);
        return &parentItem.nodeAs<GeoDataDocument>()->styleMap(styleMap.id());
    } else if (parentItem.is<GeoDataFeature>()) {
        /*        GeoDataStyleMap styleMap;
                parentItem.nodeAs<GeoDataFeature>()->setStyleMap( styleMap );*/
    }

    return nullptr;
}

}
}
