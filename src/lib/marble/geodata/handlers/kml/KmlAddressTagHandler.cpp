/*
    SPDX-FileCopyrightText: 2017 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlAddressTagHandler.h"

#include "GeoDataFeature.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(address)

GeoNode *KmladdressTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_address)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataFeature>()) {
        const auto address = parser.readElementText().trimmed();

        parentItem.nodeAs<GeoDataFeature>()->setAddress(address);
    }

    return nullptr;
}

}
}
