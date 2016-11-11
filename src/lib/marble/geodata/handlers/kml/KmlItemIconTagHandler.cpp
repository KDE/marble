//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlItemIconTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataListStyle.h"
#include "GeoDataItemIcon.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ItemIcon )

GeoNode* KmlItemIconTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_ItemIcon)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_ListStyle ) ) {
        GeoDataItemIcon *itemIcon = new GeoDataItemIcon;
        parentItem.nodeAs<GeoDataListStyle>()->append( itemIcon );
        return itemIcon;
    }
    return 0;
}

}
}
