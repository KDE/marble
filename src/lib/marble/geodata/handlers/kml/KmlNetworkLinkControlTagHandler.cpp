//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlNetworkLinkControlTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( NetworkLinkControl )

GeoNode* KmlNetworkLinkControlTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_NetworkLinkControl ) );

    GeoStackItem parentItem = parser.parentElement();
    GeoDataNetworkLinkControl *networkLinkControl = new GeoDataNetworkLinkControl;
    if ( parentItem.represents( kmlTag_kml ) ) {
        GeoDataDocument* doc = geoDataDoc( parser );
        doc->append( networkLinkControl );
        return networkLinkControl;
    } else {
        delete networkLinkControl;
        return 0;
    }
}

}
}
