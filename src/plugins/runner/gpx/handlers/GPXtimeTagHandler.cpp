// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GPXtimeTagHandler.h"

#include "GPXElementDictionary.h"
#include "GeoDataTrack.h"
#include "GeoParser.h"

#include <QDateTime>

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(time)

GeoNode *GPXtimeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_time)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trkpt)) {
        auto track = parentItem.nodeAs<GeoDataTrack>();
        const QDateTime dateTime = QDateTime::fromString(parser.readElementText().trimmed(), Qt::ISODate);
        track->appendWhen(dateTime);
    }
    return nullptr;
}

} // namespace gpx

} // namespace Marble
