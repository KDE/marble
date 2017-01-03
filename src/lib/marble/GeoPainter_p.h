//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2009 Bastian Holst <bastianholst@gmx.de>

#ifndef MARBLE_GEOPAINTERPRIVATE_H
#define MARBLE_GEOPAINTERPRIVATE_H

#include "MarbleGlobal.h"
//#include "GeoPainter.h"

class QPolygonF;
class QSizeF;
class QPainterPath;
class QRectF;

namespace Marble
{

class ViewportParams;
class GeoDataCoordinates;
class GeoPainter;

class GeoPainterPrivate
{
 public:
    GeoPainterPrivate( GeoPainter* q, const ViewportParams *viewport, MapQuality mapQuality );

    ~GeoPainterPrivate();


    static void createAnnotationLayout ( qreal x, qreal y,
                                         const QSizeF& bubbleSize,
                                         qreal bubbleOffsetX, qreal bubbleOffsetY,
                                         qreal xRnd, qreal yRnd,
                                         QPainterPath& path, QRectF& rect );

    static GeoDataLinearRing createLinearRingFromGeoRect( const GeoDataCoordinates & centerCoordinates,
                                                          qreal width, qreal height );

    static bool doClip( const ViewportParams *viewport );

    static qreal normalizeAngle(qreal angle);

    void drawTextRotated( const QPointF &startPoint, qreal angle, const QString &text );

    const ViewportParams *const m_viewport;
    const MapQuality       m_mapQuality;
    qreal             *const m_x;

private:
    GeoPainter* m_parent;
};

} // namespace Marble

#endif
