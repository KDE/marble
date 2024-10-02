// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlAliasTagHandler.h"

#include "GeoDataAlias.h"
#include "GeoDataResourceMap.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Alias)

GeoNode *KmlAliasTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Alias)));

    GeoDataAlias alias;
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ResourceMap)) {
        parentItem.nodeAs<GeoDataResourceMap>()->setAlias(alias);
        return &parentItem.nodeAs<GeoDataResourceMap>()->alias();
    } else {
        return nullptr;
    }
}

}
}
