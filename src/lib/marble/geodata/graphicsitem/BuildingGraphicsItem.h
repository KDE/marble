//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
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
    void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel) override;

private:
    void paintFrame(GeoPainter* painter, const ViewportParams *viewport);

    void paintRoof(GeoPainter* painter, const ViewportParams *viewport);
    bool configurePainterForFrame(GeoPainter *painter) const;
    void initializeBuildingPainting(const GeoPainter* painter, const ViewportParams *viewport,
                                    bool &drawAccurate3D, bool &isCameraAboveBuilding) const;
    void updatePolygons(const ViewportParams &viewport,
                         QVector<QPolygonF*>& outlinePolygons,
                         QVector<QPolygonF*>& innerPolygons,
                         bool &hasInnerBoundaries) const;

    QPointF buildingOffset(const QPointF &point, const ViewportParams *viewport, bool* isCameraAboveBuilding = nullptr) const;

    static QPointF centroid(const QPolygonF &polygon, double &area);
    static void screenPolygons(const ViewportParams &viewport, const GeoDataPolygon *polygon,
                               QVector<QPolygonF*> &polygons,
                               QVector<QPolygonF*> &outlines
                               );

    bool contains(const QPoint &screenPosition, const ViewportParams *viewport) const override;

private:
    QVector<QPolygonF*> m_cachedOuterPolygons;
    QVector<QPolygonF*> m_cachedInnerPolygons;
    QVector<QPolygonF*> m_cachedOuterRoofPolygons;
    QVector<QPolygonF*> m_cachedInnerRoofPolygons;
    bool m_hasInnerBoundaries;

};

}

#endif
