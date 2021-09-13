/*
    SPDX-FileCopyrightText: 2011 Anders Lund <anders@alweb.dk>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXrteTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataLineString.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(rte)

GeoNode* GPXrteTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_rte)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_gpx))
    {
        GeoDataDocument* doc = parentItem.nodeAs<GeoDataDocument>();

        // placemark for the linestring
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        doc->append(placemark);
        GeoDataLineString *linestring = new GeoDataLineString;
        placemark->setGeometry(linestring);
        placemark->setStyleUrl(QStringLiteral("#map-route"));

        return placemark;
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
