//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlNetworkLinkTagHandler.h"

#include "GeoDataNetworkLink.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( NetworkLink )

GeoNode* KmlNetworkLinkTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_NetworkLink)));

    GeoDataNetworkLink *networkLink = new GeoDataNetworkLink;
    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ) {
        parentItem.nodeAs<GeoDataContainer>()->append( networkLink );
        return networkLink;
    } else if ( parentItem.qualifiedName().first == kmlTag_kml ) {
        GeoDataDocument* doc = geoDataDoc( parser );
        doc->append( networkLink );
        return networkLink;
    } else {
        delete networkLink;
        return 0;
    }
}

}
}
