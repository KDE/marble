/*
    SPDX-FileCopyrightText: 2011 Anders Lund <anders@alweb.dk>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXrteptTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(rtept)

GeoNode* GPXrteptTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_rtept)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_rte))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();
        GeoDataLineString* linestring = static_cast<GeoDataLineString*>(placemark->geometry());
        GeoDataCoordinates coord;

        QXmlStreamAttributes attributes = parser.attributes();
        QStringRef tmp;
        qreal lat = 0;
        qreal lon = 0;
        tmp = attributes.value(QLatin1String(gpxTag_lat));
        if ( !tmp.isEmpty() )
        {
            lat = tmp.toString().toFloat();
        }
        tmp = attributes.value(QLatin1String(gpxTag_lon));
        if ( !tmp.isEmpty() )
        {
            lon = tmp.toString().toFloat();
        }
        coord.set(lon, lat, 0, GeoDataCoordinates::Degree);
        linestring->append(coord);

    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
