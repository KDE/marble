// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlLinkDescriptionTagHandler.h"

#include "GeoDataNetworkLinkControl.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(linkDescription)

GeoNode *KmllinkDescriptionTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_linkDescription)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_NetworkLinkControl)) {
        QString linkDescription = parser.readElementText();

        parentItem.nodeAs<GeoDataNetworkLinkControl>()->setLinkDescription(linkDescription);
    }

    return nullptr;
}

}
}
