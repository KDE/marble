// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
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
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_NetworkLinkControl)));

    GeoStackItem parentItem = parser.parentElement();
    GeoDataNetworkLinkControl *networkLinkControl = new GeoDataNetworkLinkControl;

    if ( parentItem.qualifiedName().first == kmlTag_kml ) {
        GeoDataDocument* doc = geoDataDoc( parser );
        doc->append( networkLinkControl );
        return networkLinkControl;
    } else {
        delete networkLinkControl;
        return nullptr;
    }
}

}
}
