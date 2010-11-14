//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#include "KmlLatLonAltBoxTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataParser.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataRegion.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( LatLonAltBox )

GeoNode* KmlLatLonAltBoxTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_LatLonAltBox ) );

    GeoDataLatLonAltBox box;

    GeoStackItem parentItem = parser.parentElement();
#ifdef DEBUG_TAGS
    mDebug() << "Parsed <" << kmlTag_LatLonAltBox << ">"
             << " parent item name: " << parentItem.qualifiedName().first;
#endif

    if( parentItem.represents( kmlTag_Region )
        || parentItem.represents( kmlTag_Region ) )
    {
        parentItem.nodeAs<GeoDataRegion>()->setLatLonAltBox( box );
        return &parentItem.nodeAs<GeoDataRegion>()->latLonAltBox();
    } else {
        return 0;
    }
}

}
}
