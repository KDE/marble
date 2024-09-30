// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlListStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataListStyle.h"
#include "GeoDataParser.h"
#include "GeoDataStyle.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(ListStyle)

GeoNode *KmlListStyleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_ListStyle)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Style)) {
        GeoDataListStyle style;
        KmlObjectTagHandler::parseIdentifiers(parser, &style);

        parentItem.nodeAs<GeoDataStyle>()->setListStyle(style);
        return &parentItem.nodeAs<GeoDataStyle>()->listStyle();
    }
    return nullptr;
}

}
}
