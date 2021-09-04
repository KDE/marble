// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#include "KmlWaitTagHandler.h"
#include "KmlPlaylistTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"
#include "GeoDataPlaylist.h"
#include "GeoDataWait.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( Wait )

GeoNode* KmlWaitTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Wait)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataWait *wait = new GeoDataWait;
    KmlObjectTagHandler::parseIdentifiers( parser, wait );

    if (parentItem.is<GeoDataPlaylist>()) {
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(wait);
        return wait;
    } else {
        delete wait;
    }

    return nullptr;
}

} // namespace kml
} // namespace Marble
