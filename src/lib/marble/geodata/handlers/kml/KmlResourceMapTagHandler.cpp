// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlResourceMapTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataGeometry.h"
#include "GeoDataModel.h"
#include "GeoDataParser.h"
#include "GeoDataResourceMap.h"
#include "GeoDataScale.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(ResourceMap)

GeoNode *KmlResourceMapTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_ResourceMap)));

    GeoDataResourceMap map;
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Model)) {
        parentItem.nodeAs<GeoDataModel>()->setResourceMap(map);
        return &parentItem.nodeAs<GeoDataModel>()->resourceMap();
    } else {
        return nullptr;
    }
}

}
}
