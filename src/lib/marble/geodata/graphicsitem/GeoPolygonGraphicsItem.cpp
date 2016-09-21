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
#include "StyleBuilder.h"

namespace Marble
{

AbstractGeoPolygonGraphicsItem* GeoPolygonGraphicsItem::createGraphicsItem(const GeoDataFeature *feature, const GeoDataPolygon *polygon)
{
    if (feature->visualCategory() == GeoDataFeature::Building) {
        return new BuildingGeoPolygonGraphicsItem(feature, polygon);
    }
    return new GeoPolygonGraphicsItem(feature, polygon);
}

AbstractGeoPolygonGraphicsItem* GeoPolygonGraphicsItem::createGraphicsItem(const GeoDataFeature *feature, const GeoDataLinearRing *ring)
{
    if (feature->visualCategory() == GeoDataFeature::Building) {
        return new BuildingGeoPolygonGraphicsItem(feature, ring);
    }
    return new GeoPolygonGraphicsItem(feature, ring);
}


GeoPolygonGraphicsItem::GeoPolygonGraphicsItem(const GeoDataFeature *feature, const GeoDataPolygon *polygon) :
    AbstractGeoPolygonGraphicsItem(feature, polygon)
{
    const int elevation = extractElevation(*feature);
    setZValue(zValue() + elevation);

    const GeoDataFeature::GeoDataVisualCategory visualCategory = feature->visualCategory();
    const QString paintLayer = QLatin1String("Polygon/") + StyleBuilder::visualCategoryName(visualCategory);
    setPaintLayers(QStringList(paintLayer));
}

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem(const GeoDataFeature *feature, const GeoDataLinearRing *ring) :
    AbstractGeoPolygonGraphicsItem(feature, ring)
{
    const int elevation = extractElevation(*feature);
    setZValue(zValue() + elevation);

    const GeoDataFeature::GeoDataVisualCategory visualCategory = feature->visualCategory();
    const QString paintLayer = QLatin1String("Polygon/") + StyleBuilder::visualCategoryName(visualCategory);
    setPaintLayers(QStringList(paintLayer));
}

}
