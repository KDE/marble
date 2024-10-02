// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//
#include "KmlDurationTagHandler.h"

#include "KmlElementDictionary.h"

#include "GeoDataAnimatedUpdate.h"
#include "GeoDataFlyTo.h"
#include "GeoDataWait.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22(duration)

GeoNode *KmldurationTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_duration)));

    GeoStackItem parentItem = parser.parentElement();

    qreal const duration = parser.readElementText().trimmed().toDouble();
    if (parentItem.is<GeoDataFlyTo>()) {
        parentItem.nodeAs<GeoDataFlyTo>()->setDuration(duration);
    }
    if (parentItem.is<GeoDataWait>()) {
        parentItem.nodeAs<GeoDataWait>()->setDuration(duration);
    }
    if (parentItem.is<GeoDataAnimatedUpdate>()) {
        parentItem.nodeAs<GeoDataAnimatedUpdate>()->setDuration(duration);
    }
    return nullptr;
}

}
}
