//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GPXtimeTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTrack.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(time)

GeoNode* GPXtimeTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_time));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trkpt))
    {
        GeoDataTrack* track = parentItem.nodeAs<GeoDataTrack>();
        QDateTime dateTime = QDateTime::fromString( parser.readElementText().trimmed(), Qt::ISODate );
        track->appendWhen( dateTime );

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_time << ">" << dateTime;
#endif
    }
    return 0;
}

} // namespace gpx

} // namespace Marble
