// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_BUILDINGGRAPHICSITEM_H
#define MARBLE_BUILDINGGRAPHICSITEM_H

#include "AbstractGeoPolygonGraphicsItem.h"
#include "GeoDataCoordinates.h"

class QPointF;

namespace Marble
{

class MARBLE_EXPORT BuildingGraphicsItem : public AbstractGeoPolygonGraphicsItem
{
public:
    BuildingGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataBuilding *building);
    ~BuildingGraphicsItem() override;

public:
    void paint(GeoPainter *painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel) override;

private:
    void paintFrame(GeoPainter *painter, const ViewportParams *viewport);

    void paintRoof(GeoPainter *painter, const ViewportParams *viewport);
    bool configurePainterForFrame(GeoPainter *painter) const;
    void initializeBuildingPainting(const GeoPainter *painter, const ViewportParams *viewport, bool &drawAccurate3D, bool &isCameraAboveBuilding) const;
    void updatePolygons(const ViewportParams &viewport, QList<QPolygonF *> &outlinePolygons, QList<QPolygonF *> &innerPolygons, bool &hasInnerBoundaries) const;

    QPointF buildingOffset(const QPointF &point, const ViewportParams *viewport, bool *isCameraAboveBuilding = nullptr) const;

    static QPointF centroid(const QPolygonF &polygon, double &area);
    static void screenPolygons(const ViewportParams &viewport, const GeoDataPolygon *polygon, QList<QPolygonF *> &polygons, QList<QPolygonF *> &outlines);

    bool contains(const QPoint &screenPosition, const ViewportParams *viewport) const override;

private:
    QList<QPolygonF *> m_cachedOuterPolygons;
    QList<QPolygonF *> m_cachedInnerPolygons;
    QList<QPolygonF *> m_cachedOuterRoofPolygons;
    QList<QPolygonF *> m_cachedInnerRoofPolygons;
    bool m_hasInnerBoundaries;
};

}

#endif
