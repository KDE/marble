//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014       Sanjiban Bairagya <sanjiban22393@gmail.com>

#include "KmlAnimatedUpdateTagHandler.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataPlaylist.h"
#include "GeoParser.h"
#include "MarbleDebug.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22( AnimatedUpdate )

GeoNode* KmlAnimatedUpdateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_AnimatedUpdate)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataAnimatedUpdate *animatedUpdate = new GeoDataAnimatedUpdate;
    KmlObjectTagHandler::parseIdentifiers( parser, animatedUpdate );

    if (parentItem.is<GeoDataPlaylist>()) {
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive( animatedUpdate );
        return animatedUpdate;
    } else {
        delete animatedUpdate;
    }

    return 0;
}
}
}
}
