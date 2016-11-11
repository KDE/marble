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
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(desc)

GeoNode* GPXdescTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_desc)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt)
        || parentItem.represents(gpxTag_trk)
        || parentItem.represents(gpxTag_rtept))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QString desc = placemark->description();
        if (!desc.isEmpty())
        {
            desc += QLatin1String("<br/>");
        }
        placemark->setDescription(desc.append(parser.readElementText().trimmed().replace(QLatin1Char('\n'),QLatin1String("\n<br/>"))));
        placemark->setDescriptionCDATA(true);

    }
    else if (parentItem.represents(gpxTag_rte))
    {
        GeoDataFeature* route = parentItem.nodeAs<GeoDataFeature>();
        QString desc = route->description();
        if (!desc.isEmpty())
        {
            desc += QLatin1String("<br/>");
        }
        route->setDescription(desc.append(parser.readElementText().trimmed().replace(QLatin1Char('\n'),QLatin1String("\n<br/>"))));
        route->setDescriptionCDATA(true);
    }
    return 0;
}

} // namespace gpx

} // namespace Marble
