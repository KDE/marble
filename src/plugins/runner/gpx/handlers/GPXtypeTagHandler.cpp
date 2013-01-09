//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXtypeTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataFolder.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(type)

// type is used in wpt, rte and trk to provide some sort of keyword, like "Geocache/traditional cache",
// and in link to provide a mimetype

GeoNode* GPXtypeTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_type));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt)
        || parentItem.represents(gpxTag_trk)
        || parentItem.represents(gpxTag_rtept))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        placemark->setRole(parser.readElementText().trimmed());
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_type << "> : " << placemark->role();
#endif
    }
    else if (parentItem.represents(gpxTag_rte))
    {
        GeoDataFeature* route = parentItem.nodeAs<GeoDataFeature>();
        route->setRole(parser.readElementText().trimmed());
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_type << "> : " << route->role();
#endif
    }
    // TODO: tracks, link
    return 0;
}

} // namespace gpx

} // namespace Marble
