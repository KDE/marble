// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXcmtTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoParser.h"

// type is used in wpt, rte and trk to provide some sort of keyword, like "Geocache/traditional cache",
// and in link to provide a mimetype

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(cmt)

GeoNode *GPXcmtTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_cmt)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt)) {
        GeoDataPlacemark *placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QString cmt = parser.readElementText().trimmed();
        if (!cmt.isEmpty()) {
            QString desc = placemark->description();
            if (!desc.isEmpty()) {
                desc += QLatin1StringView("<br/>");
            }
            placemark->setDescription(desc.append(cmt.replace(QLatin1Char('\n'), QLatin1StringView("\n<br/>"))));
            placemark->setDescriptionCDATA(true);
        }
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
