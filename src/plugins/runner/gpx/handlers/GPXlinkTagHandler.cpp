//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXlinkTagHandler.h"

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
GPX_DEFINE_TAG_HANDLER_11(link)

// Gpx link provides a linke related to the object, and is inserted into the
// waypoint description as this seems to be the simplest means to make it
// available to the user.
// In addition, link properties are saved to extendedData.
// there are text and type properties, type being ignored for now.
GeoNode* GPXlinkTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_link)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QXmlStreamAttributes attributes = parser.attributes();
        QString href = attributes.value(QLatin1String("href")).toString();
        QString text = href;
        if (parser.readNextStartElement())
        {
            text = parser.readElementText();
        }

        const QString link = QStringLiteral("Link: <a href=\"%1\">%2</a>")
            .arg(href).arg(text);

        QString desc = placemark->description();
        if (!desc.isEmpty())
        {
            desc += QLatin1String("<br/>");
        }

        placemark->setDescription(desc.append(link));
        placemark->setDescriptionCDATA(true);
    }

    return 0;
}

} // namespace gpx

} // namespace Marble
