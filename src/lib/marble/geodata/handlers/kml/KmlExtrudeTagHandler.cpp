/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
    SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlExtrudeTagHandler.h"
#include "KmlElementDictionary.h"

#include "GeoDataGeometry.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(extrude)

GeoNode *KmlextrudeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_extrude)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataGeometry *geometry;
    bool validParents = false;

    if (parentItem.is<GeoDataPoint>()) {
        geometry = parentItem.nodeAs<GeoDataPoint>();
        validParents = true;
    } else if (parentItem.is<GeoDataPlacemark>()) {
        geometry = parentItem.nodeAs<GeoDataPlacemark>()->geometry();
        validParents = true;
    } else if (parentItem.is<GeoDataPolygon>()) {
        geometry = parentItem.nodeAs<GeoDataPolygon>();
        validParents = true;
    } else if (parentItem.is<GeoDataLineString>()) {
        geometry = parentItem.nodeAs<GeoDataLineString>();
        validParents = true;
    } else if (parentItem.is<GeoDataLinearRing>()) {
        geometry = parentItem.nodeAs<GeoDataLinearRing>();
        validParents = true;
    }

    if (validParents) {
        QString content = parser.readElementText().trimmed();

        const bool extrude = (content == QLatin1StringView("1"));
        geometry->setExtrude(extrude);
    }

    return nullptr;
}

}
}
