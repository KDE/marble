// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlLatLonQuadTagHandler.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"
#include "GeoDataLatLonQuad.h"
#include "GeoDataGroundOverlay.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( LatLonQuad )

GeoNode* KmlLatLonQuadTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_LatLonQuad)));
    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.represents( kmlTag_GroundOverlay ) )
    {
        GeoDataLatLonQuad* quad = &parentItem.nodeAs<GeoDataGroundOverlay>()->latLonQuad();
        KmlObjectTagHandler::parseIdentifiers( parser, quad );
        return quad;
    }

    return nullptr;
}

}
}
