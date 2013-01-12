//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlUpdateTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataUpdate.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Update )

GeoNode* KmlUpdateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Update ) );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_NetworkLinkControl ) ) {
        return &parentItem.nodeAs<GeoDataNetworkLinkControl>()->update();
    }

    return 0;
}

}
}
