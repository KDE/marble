// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlRotationXYTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataParser.h"
#include "GeoDataScreenOverlay.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(rotationXY)

GeoNode *KmlrotationXYTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_rotationXY)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ScreenOverlay)) {
        GeoDataVec2 vec2(parser.attribute("x").trimmed().toFloat(),
                         parser.attribute("y").trimmed().toFloat(),
                         parser.attribute("xunits").trimmed(),
                         parser.attribute("yunits").trimmed());

        parentItem.nodeAs<GeoDataScreenOverlay>()->setRotationXY(vec2);
    }
    return nullptr;
}

}
}
