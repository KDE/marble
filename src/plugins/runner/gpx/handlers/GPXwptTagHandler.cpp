/*
    This file is part of the Marble Virtual Globe.

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library. If not, see <http://www.gnu.org/licenses/>.

    Copyright 2009 Thibaut GRIDEL <tgridel@free.fr>
*/

#include "GPXwptTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(wpt)

GeoNode* GPXwptTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_wpt));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_gpx))
    {
        GeoDataDocument* doc = parentItem.nodeAs<GeoDataDocument>();
        GeoDataPlacemark *placemark = new GeoDataPlacemark;

        QXmlStreamAttributes attributes = parser.attributes();
        QStringRef tmp;
        qreal lat = 0;
        qreal lon = 0;
        tmp = attributes.value(gpxTag_lat);
        if ( !tmp.isEmpty() )
        {
            lat = tmp.toString().toFloat();
        }
        tmp = attributes.value(gpxTag_lon);
        if ( !tmp.isEmpty() )
        {
            lon = tmp.toString().toFloat();
        }
        placemark->setCoordinate( lon, lat, 0, GeoDataPoint::Degree );
        placemark->setRole("Waypoint");

        placemark->setStyle(&doc->style("waypoint"));

        doc->append(placemark);
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_wpt << "> waypoint: " << doc->size();
#endif
        return placemark;
    }
    mDebug() << "wpt parsing with parentitem" << parentItem.qualifiedName();
    return 0;
}

} // namespace gpx

} // namespace Marble
