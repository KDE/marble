// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GPXTrackPointExtensionTagHandler.h"

#include "GPXElementDictionary.h"
#include "GeoDataTrack.h"
#include "GeoParser.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER_GARMIN_TRACKPOINTEXT1(TrackPointExtension)

GeoNode *GPXTrackPointExtensionTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_TrackPointExtension)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataTrack>()) {
        auto track = parentItem.nodeAs<GeoDataTrack>();
        return track;
    }

    return nullptr;
}

} // namespace gpx

} // namespace Marble
