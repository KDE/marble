// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>

#include "KmlAnimatedUpdateTagHandler.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataPlaylist.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22(AnimatedUpdate)

GeoNode *KmlAnimatedUpdateTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_AnimatedUpdate)));

    GeoStackItem parentItem = parser.parentElement();

    auto animatedUpdate = new GeoDataAnimatedUpdate;
    KmlObjectTagHandler::parseIdentifiers(parser, animatedUpdate);

    if (parentItem.is<GeoDataPlaylist>()) {
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(animatedUpdate);
        return animatedUpdate;
    } else {
        delete animatedUpdate;
    }

    return nullptr;
}
}
}
}
