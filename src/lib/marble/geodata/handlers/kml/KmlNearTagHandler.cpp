// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlNearTagHandler.h"

#include "GeoDataParser.h"
#include "GeoDataViewVolume.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(near)

GeoNode *KmlnearTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_near)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ViewVolume)) {
        qreal near = parser.readElementText().toDouble();

        parentItem.nodeAs<GeoDataViewVolume>()->setNear(near);
    }
    return nullptr;
}

}
}
