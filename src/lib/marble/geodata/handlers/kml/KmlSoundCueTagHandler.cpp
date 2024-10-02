// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlSoundCueTagHandler.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

#include "GeoDataPlaylist.h"
#include "GeoDataSoundCue.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22(SoundCue)

GeoNode *KmlSoundCueTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_SoundCue)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlaylist>()) {
        auto cue = new GeoDataSoundCue;
        KmlObjectTagHandler::parseIdentifiers(parser, cue);
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(cue);
        return cue;
    }

    return nullptr;
}

} // namespace kml
} // namespace Marble
