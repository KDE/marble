// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

// Self
#include "KmlNdTagHandler.h"

// Marble
#include "GeoDataExtendedData.h"
#include "GeoDataGeometry.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

// Qt
#include <QDebug>

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_MX(nd)

GeoNode *KmlndTagHandler::parse(GeoParser &parser) const
{
    int ndIndex = parser.attribute("index").toInt();
    /* Case 1: node of a line placemark:
     *...
     * <Placemark>
     *      <ExtendedData>
     *          <mx:OsmPlacemarkData>
     *              <mx:nd index="0">...</nd>
     *              <mx:nd index="1">...</nd>
     * ...
     */
    if (parser.parentElement().represents(kmlTag_OsmPlacemarkData) && parser.parentElement(2).is<GeoDataPlacemark>()) {
        auto placemark = parser.parentElement(2).nodeAs<GeoDataPlacemark>();
        if (auto lineString = geodata_cast<GeoDataLineString>(placemark->geometry())) {
            // Using GeoDataPoint because GeoDataCoordinates is not a GeoNode, so it can't be returned.
            auto point = new GeoDataPoint(lineString->at(ndIndex));
            return point;
        }
        return nullptr;
    }
    /* Case 2: node of a polygon's boundary
     *...
     * <Placemark>
     *      <ExtendedData>
     *          <mx:OsmPlacemarkData>
     *              <mx:member>
     *                   <mx:OsmPlacemarkData>
     *                       <mx:nd index="0">...</nd>
     *                       <mx:nd index="1">...</nd>
     * ...
     */
    else if (parser.parentElement().represents(kmlTag_OsmPlacemarkData) && parser.parentElement(1).is<GeoDataLinearRing>()) {
        auto linearRing = parser.parentElement(1).nodeAs<GeoDataLinearRing>();

        // Using GeoDataPoint because GeoDataCoordinates is not a GeoNode, so it can't be returned.
        auto point = new GeoDataPoint(linearRing->at(ndIndex));
        return point;
    }
    return nullptr;
}
}
}
