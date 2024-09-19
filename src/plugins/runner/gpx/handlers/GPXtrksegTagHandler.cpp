/*
    SPDX-FileCopyrightText: 2010 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXtrksegTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTrack.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(trkseg)

GeoNode *GPXtrksegTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_trkseg)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trk)) {
        GeoDataPlacemark *placemark = parentItem.nodeAs<GeoDataPlacemark>();
        GeoDataMultiGeometry *multigeometry = static_cast<GeoDataMultiGeometry *>(placemark->geometry());
        GeoDataTrack *track = new GeoDataTrack;

        multigeometry->append(track);
        return track;
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
