/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlLinearRingTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLinearRing.h"
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
KML_DEFINE_TAG_HANDLER(LinearRing)

GeoNode *KmlLinearRingTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_LinearRing)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_outerBoundaryIs)) {
        GeoDataLinearRing linearRing;
        KmlObjectTagHandler::parseIdentifiers(parser, &linearRing);
        parentItem.nodeAs<GeoDataPolygon>()->setOuterBoundary(linearRing);
        return &parentItem.nodeAs<GeoDataPolygon>()->outerBoundary();

    } else if (parentItem.represents(kmlTag_innerBoundaryIs)) {
        GeoDataLinearRing linearRing;
        KmlObjectTagHandler::parseIdentifiers(parser, &linearRing);
        parentItem.nodeAs<GeoDataPolygon>()->appendInnerBoundary(linearRing);
        return &parentItem.nodeAs<GeoDataPolygon>()->innerBoundaries().last();

    } else if (parentItem.represents(kmlTag_Placemark)) {
        auto linearRing = new GeoDataLinearRing;
        KmlObjectTagHandler::parseIdentifiers(parser, linearRing);
        auto placemark = parentItem.nodeAs<GeoDataPlacemark>();
        placemark->setGeometry(linearRing);
        return placemark->geometry();

    } else if (parentItem.is<GeoDataMultiGeometry>()) {
        auto linearRing = new GeoDataLinearRing;
        KmlObjectTagHandler::parseIdentifiers(parser, linearRing);
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(linearRing);
        return linearRing;
    } else
        return nullptr;
}

}
}
