//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXcmtTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"

// type is used in wpt, rte and trk to provide some sort of keyword, like "Geocache/traditional cache",
// and in link to provide a mimetype

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(cmt)


GeoNode* GPXcmtTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(gpxTag_cmt));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QString cmt = parser.readElementText().trimmed();
        if (!cmt.isEmpty())
        {
            QString desc = placemark->description();
            if (!desc.isEmpty())
            {
                desc.append("<br/>");
            }
            placemark->setDescription(desc.append(cmt.replace("\n", "\n<br/>")));
            placemark->setDescriptionCDATA(true);
        }
    }
    return 0;
}

} // namespace gpx

} // namespace Marble
