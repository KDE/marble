/*
    SPDX-FileCopyrightText: 2009 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXwptTagHandler.h"

#include "MarbleDebug.h"

#include <QStringView>

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
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_wpt)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_gpx))
    {
        GeoDataDocument* doc = parentItem.nodeAs<GeoDataDocument>();
        GeoDataPlacemark *placemark = new GeoDataPlacemark;

        QXmlStreamAttributes attributes = parser.attributes();
        QStringView tmp;
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
        placemark->setCoordinate( lon, lat, 0, GeoDataCoordinates::Degree );
        placemark->setRole(QStringLiteral("Waypoint"));

        placemark->setStyle(doc->style(QStringLiteral("waypoint")));

        doc->append(placemark);
        return placemark;
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
