// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlItemIconTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataItemIcon.h"
#include "GeoDataListStyle.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(ItemIcon)

GeoNode *KmlItemIconTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_ItemIcon)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ListStyle)) {
        auto itemIcon = new GeoDataItemIcon;
        parentItem.nodeAs<GeoDataListStyle>()->append(itemIcon);
        return itemIcon;
    }
    return nullptr;
}

}
}
