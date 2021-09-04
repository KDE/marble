// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoPolygonGraphicsItem.h"

#include "BuildingGraphicsItem.h"
#include "GeoDataPlacemark.h"
#include "StyleBuilder.h"

namespace Marble
{

AbstractGeoPolygonGraphicsItem *GeoPolygonGraphicsItem::createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon)
{
    return new GeoPolygonGraphicsItem(placemark, polygon);
}

AbstractGeoPolygonGraphicsItem *GeoPolygonGraphicsItem::createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring)
{
    return new GeoPolygonGraphicsItem(placemark, ring);
}

AbstractGeoPolygonGraphicsItem *GeoPolygonGraphicsItem::createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataBuilding *building)
{
    return new BuildingGraphicsItem(placemark, building);
}

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon) :
    AbstractGeoPolygonGraphicsItem(placemark, polygon)
{
    const int elevation = extractElevation(*placemark);
    setZValue(zValue() + elevation);

    const GeoDataPlacemark::GeoDataVisualCategory visualCategory = placemark->visualCategory();
    const QString paintLayer = QLatin1String("Polygon/") + StyleBuilder::visualCategoryName(visualCategory);
    setPaintLayers(QStringList(paintLayer));
}

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring) :
    AbstractGeoPolygonGraphicsItem(placemark, ring)
{
    const int elevation = extractElevation(*placemark);
    setZValue(zValue() + elevation);

    const GeoDataPlacemark::GeoDataVisualCategory visualCategory = placemark->visualCategory();
    const QString paintLayer = QLatin1String("Polygon/") + StyleBuilder::visualCategoryName(visualCategory);
    setPaintLayers(QStringList(paintLayer));
}

}
