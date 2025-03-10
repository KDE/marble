// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlChangeTagHandler.h"

#include "GeoDataChange.h"
#include "GeoDataUpdate.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Change)

GeoNode *KmlChangeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Change)));

    auto change = new GeoDataChange;
    KmlObjectTagHandler::parseIdentifiers(parser, change);
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Update)) {
        parentItem.nodeAs<GeoDataUpdate>()->setChange(change);
        return change;
    } else {
        delete change;
        return nullptr;
    }
}

}
}
