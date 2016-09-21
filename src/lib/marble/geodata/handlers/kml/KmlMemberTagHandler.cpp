//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#include "KmlMemberTagHandler.h"

#include "GeoDataPlacemark.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoDataGeometry.h"
#include "GeoDataTypes.h"
#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"
#include "GeoParser.h"

#include <QtDebug>

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_MX( member )

GeoNode* KmlmemberTagHandler::parse( GeoParser& parser ) const
{
    int memberIndex = parser.attribute( "index" ).toInt();
    /* Only possible case: member of polygon placemark:
     *...
    * <Placemark>
    *      <ExtendedData>
    *          <mx:OsmPlacemarkData>
    *              <mx:member index="-1">
    *                   <mx:OsmPlacemarkData>
    *                       <mx:nd index="0">...</nd>
    *                       <mx:nd index="1">...</nd>
    * ...
    */
    if( parser.parentElement( 2 ).is<GeoDataPlacemark>() ) {
        GeoDataPlacemark *placemark = parser.parentElement( 2 ).nodeAs<GeoDataPlacemark>();
        if ( placemark->geometry()->nodeType() != GeoDataTypes::GeoDataPolygonType ) {
            return 0;
        }
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark->geometry() );

        // The memberIndex is used to determine which member this tag represents
        if ( memberIndex == -1 ) {
            return &polygon->outerBoundary();
        }
        else {
            if ( memberIndex >= polygon->innerBoundaries().size() ) {
                return 0;
            }
            return &polygon->innerBoundaries()[ memberIndex ];
        }
    }

    return 0;
}

}
}
