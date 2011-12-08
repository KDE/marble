//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "GPXTrackPointExtensionTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTrack.h"
#include "GeoDataExtendedData.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER_GARMIN_TRACKPOINTEXT1(TrackPointExtension)

GeoNode* GPXTrackPointExtensionTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( gpxTag_TrackPointExtension ) );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.is<GeoDataTrack>() )
    {
        GeoDataTrack* track = parentItem.nodeAs<GeoDataTrack>();
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_TrackPointExtension << ">";
#endif
        return track;
    }

    mDebug() << "TrackPointExtension parsing with parentitem" << parentItem.qualifiedName();
    return 0;
}

} // namespace gpx

} // namespace Marble
