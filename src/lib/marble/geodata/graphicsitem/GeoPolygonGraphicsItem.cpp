//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoPolygonGraphicsItem.h"

#include "BuildingGeoPolygonGraphicsItem.h"
#include "GeoDataPlacemark.h"
#include "StyleBuilder.h"

namespace Marble
{

AbstractGeoPolygonGraphicsItem *GeoPolygonGraphicsItem::createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon)
{
    if (placemark->visualCategory() == GeoDataPlacemark::Building) {
        return new BuildingGeoPolygonGraphicsItem(placemark, polygon);
    }
    return new GeoPolygonGraphicsItem(placemark, polygon);
}

AbstractGeoPolygonGraphicsItem *GeoPolygonGraphicsItem::createGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring)
{
    if (placemark->visualCategory() == GeoDataPlacemark::Building) {
        return new BuildingGeoPolygonGraphicsItem(placemark, ring);
    }
    return new GeoPolygonGraphicsItem(placemark, ring);
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
