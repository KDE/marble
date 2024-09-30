// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>

#include "KmlLinkTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLink.h"
#include "GeoDataModel.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Link)

GeoNode *KmlLinkTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Link)));
    GeoDataLink link;
    KmlObjectTagHandler::parseIdentifiers(parser, &link);
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_NetworkLink)) {
        return &parentItem.nodeAs<GeoDataNetworkLink>()->link();
    } else if (parentItem.represents(kmlTag_Model)) {
        parentItem.nodeAs<GeoDataModel>()->setLink(link);
        return &parentItem.nodeAs<GeoDataModel>()->link();
    } else {
        return nullptr;
    }
}

}
}
