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

#include "GPXtrkptTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "GeoDataTrack.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(trkpt)

GeoNode* GPXtrkptTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_trkpt));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trkseg))
    {
        GeoDataTrack* track = parentItem.nodeAs<GeoDataTrack>();
        GeoDataCoordinates coord;

        QXmlStreamAttributes attributes = parser.attributes();
        QStringRef tmp;
        qreal lat = 0;
        qreal lon = 0;
        tmp = attributes.value(gpxTag_lat);
        if ( !tmp.isEmpty() )
        {
            lat = tmp.toString().toDouble();
        }
        tmp = attributes.value(gpxTag_lon);
        if ( !tmp.isEmpty() )
        {
            lon = tmp.toString().toDouble();
        }
        coord.set(lon, lat, 0, GeoDataCoordinates::Degree);
        track->appendCoordinates( coord );

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_trkpt << "> waypoint: " /*<< linestring->size()*/
                << coord.toString(GeoDataCoordinates::Decimal);
#endif
        return track;
    }
    mDebug() << "trkpt parsing with parentitem" << parentItem.qualifiedName();
    return 0;
}

} // namespace gpx

} // namespace Marble
