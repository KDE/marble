// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlMaxSessionLengthTagHandler.h"

#include "GeoDataNetworkLinkControl.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(maxSessionLength)

GeoNode *KmlmaxSessionLengthTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_maxSessionLength)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_NetworkLinkControl)) {
        qreal maxSessionLength = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataNetworkLinkControl>()->setMaxSessionLength(maxSessionLength);
    }

    return nullptr;
}

}
}
