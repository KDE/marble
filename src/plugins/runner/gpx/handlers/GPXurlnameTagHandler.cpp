//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXurlnameTagHandler.h"

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
GPX_DEFINE_TAG_HANDLER_10(urlname)

// completes gpx 1.0 url element
GeoNode* GPXurlnameTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_urlname)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();
        QString text = parser.readElementText().trimmed();
// no reason to save for now!
//         GeoDataExtendedData extendedData = placemark->extendedData();
//         extendedData.addValue(GeoDataData("urlname", text));
//         placemark->setExtendedData(extendedData);

        const QString link = QStringLiteral("<br/>Link: <a href=\"%1\">%2</a>")
            .arg(placemark->extendedData().value(QStringLiteral("url")).value().toString())
            .arg(text);
        placemark->setDescription(placemark->description().append(link));
        placemark->setDescriptionCDATA(true);
    }
    return 0;
}

} // namespace gpx

} // namespace Marble
