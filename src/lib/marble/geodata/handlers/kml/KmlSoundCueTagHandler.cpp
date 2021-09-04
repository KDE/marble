// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmlSoundCueTagHandler.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"

#include "GeoDataPlaylist.h"
#include "GeoDataSoundCue.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( SoundCue )

GeoNode* KmlSoundCueTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_SoundCue)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlaylist>()) {
        GeoDataSoundCue *cue = new GeoDataSoundCue;
        KmlObjectTagHandler::parseIdentifiers( parser, cue );
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(cue);
        return cue;
    }

    return nullptr;
}

} // namespace kml
} // namespace Marble
