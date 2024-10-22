// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlNetworkLinkControlTagHandler.h"

#include "GeoDataDocument.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(NetworkLinkControl)

GeoNode *KmlNetworkLinkControlTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_NetworkLinkControl)));

    GeoStackItem parentItem = parser.parentElement();
    auto networkLinkControl = new GeoDataNetworkLinkControl;

    if (parentItem.qualifiedName().first == QString::fromLatin1(kmlTag_kml)) {
        GeoDataDocument *doc = geoDataDoc(parser);
        doc->append(networkLinkControl);
        return networkLinkControl;
    } else {
        delete networkLinkControl;
        return nullptr;
    }
}

}
}
