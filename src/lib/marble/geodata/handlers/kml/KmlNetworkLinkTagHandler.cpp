// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlNetworkLinkTagHandler.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(NetworkLink)

GeoNode *KmlNetworkLinkTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_NetworkLink)));

    auto networkLink = new GeoDataNetworkLink;
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Folder) || parentItem.represents(kmlTag_Document)) {
        parentItem.nodeAs<GeoDataContainer>()->append(networkLink);
        return networkLink;
    } else if (parentItem.qualifiedName().first == QString::fromLatin1(kmlTag_kml)) {
        GeoDataDocument *doc = geoDataDoc(parser);
        doc->append(networkLink);
        return networkLink;
    } else {
        delete networkLink;
        return nullptr;
    }
}

}
}
