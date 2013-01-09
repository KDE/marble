//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlRotationTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( rotation )

GeoNode* KmlrotationTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_rotation ) );

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents( kmlTag_ScreenOverlay ))
    {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataScreenOverlay>()->setRotation( rotation );
    }
    else if (parentItem.represents( kmlTag_LatLonBox ))
    {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataLatLonBox>()->setRotation( rotation * DEG2RAD );
    }
    return 0;
}

}
}
