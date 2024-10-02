// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>

#include "KmlFlyToTagHandler.h"
#include "GeoDataFlyTo.h"
#include "GeoDataPlaylist.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22(FlyTo)

GeoNode *KmlFlyToTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_FlyTo)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlaylist>()) {
        auto flyTo = new GeoDataFlyTo;
        KmlObjectTagHandler::parseIdentifiers(parser, flyTo);
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(flyTo);
        return flyTo;
    }

    return nullptr;
}

}
}
