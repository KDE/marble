// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

// Marble
#include "KmlOsmPlacemarkDataTagHandler.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataGeometry.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

#include <QVariant>

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_MX(OsmPlacemarkData)

GeoNode *KmlOsmPlacemarkDataTagHandler::parse(GeoParser &parser) const
{
    OsmPlacemarkData osmData = OsmPlacemarkData::fromParserAttributes(parser.attributes());
    /* Case 1: This is the main OsmPlacemarkData of a placemark:
     * <Placemark>
     *      <ExtendedData>
     *          <mx:OsmPlacemarkData>
     * ...
     */
    if (parser.parentElement().is<GeoDataExtendedData>() && parser.parentElement(1).is<GeoDataPlacemark>()) {
        auto placemark = parser.parentElement(1).nodeAs<GeoDataPlacemark>();
        placemark->setOsmData(osmData);
        return &placemark->osmData();
    }
    /* Case 2: This is the OsmPlacemarkData of a Nd
     * <Placemark>
     *      <ExtendedData>
     *          <mx:OsmPlacemarkData>
     *              <mx:nd>
     *                  <mx:OsmPlacemarkData>
     * ...
     */
    else if (parser.parentElement(1).is<OsmPlacemarkData>() && parser.parentElement().is<GeoDataPoint>()) {
        auto placemarkOsmData = parser.parentElement(1).nodeAs<OsmPlacemarkData>();
        auto point = parser.parentElement().nodeAs<GeoDataPoint>();
        GeoDataCoordinates coordinates = point->coordinates();
        /* The GeoDataPoint object was only used as GeoNode wrapper for the GeoDataCoordinates
         * and it is no longer needed
         */
        delete point;
        placemarkOsmData->addNodeReference(coordinates, osmData);
        return &placemarkOsmData->nodeReference(coordinates);
    }
    /* Case 3: This is the OsmPlacemarkData of a polygon's member
     * <Placemark>
     *      <ExtendedData>
     *          <mx:OsmPlacemarkData>
     *              <mx:member>
     *                  <mx:OsmPlacemarkData>
     * ...
     */
    else if (parser.parentElement(1).is<OsmPlacemarkData>() && parser.parentElement().is<GeoDataLinearRing>()
             && parser.parentElement(3).is<GeoDataPlacemark>()) {
        auto placemarkOsmData = parser.parentElement(1).nodeAs<OsmPlacemarkData>();
        auto placemark = parser.parentElement(3).nodeAs<GeoDataPlacemark>();
        GeoDataLinearRing &ring = *parser.parentElement().nodeAs<GeoDataLinearRing>();
        auto polygon = geodata_cast<GeoDataPolygon>(placemark->geometry());
        if (!polygon) {
            return nullptr;
        }

        /* The QList's indexOf function is perfect: returns the index of the ring
         * within the vector if the ring is an innerBoundary;
         * Else it returns -1, meaning it's an outerBoundary
         */
        int memberIndex = polygon->innerBoundaries().indexOf(ring);

        placemarkOsmData->addMemberReference(memberIndex, osmData);
        return &placemarkOsmData->memberReference(memberIndex);
    }
    return nullptr;
}
}
}
