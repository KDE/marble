// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "KmldrawOrderTagHandler.h"

#include "GeoDataOverlay.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(drawOrder)

GeoNode *KmldrawOrderTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_drawOrder)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataOverlay>()) {
        bool ok = false;
        int value = parser.readElementText().trimmed().toInt(&ok, 10);
        if (ok) {
            parentItem.nodeAs<GeoDataOverlay>()->setDrawOrder(value);
        }
    }

    return nullptr;
}

} // namespace kml
} // namespace Marble
