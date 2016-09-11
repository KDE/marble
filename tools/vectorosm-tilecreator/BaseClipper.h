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

#include "GeoDataLinearRing.h"
#include "GeoDataLatLonBox.h"

#include <QPointF>
#include <QPolygonF>

namespace Marble {

class BaseClipper
{
public:
    BaseClipper();

    template<class T>
    static QPolygonF toQPolygon(const T &lineString, bool reverseOrder = false)
    {
        QPolygonF polygon;
        if(!reverseOrder) {
            foreach (const Marble::GeoDataCoordinates& coord, lineString) {
                // Need to flip the Y axis(latitude)
                QPointF point(coord.longitude(), -coord.latitude());
                polygon.append(point);
            }
        } else {
            for(int i = lineString.size()-1; i >= 0; --i) {
                // Need to flip the Y axis(latitude)
                QPointF point(lineString.at(i).longitude(), -lineString.at(i).latitude());
                polygon.append(point);
            }
        }

        return polygon;
    }

    template<class T>
    static T toLineString(const QPolygonF& polygon, bool reverseOrder = false)
    {
        T lineString;

        if(!reverseOrder) {
            foreach (const QPointF& point, polygon) {
                // Flipping back the Y axis
                Marble::GeoDataCoordinates coord(point.x(), -point.y());
                lineString.append(coord);
            }
        } else {
            for(int i = polygon.size()-1; i >= 0; --i) {
                // Need to flip the Y axis(latitude)
                Marble::GeoDataCoordinates coord(polygon.at(i).x(), -polygon.at(i).y());
                lineString.append(coord);
            }
        }

        return lineString;
    }

    void initClipRect(const Marble::GeoDataLatLonBox& clippingBox, int pointsToAddAtEdges);

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

}

#endif // BASECLIPPER_H
