//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_BUILDINGGEOPOLYGONGRAPHICSITEM_H
#define MARBLE_BUILDINGGEOPOLYGONGRAPHICSITEM_H

#include "AbstractGeoPolygonGraphicsItem.h"
#include "GeoDataCoordinates.h"

class QPointF;

namespace Marble
{

class MARBLE_EXPORT BuildingGeoPolygonGraphicsItem : public AbstractGeoPolygonGraphicsItem
{
public:
    explicit BuildingGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon);
    explicit BuildingGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring);

public:
    virtual void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer);

private:
    struct NamedEntry {
        GeoDataCoordinates point;
        QString label;
    };

    void paintFrame(GeoPainter* painter, const ViewportParams *viewport);
    void paintRoof(GeoPainter* painter, const ViewportParams *viewport);
    void configureFramePainter(GeoPainter *painter) const;
    void initializeBuildingPainting(const GeoPainter* painter, const ViewportParams *viewport,
                                    bool &drawAccurate3D, bool &isCameraAboveBuilding, bool &hasInnerBoundaries,
                                    QVector<QPolygonF*>& outlinePolygons,
                                    QVector<QPolygonF*>& innerPolygons) const;
    QPointF buildingOffset(const QPointF &point, const ViewportParams *viewport, bool* isCameraAboveBuilding = nullptr) const;

    static QPointF centroid(const QPolygonF &polygon, double &area);
    static void screenPolygons(const ViewportParams *viewport, const GeoDataPolygon* polygon,
                               QVector<QPolygonF*> &polygons,  QVector<QPolygonF*> &outlines);
    static QVector<NamedEntry> extractNamedEntries(const GeoDataPlacemark &placemark);

private:
    const double m_buildingHeight;
    const QVector<NamedEntry> m_entries;
};

}

#endif
