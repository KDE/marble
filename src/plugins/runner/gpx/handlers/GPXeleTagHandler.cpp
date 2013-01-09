//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GPXeleTagHandler.h"

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
GPX_DEFINE_TAG_HANDLER(ele)

GeoNode* GPXeleTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_ele));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_trkpt))
    {
        GeoDataTrack* track = parentItem.nodeAs<GeoDataTrack>();
        track->appendAltitude( parser.readElementText().trimmed().toDouble() );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_ele << ">";
#endif
        return track;
    }
    return 0;
}

} // namespace gpx

} // namespace Marble
