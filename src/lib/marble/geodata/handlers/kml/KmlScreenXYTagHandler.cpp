//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlScreenXYTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( screenXY )

GeoNode* KmlscreenXYTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_screenXY ) );

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ScreenOverlay ))
    {
        GeoDataVec2 vec2(parser.attribute("x").trimmed().toFloat(),
                         parser.attribute("y").trimmed().toFloat(),
                         parser.attribute("xunits").trimmed(),
                         parser.attribute("yunits").trimmed());

        parentItem.nodeAs<GeoDataScreenOverlay>()->setScreenXY( vec2 );
    }
    return 0;
}

}
}
