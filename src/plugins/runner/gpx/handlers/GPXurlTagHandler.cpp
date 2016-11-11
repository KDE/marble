//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXurlTagHandler.h"

#include "MarbleDebug.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER_10(url)

// Gpx 1.0 url element provides a link related to a waypoint, and is
// inserted into the waypoint description as this seems to be the
// simplest means to make it available to the user.
// In addition, url properties are saved to extendedData.
// The insertion is done in the urlname element, which is the link text.
GeoNode* GPXurlTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_url)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QString url = parser.readElementText().trimmed();

        GeoDataExtendedData extendedData = placemark->extendedData();
        extendedData.addValue(GeoDataData(QStringLiteral("url"), url));
        placemark->setExtendedData(extendedData);
    }

    return 0;
}

} // namespace gpx

} // namespace Marble
