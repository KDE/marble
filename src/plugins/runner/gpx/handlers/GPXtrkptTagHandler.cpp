/*
    SPDX-FileCopyrightText: 2009 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXtrkptTagHandler.h"

#include "MarbleDebug.h"

#include <QStringView>

#include "GPXElementDictionary.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataTrack.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(trkpt)

GeoNode *GPXtrkptTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_trkpt)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trkseg)) {
        auto track = parentItem.nodeAs<GeoDataTrack>();
        GeoDataCoordinates coord;

        QXmlStreamAttributes attributes = parser.attributes();
        QStringView tmp;
        qreal lat = 0;
        qreal lon = 0;
        tmp = attributes.value(QLatin1StringView(gpxTag_lat));
        if (!tmp.isEmpty()) {
            lat = tmp.toString().toDouble();
        }
        tmp = attributes.value(QLatin1StringView(gpxTag_lon));
        if (!tmp.isEmpty()) {
            lon = tmp.toString().toDouble();
        }
        coord.set(lon, lat, 0, GeoDataCoordinates::Degree);
        track->appendCoordinates(coord);

        return track;
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
