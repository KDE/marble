/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlPointTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Point)

GeoNode *KmlPointTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Point)));
    // FIXME: there needs to be a check that a coordinates subtag is contained

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(kmlTag_Placemark)) {
        return parentItem.nodeAs<GeoDataPlacemark>();

    } else if (parentItem.represents(kmlTag_MultiGeometry)) {
        auto point = new GeoDataPoint;
        KmlObjectTagHandler::parseIdentifiers(parser, point);
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(point);
        return point;
    } else if (parentItem.represents(kmlTag_PhotoOverlay)) {
        GeoDataPoint *point = &parentItem.nodeAs<GeoDataPhotoOverlay>()->point();
        KmlObjectTagHandler::parseIdentifiers(parser, point);
        return point;
    }
    return nullptr;
}

}
}
