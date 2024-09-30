/*
    SPDX-FileCopyrightText: 2010 Thibaut GRIDEL <tgridel@free.fr>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "GPXtrkTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataDocument.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(trk)

GeoNode *GPXtrkTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_trk)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_gpx)) {
        GeoDataDocument *doc = parentItem.nodeAs<GeoDataDocument>();
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        doc->append(placemark);
        GeoDataMultiGeometry *multigeometry = new GeoDataMultiGeometry;
        placemark->setGeometry(multigeometry);
        placemark->setStyleUrl(QStringLiteral("#map-track"));

        return placemark;
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
