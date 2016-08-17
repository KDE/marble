//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlGroundOverlayTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( GroundOverlay )

GeoNode* KmlGroundOverlayTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_GroundOverlay)));

    GeoDataGroundOverlay *overlay = new GeoDataGroundOverlay;
    KmlObjectTagHandler::parseIdentifiers( parser, overlay );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ||
        parentItem.represents( kmlTag_Change ) || parentItem.represents( kmlTag_Create ) || parentItem.represents( kmlTag_Delete ) ) {
        parentItem.nodeAs<GeoDataContainer>()->append( overlay );
        return overlay;
    } else if (parentItem.qualifiedName().first == QLatin1String(kmlTag_kml)) {
        GeoDataDocument* doc = geoDataDoc( parser );
        doc->append( overlay );
        return overlay;
    } else {
        delete overlay;
        return 0;
    }
}

}
}
