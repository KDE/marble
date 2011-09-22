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

    Copyright 2011 Thibaut GRIDEL <tgridel@free.fr>
    Copyright 2011 Anders Lund <anders@alweb.dk>
*/

#include "GPXrteTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(rte)

GeoNode* GPXrteTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_rte));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_gpx))
    {
        GeoDataDocument* doc = parentItem.nodeAs<GeoDataDocument>();
        
        // route is a folder, containing a linestring and individual routepoints
        GeoDataFolder *folder = new GeoDataFolder;
        doc->append(folder);
        // placemark for the linestring
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        folder->append(placemark);
        GeoDataLineString *linestring = new GeoDataLineString;
        placemark->setGeometry(linestring);
        placemark->setStyleUrl("#map-route");

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_rte << "> rte: " << doc->size();
#endif
        return folder;
    }
    mDebug() << "rte parsing with parentitem" << parentItem.qualifiedName();
    return 0;
}

} // namespace gpx

} // namespace Marble
