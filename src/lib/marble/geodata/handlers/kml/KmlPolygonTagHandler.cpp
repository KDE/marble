/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlPolygonTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Polygon)

GeoNode *KmlPolygonTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Polygon)));

    GeoStackItem parentItem = parser.parentElement();

    auto polygon = new GeoDataPolygon;
    KmlObjectTagHandler::parseIdentifiers(parser, polygon);

    if (parentItem.represents(kmlTag_Placemark)) {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry(polygon);
        return parentItem.nodeAs<GeoDataPlacemark>()->geometry();

    } else if (parentItem.represents(kmlTag_MultiGeometry)) {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(polygon);
        return polygon;
    } else {
        delete polygon;
        return nullptr;
    }
}

}
}
