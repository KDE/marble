// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXlinkTagHandler.h"

#include "GPXElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoParser.h"

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
GeoNode *GPXlinkTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(gpxTag_link)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt)) {
        auto placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QXmlStreamAttributes attributes = parser.attributes();
        QString href = attributes.value(QLatin1StringView("href")).toString();
        QString text = href;
        if (parser.readNextStartElement()) {
            text = parser.readElementText();
        }

        const QString link = QStringLiteral("Link: <a href=\"%1\">%2</a>").arg(href).arg(text);

        QString desc = placemark->description();
        if (!desc.isEmpty()) {
            desc += QLatin1StringView("<br/>");
        }

        placemark->setDescription(desc.append(link));
        placemark->setDescriptionCDATA(true);
    }

    return nullptr;
}

} // namespace gpx

} // namespace Marble
