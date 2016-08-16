//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
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

    return 0;
}

} // namespace kml
} // namespace Marble
