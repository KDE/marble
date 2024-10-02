/*
    SPDX-FileCopyrightText: 2011 Anders Lund <anders@alweb.dk>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXrteTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(rte)

GeoNode *GPXrteTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_rte)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_gpx)) {
        auto doc = parentItem.nodeAs<GeoDataDocument>();

        // placemark for the linestring
        auto placemark = new GeoDataPlacemark;
        doc->append(placemark);
        auto linestring = new GeoDataLineString;
        placemark->setGeometry(linestring);
        placemark->setStyleUrl(QStringLiteral("#map-route"));

        return placemark;
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
