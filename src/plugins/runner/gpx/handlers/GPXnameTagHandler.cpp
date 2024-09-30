/*
    SPDX-FileCopyrightText: 2009 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXnameTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(name)

GeoNode *GPXnameTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_name)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt) || parentItem.represents(gpxTag_trk) || parentItem.represents(gpxTag_rtept)) {
        GeoDataPlacemark *placemark = parentItem.nodeAs<GeoDataPlacemark>();

        placemark->setName(parser.readElementText().trimmed());
    } else if (parentItem.represents(gpxTag_rte)) {
        GeoDataFeature *route = parentItem.nodeAs<GeoDataFeature>();
        route->setName(parser.readElementText().trimmed());
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
