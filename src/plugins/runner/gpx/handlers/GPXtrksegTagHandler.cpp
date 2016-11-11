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

    Copyright 2010 Thibaut GRIDEL <tgridel@free.fr>
*/

#include "GPXtrksegTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataTrack.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(trkseg)

GeoNode* GPXtrksegTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_trkseg)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trk))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();
        GeoDataMultiGeometry *multigeometry = static_cast<GeoDataMultiGeometry*>(placemark->geometry());
        GeoDataTrack *track = new GeoDataTrack;

        multigeometry->append( track );
        return track;
    }
    return 0;
}

} // namespace gpx

} // namespace Marble
