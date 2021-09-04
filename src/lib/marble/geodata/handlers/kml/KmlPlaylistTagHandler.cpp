// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlPlaylistTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTour.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( Playlist )

GeoNode* KmlPlaylistTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Playlist)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataTour>()) {
        GeoDataPlaylist *playlist = new GeoDataPlaylist;
        KmlObjectTagHandler::parseIdentifiers( parser, playlist );
        parentItem.nodeAs<GeoDataTour>()->setPlaylist(playlist);
        return playlist;
    }

    return nullptr;
}

} // namespace kml
} // namespace Marble
