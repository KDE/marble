/*
    SPDX-FileCopyrightText: 2011 Anders Lund <anders@alweb.dk>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXrteptTagHandler.h"

#include <QStringView>

#include "GPXElementDictionary.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(rtept)

GeoNode *GPXrteptTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_rtept)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_rte)) {
        auto placemark = parentItem.nodeAs<GeoDataPlacemark>();
        auto linestring = static_cast<GeoDataLineString *>(placemark->geometry());
        GeoDataCoordinates coord;

        QXmlStreamAttributes attributes = parser.attributes();
        QStringView tmp;
        qreal lat = 0;
        qreal lon = 0;
        tmp = attributes.value(QLatin1StringView(gpxTag_lat));
        if (!tmp.isEmpty()) {
            lat = tmp.toString().toFloat();
        }
        tmp = attributes.value(QLatin1StringView(gpxTag_lon));
        if (!tmp.isEmpty()) {
            lon = tmp.toString().toFloat();
        }
        coord.set(lon, lat, 0, GeoDataCoordinates::Degree);
        linestring->append(coord);
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
