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

#include <QtCore/QVector>

#include "global.h"

class QPolygonF;
class QSizeF;
class QPainterPath;
class QRectF;

namespace Marble
{

class ViewportParams;
class GeoDataCoordinates;

class GeoPainterPrivate
{
 public:
    GeoPainterPrivate( ViewportParams *viewport, MapQuality mapQuality );

    ~GeoPainterPrivate();


    void createPolygonsFromLineString( const GeoDataLineString & lineString,
                                       QVector<QPolygonF *> &polygons );


    void createPolygonsFromLinearRing( const GeoDataLinearRing & linearRing,
                                       QVector<QPolygonF *> &polygons );


    void createAnnotationLayout ( qreal x, qreal y,
                                  QSizeF bubbleSize,
                                  qreal bubbleOffsetX, qreal bubbleOffsetY,
                                  qreal xRnd, qreal yRnd,
                                  QPainterPath& path, QRectF& rect );

    GeoDataLinearRing createLinearRingFromGeoRect( const GeoDataCoordinates & centerCoordinates,
                                                   qreal width, qreal height );

    ViewportParams  *m_viewport;
    MapQuality       m_mapQuality;
    qreal             *m_x;
};

} // namespace Marble

#endif
