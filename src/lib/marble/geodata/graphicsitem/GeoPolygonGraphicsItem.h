//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_GEOPOLYGONGRAPHICSITEM_H
#define MARBLE_GEOPOLYGONGRAPHICSITEM_H

#include "GeoGraphicsItem.h"
#include "marble_export.h"
#include <QImage>
#include <QColor>

class QPointF;

namespace Marble
{

class GeoDataLinearRing;
class GeoDataPolygon;

class MARBLE_EXPORT GeoPolygonGraphicsItem : public GeoGraphicsItem
{
public:
    explicit GeoPolygonGraphicsItem( const GeoDataFeature *feature, const GeoDataPolygon* polygon );
    explicit GeoPolygonGraphicsItem( const GeoDataFeature *feature, const GeoDataLinearRing* ring );

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

    void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer);

private:
    void paintFrame( GeoPainter* painter, const ViewportParams *viewport );
    void paintRoof( GeoPainter* painter, const ViewportParams *viewport );

    QPointF buildingOffset(const QPointF &point, const ViewportParams *viewport, bool* isCameraAboveBuilding=0) const;
    void extractBuildingHeight();
    void screenPolygons(const ViewportParams *viewport, const GeoDataPolygon* polygon, QVector<QPolygonF*> &polygons,  QVector<QPolygonF*> &outlines) const;
    QPen configurePainter(GeoPainter* painter, const ViewportParams *viewport, bool isBuildingFrame);
    void determineBuildingHeight();
    void initializeBuildingPainting(const GeoPainter* painter, const ViewportParams *viewport,
                                    bool &drawAccurate3D, bool &isCameraAboveBuilding, bool &hasInnerBoundaries,
                                    QVector<QPolygonF*>& outlinePolygons,
                                    QVector<QPolygonF*>& innerPolygons) const;
    QPointF centroid(const QPolygonF &polygon) const;

    const GeoDataPolygon *const m_polygon;
    const GeoDataLinearRing *const m_ring;
    double m_buildingHeight;
    QString m_buildingLabel;
    QString m_cachedTexturePath;
    QColor m_cachedTextureColor;
    QImage m_cachedTexture;
};

}

#endif
