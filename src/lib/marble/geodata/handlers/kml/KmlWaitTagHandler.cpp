//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#include "KmlWaitTagHandler.h"
#include "KmlPlaylistTagHandler.h"

#include "KmlElementDictionary.h"
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
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Wait ) );

    GeoStackItem parentItem = parser.parentElement();

    GeoDataWait *wait = new GeoDataWait;
    wait->setId(QString(parser.attribute("id").toUtf8()));

    if (parentItem.is<GeoDataPlaylist>()) {
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(wait);
        return wait;
    } else {
        delete wait;
    }

    return 0;
}

} // namespace kml
} // namespace Marble
