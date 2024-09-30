// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlRotationTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLatLonBox.h"
#include "GeoDataParser.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(rotation)

GeoNode *KmlrotationTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_rotation)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ScreenOverlay)) {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataScreenOverlay>()->setRotation(rotation);
    } else if (parentItem.represents(kmlTag_LatLonBox)) {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataLatLonBox>()->setRotation(rotation * DEG2RAD);
    } else if (parentItem.represents(kmlTag_PhotoOverlay)) {
        qreal rotation = parser.readElementText().toFloat();

        parentItem.nodeAs<GeoDataPhotoOverlay>()->setRotation(rotation);
    }
    return nullptr;
}

}
}
