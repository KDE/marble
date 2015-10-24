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

    virtual void paint( GeoPainter* painter, const ViewportParams *viewport );

protected:
    virtual void createDecorations();

private:
    QPointF buildingOffset(const QPointF &point, const ViewportParams *viewport, bool* isCameraAboveBuilding=0) const;
    double extractBuildingHeight(double defaultValue) const;
    void screenPolygons(const ViewportParams *viewport, const GeoDataPolygon* polygon, QVector<QPolygonF*> &polygons,  QVector<QPolygonF> &outlines);

    const GeoDataPolygon *const m_polygon;
    const GeoDataLinearRing *const m_ring;
    static const float s_decorationZValue;
    double m_buildingHeight;
    QString m_cachedTexturePath;
    QColor m_cachedTextureColor;
    QImage m_cachedTexture;
};

}

#endif
