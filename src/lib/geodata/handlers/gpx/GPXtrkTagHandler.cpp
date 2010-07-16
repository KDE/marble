/*
    This file is part of the Marble Desktop Globe.

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

#include "GPXtrkTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(trk)

GeoNode* GPXtrkTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_trk));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_gpx))
    {
        GeoDataDocument* doc = parentItem.nodeAs<GeoDataDocument>();
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        GeoDataMultiGeometry multigeometry;
        placemark->setGeometry(multigeometry);
        placemark->setStyleUrl("#map-track");

        doc->append(placemark);
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_trk << "> trk: " << doc->size();
#endif
        return placemark;
    }
    mDebug() << "trk parsing with parentitem" << parentItem.qualifiedName();
    return 0;
}

} // namespace gpx

} // namespace Marble
