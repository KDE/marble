// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//
#include "KmlLookAtTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataFeature.h"
#include "GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{

KML_DEFINE_TAG_HANDLER(LookAt)

GeoNode *KmlLookAtTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_LookAt)));

    auto lookAt = new GeoDataLookAt();
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataFeature>()) {
        auto feature = parentItem.nodeAs<GeoDataFeature>();
        feature->setAbstractView(lookAt);
        return lookAt;
    }
    if (parentItem.is<GeoDataFlyTo>()) {
        auto feature = parentItem.nodeAs<GeoDataFlyTo>();
        feature->setView(lookAt);
        return lookAt;
    } else {
        delete lookAt;
        return nullptr;
    }
}

}
}
