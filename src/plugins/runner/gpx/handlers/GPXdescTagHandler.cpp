//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Anders Lund <anders@alweb.dk>
// Copyright 2009 Thibaut GRIDEL <tgridel@free.fr>
//

#include "GPXdescTagHandler.h"

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
GPX_DEFINE_TAG_HANDLER(desc)

GeoNode* GPXdescTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_desc));
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt)
        || parentItem.represents(gpxTag_trk)
        || parentItem.represents(gpxTag_rtept))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QString desc = placemark->description();
        if (!desc.isEmpty())
        {
            desc.append("<br/>");
        }
        placemark->setDescription(desc.append(parser.readElementText().trimmed().replace("\n","\n<br/>")));
        placemark->setDescriptionCDATA(true);

#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_desc << "> : " << placemark->description();
#endif
    }
    else if (parentItem.represents(gpxTag_rte))
    {
        GeoDataFeature* route = parentItem.nodeAs<GeoDataFeature>();
        QString desc = route->description();
        if (!desc.isEmpty())
        {
            desc.append("<br/>");
        }
        route->setDescription(desc.append(parser.readElementText().trimmed().replace("\n","\n<br/>")));
        route->setDescriptionCDATA(true);
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << gpxTag_desc << "> : " << route->description();
#endif
    }
    return 0;
}

} // namespace gpx

} // namespace Marble
