// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlTopFovTagHandler.h"

#include "GeoDataParser.h"
#include "GeoDataViewVolume.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(topFov)

GeoNode *KmltopFovTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_topFov)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ViewVolume)) {
        qreal topFov = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataViewVolume>()->setTopFov(topFov);
    }
    return nullptr;
}

}
}
