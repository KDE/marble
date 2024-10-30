// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#include "KmlMemberTagHandler.h"

#include "GeoDataGeometry.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataTypes.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_MX(member)

GeoNode *KmlmemberTagHandler::parse(GeoParser &parser) const
{
    int memberIndex = parser.attribute("index").toInt();
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
    if (parser.parentElement(2).is<GeoDataPlacemark>()) {
        auto placemark = parser.parentElement(2).nodeAs<GeoDataPlacemark>();
        auto polygon = geodata_cast<GeoDataPolygon>(placemark->geometry());
        if (!polygon) {
            return nullptr;
        }

        // The memberIndex is used to determine which member this tag represents
        if (memberIndex == -1) {
            return &polygon->outerBoundary();
        } else {
            if (memberIndex >= polygon->innerBoundaries().size()) {
                return nullptr;
            }
            return &polygon->innerBoundaries()[memberIndex];
        }
    }

    return nullptr;
}

}
}
