//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlScreenOverlayTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( ScreenOverlay )

GeoNode* KmlScreenOverlayTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_ScreenOverlay)));

    GeoDataScreenOverlay *overlay = new GeoDataScreenOverlay();
    KmlObjectTagHandler::parseIdentifiers( parser, overlay );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ||
        parentItem.represents( kmlTag_Change ) || parentItem.represents( kmlTag_Create ) || parentItem.represents( kmlTag_Delete ) ) {
        parentItem.nodeAs<GeoDataContainer>()->append( overlay );
        return overlay;
    } else if ( parentItem.qualifiedName().first == kmlTag_kml ) {
        GeoDataDocument* doc = geoDataDoc( parser );
        doc->append( overlay );
        return overlay;
    } else {
        delete overlay;
        return nullptr;
    }
    return nullptr;
}

}
}
