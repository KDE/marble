//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlOverlayXYTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataParser.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( overlayXY )

GeoNode* KmloverlayXYTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_overlayXY ) );

    GeoStackItem parentItem = parser.parentElement();
#ifdef DEBUG_TAGS
    mDebug() << "Parsed <" << kmlTag_overlayXY << ">"
             << " parent item name: " << parentItem.qualifiedName().first;
#endif

    if (parentItem.represents( kmlTag_ScreenOverlay ))
    {
        GeoDataVec2 vec2(parser.attribute("x").trimmed().toFloat(),
                         parser.attribute("y").trimmed().toFloat(),
                         parser.attribute("xunits").trimmed(),
                         parser.attribute("yunits").trimmed());

        parentItem.nodeAs<GeoDataScreenOverlay>()->setOverlayXY( vec2 );
    }
    return 0;
}

}
}
