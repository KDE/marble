// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GPXeleTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataPoint.h"
#include "GeoDataTrack.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(ele)

GeoNode *GPXeleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_ele)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trkpt)) {
        auto track = parentItem.nodeAs<GeoDataTrack>();
        track->appendAltitude(parser.readElementText().trimmed().toDouble());
        return track;
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
