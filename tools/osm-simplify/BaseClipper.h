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

#include "GeoDataLinearRing.h"
#include "GeoDataLatLonBox.h"

using namespace Marble;

class BaseClipper
{
public:
    BaseClipper();

    static qreal tileX2lon( unsigned int x, unsigned int maxTileX );
    static qreal tileY2lat( unsigned int y, unsigned int maxTileY );

    static QPolygonF* lineString2Qpolygon(const GeoDataLineString &lineString);
    static QPolygonF* linearRing2Qpolygon(const GeoDataLinearRing &linearRing);

    static GeoDataLineString* qPolygon2lineString(const QPolygonF& polygon);
    static GeoDataLinearRing* qPolygon2linearRing(const QPolygonF& polygon);


    void initClipRect(const GeoDataLatLonBox& clippingBox);

    void clipPolyObject ( const QPolygonF & sourcePolygon,
                          QVector<QPolygonF> & clippedPolyObjects,
                          bool isClosed );

private:

    inline int sector( const QPointF & point ) const;

    inline QPointF clipTop( qreal m, const QPointF & point ) const;
    inline QPointF clipLeft( qreal m, const QPointF & point ) const;
    inline QPointF clipBottom( qreal m, const QPointF & point ) const;
    inline QPointF clipRight( qreal m, const QPointF & point ) const;


    inline void clipMultiple( QPolygonF & clippedPolyObject,
                              QVector<QPolygonF> & clippedPolyObjects,
                              bool isClosed );
    inline void clipOnce( QPolygonF & clippedPolyObject,
                              QVector<QPolygonF> & clippedPolyObjects,
                              bool isClosed );
    inline void clipOnceCorner( QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                const QPointF& corner,
                                const QPointF& point,
                                bool isClosed ) const;
    inline void clipOnceEdge(   QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                const QPointF& point,
                                bool isClosed ) const;

    static inline qreal _m( const QPointF & start, const QPointF & end );


    qreal  m_left;
    qreal  m_right;
    qreal  m_top;
    qreal  m_bottom;

    int     m_currentSector;
    int     m_previousSector;

    QPointF    m_currentPoint;
    QPointF    m_previousPoint;
};

#endif // BASECLIPPER_H
