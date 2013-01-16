//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlLatLonQuadTagHandler.h"

#include "KmlElementDictionary.h"
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
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_LatLonQuad ) );
    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.represents( kmlTag_GroundOverlay ) )
    {
        return &parentItem.nodeAs<GeoDataGroundOverlay>()->latLonQuad();
    }

    return 0;
}

}
}
