//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#ifndef BASECLIPPER_H
#define BASECLIPPER_H

#include <QPointF>
#include <QPolygonF>

#include "GeoDataLinearRing.h"
#include "GeoDataLatLonBox.h"

using namespace Marble;

class BaseClipper
{
public:
    BaseClipper();

    static qreal tileX2lon( unsigned int x, unsigned int maxTileX );
    static qreal tileY2lat( unsigned int y, unsigned int maxTileY );

    static QPolygonF lineString2Qpolygon(const GeoDataLineString &lineString, bool reverseOrder = false);
    static QPolygonF linearRing2Qpolygon(const GeoDataLinearRing &linearRing, bool reverseOrder = false);

    static GeoDataLineString qPolygon2lineString(const QPolygonF& polygon, bool reverseOrder = false);
    static GeoDataLinearRing qPolygon2linearRing(const QPolygonF& polygon, bool reverseOrder = false);


    void initClipRect(const GeoDataLatLonBox& clippingBox, int pointsToAddAtEdges);

    void clipPolyObject ( const QPolygonF & sourcePolygon,
                          QVector<QPolygonF> & clippedPolyObjects,
                          bool isClosed );

private:

    int sector( const QPointF & point ) const;
    int borderSector( const QPointF & point ) const;
    bool isCornerPoint (const QPointF & point) const;

    QPointF clipTop( qreal m, const QPointF & point ) const;
    QPointF clipLeft( qreal m, const QPointF & point ) const;
    QPointF clipBottom( qreal m, const QPointF & point ) const;
    QPointF clipRight( qreal m, const QPointF & point ) const;


    void clipMultiple( QPolygonF & clippedPolyObject,
                              QVector<QPolygonF> & clippedPolyObjects,
                              bool isClosed );
    void clipOnce( QPolygonF & clippedPolyObject,
                              QVector<QPolygonF> & clippedPolyObjects,
                              bool isClosed );
    void clipOnceCorner( QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                const QPointF& corner,
                                const QPointF& point,
                                bool isClosed );
    void clipOnceEdge(   QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                const QPointF& point,
                                bool isClosed );

    static inline qreal _m( const QPointF & start, const QPointF & end );


    qreal  m_left;
    qreal  m_right;
    qreal  m_top;
    qreal  m_bottom;

    QPointF m_topLeft;
    QPointF m_topRight;
    QPointF m_bottomRight;
    QPointF m_bottomLeft;

    QPolygonF m_topEdge;
    QPolygonF m_bottomEdge;
    QPolygonF m_leftEdge;
    QPolygonF m_rightEdge;

    QPolygonF m_viewport;

    int     m_currentSector;
    int     m_previousSector;

    QPointF    m_currentPoint;
    QPointF    m_previousPoint;

    bool m_clippedTwice;
};

#endif // BASECLIPPER_H
