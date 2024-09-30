// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlRegionTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataFeature.h"
#include "GeoDataRegion.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Region)

GeoNode *KmlRegionTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Region)));

    GeoDataRegion region;
    KmlObjectTagHandler::parseIdentifiers(parser, &region);

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>()) {
        parentItem.nodeAs<GeoDataFeature>()->setRegion(region);
        return &parentItem.nodeAs<GeoDataFeature>()->region();
    } else {
        return nullptr;
    }
}

}
}
