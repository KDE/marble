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

#include "BaseClipper.h"

#include "MarbleMath.h"

#include <QPolygonF>
#include <QDebug>
#include <QSharedPointer>

namespace Marble {

class LinkedPoint {
public:
    explicit LinkedPoint(const QPointF& point, bool isEntering=false, bool isLeaving=false) : m_point(point),
        m_isEntering(isEntering), m_isLeaving(isLeaving),
        m_nextBasePolygonPoint(nullptr), m_nextClipPolygonPoint(nullptr),
        m_processed(false)
    {}

    void clear()
    {
        m_nextBasePolygonPoint.clear();
        m_nextClipPolygonPoint.clear();
    }

    inline bool isEntering() const
    {
        return m_isEntering;
    }

    inline bool isLeaving() const
    {
        return m_isLeaving;
    }

    inline bool isProcessed() const
    {
        return m_processed;
    }

    inline const QPointF& point() const
    {
        return m_point;
    }

    inline const QSharedPointer<LinkedPoint>& nextClipPolygonPoint() const
    {
        return m_nextClipPolygonPoint;
    }

    inline const QSharedPointer<LinkedPoint>& nextBasePolygonPoint() const
    {
        return m_nextBasePolygonPoint;
    }

    inline void setNextClipPolygonPoint(const QSharedPointer<LinkedPoint>& nextPoint)
    {
        m_nextClipPolygonPoint = nextPoint;
    }

    inline void setNextBasePolygonPoint(const QSharedPointer<LinkedPoint>& nextPoint)
    {
        m_nextBasePolygonPoint = nextPoint;
    }

    inline void setProcessed(bool processed)
    {
        m_processed = processed;
    }

private:

    QPointF m_point;
    bool m_isEntering;
    bool m_isLeaving;

    QSharedPointer<LinkedPoint> m_nextBasePolygonPoint;
    QSharedPointer<LinkedPoint> m_nextClipPolygonPoint;

    bool m_processed;
};

BaseClipper::BaseClipper() :
    m_left(0.0),
    m_right(0.0),
    m_top(0.0),
    m_bottom(0.0),
    m_topLeft(QPointF()),
    m_topRight(QPointF()),
    m_bottomRight(QPointF()),
    m_bottomLeft(QPointF()),
    m_viewport(QPolygonF()),
    m_currentSector(4),
    m_previousSector(4),
    m_currentPoint(QPointF()),
    m_previousPoint(QPointF()),
    m_clippedTwice(false)
{

}

void BaseClipper::initClipRect (const GeoDataLatLonBox &clippingBox, int pointsToAddAtEdges)
{
    m_left   = clippingBox.west();
    m_right  = clippingBox.east();

    // Had to flip the 'Y' axis, because the origo of the coordinate system in which the
    // geographics coordinates are is based in the bottom left corner, while the
    // screencoordinatas on which this clipper operated are in an upper left corner based origo.

    m_top    = -clippingBox.north();
    m_bottom = -clippingBox.south();

    m_topLeft = QPointF(m_left, m_top);
    m_topRight = QPointF(m_right, m_top);
    m_bottomRight = QPointF(m_right, m_bottom);
    m_bottomLeft = QPointF(m_left, m_bottom);

    qreal x, y;
    qreal deltaX = fabs(m_right - m_left) / (pointsToAddAtEdges + 1);
    qreal deltaY = fabs(m_bottom - m_top) / (pointsToAddAtEdges + 1);

    m_topEdge.clear();
    x = m_left;
    for(int i = 0; i < pointsToAddAtEdges; ++i) {
        x += deltaX;
        m_topEdge << QPointF(x, m_top);
    }

    m_rightEdge.clear();
    y = m_top;
    for(int i = 0; i < pointsToAddAtEdges; ++i) {
        y += deltaY;
        m_rightEdge << QPointF(m_right, y);
    }

    m_bottomEdge.clear();
    x = m_right;
    for(int i = 0; i < pointsToAddAtEdges; ++i) {
        x -= deltaX;
        m_bottomEdge << QPointF(x, m_bottom);
    }

    m_leftEdge.clear();
    y = m_bottom;
    for(int i = 0; i < pointsToAddAtEdges; ++i) {
        y -= deltaY;
        m_leftEdge << QPointF(m_left, y);
    }

    m_viewport.clear();
    m_viewport << m_topLeft << m_topRight << m_bottomRight << m_bottomLeft;
    m_viewport.isClosed();
}

qreal BaseClipper::_m( const QPointF & start, const QPointF & end )
{
    qreal  divisor = end.x() - start.x();

    // Had to add more zeros, because what was acceptable in screen coordinates
    // could become meters or hundreds of meters in geographic coordinates.
    if ( std::fabs( divisor ) < 0.00000000000000001 ) {
        divisor = 0.00000000000000001 * (divisor < 0 ? -1 : 1);
    }

    return ( end.y() - start.y() )
            / divisor;
}


QPointF BaseClipper::clipTop( qreal m, const QPointF & point ) const
{
    return QPointF( ( m_top - point.y() ) / m + point.x(), m_top );
}

QPointF BaseClipper::clipLeft( qreal m, const QPointF & point ) const
{
    return QPointF( m_left, ( m_left - point.x() ) * m + point.y() );
}

QPointF BaseClipper::clipBottom( qreal m, const QPointF & point ) const
{
    return QPointF( ( m_bottom - point.y() ) / m + point.x(), m_bottom );
}

QPointF BaseClipper::clipRight( qreal m, const QPointF & point ) const
{
    return QPointF( m_right, ( m_right - point.x() ) * m + point.y() );
}

int BaseClipper::sector( const QPointF & point ) const
{
    // If we think of the image borders as (infinitely long) parallel
    // lines then the plane is divided into 9 sectors.  Each of these
    // sections is identified by a unique keynumber (currentSector):
    //
    //  0 | 1 | 2
    //  --+---+--
    //  3 | 4 | 5 <- sector number "4" represents the onscreen sector / viewport
    //  --+---+--
    //  6 | 7 | 8
    //

    // Figure out the section of the current point.
    int xSector = 1;
    if ( point.x() < m_left )
        xSector = 0;
    else if ( point.x() > m_right )
        xSector = 2;

    int ySector = 3;
    if ( point.y() < m_top )
        ySector = 0;
    else if ( point.y() > m_bottom )
        ySector = 6;

    // By adding xSector and ySector we get a
    // sector number of the values shown in the ASCII-art graph above.
    return ySector + xSector;
}

// Determines that the point on which border or corner is exactly on, 4 otherwise.
int BaseClipper::borderSector( const QPointF & point ) const
{
    if(point.x() == m_left) {
        return 3;
    } else if (point.x() == m_right) {
        return 5;
    } else if (point.y() == m_top) {
        return 1;
    } else if (point.y() == m_bottom) {
        return 7;
    } else {
        return sector(point);
    }
}

bool BaseClipper::isCornerPoint (const QPointF & point) const {
    return point == m_topLeft || point == m_topRight ||
            point == m_bottomRight || point == m_bottomLeft;
}

void BaseClipper::clipPolyObject ( const QPolygonF & polygon,
                                   QVector<QPolygonF> & clippedPolyObjects,
                                   bool isClosed )
{
    QVector<QSharedPointer<LinkedPoint>> basePolygon;
    QVector<QSharedPointer<LinkedPoint>> clipPolygon;
    QVector<QSharedPointer<LinkedPoint>> intersections;

    QVector<QSharedPointer<LinkedPoint>> intersectionsTop;
    QVector<QSharedPointer<LinkedPoint>> intersectionsRight;
    QVector<QSharedPointer<LinkedPoint>> intersectionsBottom;
    QVector<QSharedPointer<LinkedPoint>> intersectionsLeft;

    auto appendToIntersectionKind = [&](QSharedPointer<LinkedPoint>& intersection) {
        switch(borderSector(intersection->point())) {
        case 1:
            intersectionsTop << intersection;
            break;
        case 3:
            intersectionsLeft << intersection;
            break;
        case 5:
            intersectionsRight << intersection;
            break;
        case 7:
            intersectionsBottom << intersection;
            break;
        default: break;
        }
    };

    bool intersectionAdded = false;

    // Only create a new polyObject as soon as we know for sure that
    // the current point is on the screen.
    QPolygonF clippedPolyObject = QPolygonF();

    const QVector<QPointF>::const_iterator  itStartPoint = polygon.constBegin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.constEnd();
    QVector<QPointF>::const_iterator        itPoint      = itStartPoint;

    // We use a while loop to be able to cover linestrings as well as linear rings:
    // Linear rings require to tessellate the path from the last node to the first node
    // which isn't really convenient to achieve with a for loop ...

    bool processingLastNode = false;

    // qDebug() << "\nNew polygon, size:" << polygon.size();

    while ( itPoint != itEndPoint ) {
        m_currentPoint = (*itPoint);
        // mDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the sector of the current point.
        m_currentSector = sector( m_currentPoint );

        // Initialize the variables related to the previous point.
        if ( itPoint == itStartPoint && processingLastNode == false ) {
            if ( isClosed ) {
                m_previousPoint = polygon.last();

                QSharedPointer<LinkedPoint> firstPoint = QSharedPointer<LinkedPoint>(new LinkedPoint(m_currentPoint));
                basePolygon << firstPoint;

                // Figure out the sector of the previous point.
                m_previousSector = sector( m_previousPoint );
            }
            else {
                m_previousSector = m_currentSector;
            }
        }

        // If the current point reaches a new sector, take care of clipping.
        if ( m_currentSector != m_previousSector ) {
            if ( m_currentSector == 4 || m_previousSector == 4 ) {
                // In this case the current or the previous point is visible on the
                // screen but not both. Hence we only need to clip once and require
                // only one interpolation for both cases.

                clipOnce( clippedPolyObject, clippedPolyObjects, isClosed );

                if(isClosed) {

                    if(!clippedPolyObject.isEmpty()) {

                        if(isCornerPoint(clippedPolyObject.last()) && clippedPolyObject.size() > 1) {
                            clippedPolyObject.removeLast();
                        }

                        QSharedPointer<LinkedPoint> intersection =
                                QSharedPointer<LinkedPoint>(new LinkedPoint(clippedPolyObject.last(),
                                                                            m_currentSector == 4,
                                                                            m_previousSector == 4));

                        intersections << intersection;
                        basePolygon.last()->setNextBasePolygonPoint(intersection);

                        QSharedPointer<LinkedPoint> nextPoint = QSharedPointer<LinkedPoint>(new LinkedPoint(m_currentPoint));
                        basePolygon << nextPoint;
                        intersections.last()->setNextBasePolygonPoint(nextPoint);

                        appendToIntersectionKind(intersection);

                        intersectionAdded = true;
                    }
                }
            } else {
                // This case mostly deals with lines that reach from one
                // sector that is located off screen to another one that
                // is located off screen. In this situation the line
                // can get clipped once, twice, or not at all.
                m_clippedTwice = false;
                clipMultiple( clippedPolyObject, clippedPolyObjects, isClosed );

                if(isClosed && m_clippedTwice) {
                    // qDebug() << "Clipped twice";
                    QPointF firstAddedPoint = clippedPolyObject.at(clippedPolyObject.size()-2);
                    QPointF secondAddedPoint = clippedPolyObject.last();

                    int firstAddedPointSector = borderSector(firstAddedPoint);
                    int secondAddedPointSector = borderSector(secondAddedPoint);

                    QSharedPointer<LinkedPoint> intersectionFirst = QSharedPointer<LinkedPoint>(new LinkedPoint(firstAddedPoint, firstAddedPointSector == m_previousSector, firstAddedPointSector == m_currentSector));
                    intersections << intersectionFirst;
                    QSharedPointer<LinkedPoint> intersectionSecond = QSharedPointer<LinkedPoint>(new LinkedPoint(secondAddedPoint, secondAddedPointSector == m_previousSector, secondAddedPointSector == m_currentSector));
                    intersections << intersectionSecond;

                    basePolygon.last()->setNextBasePolygonPoint(intersectionFirst);
                    intersectionFirst->setNextBasePolygonPoint(intersectionSecond);
                    QSharedPointer<LinkedPoint> nextPoint = QSharedPointer<LinkedPoint>(new LinkedPoint(m_currentPoint));
                    basePolygon << nextPoint;
                    intersectionSecond->setNextBasePolygonPoint(basePolygon.last());

                    appendToIntersectionKind(intersectionFirst);

                    appendToIntersectionKind(intersectionSecond);

                    intersectionAdded = true;
                }
            }

            m_previousSector = m_currentSector;
        }

        if (isClosed) {
            if(!intersectionAdded) {
                QSharedPointer<LinkedPoint> nextPoint = QSharedPointer<LinkedPoint>(new LinkedPoint(m_currentPoint));
                if(!basePolygon.isEmpty()) {
                    basePolygon.last()->setNextBasePolygonPoint(nextPoint);
                }
                basePolygon << nextPoint;
            } else {
                intersectionAdded = false;
            }
        }

        m_previousPoint = m_currentPoint;
        if(m_currentSector == 4) {
            clippedPolyObject << m_currentPoint;
        }
        // Now let's handle the case where we have a (closed) polygon and where the
        // last point of the polyline is outside the viewport and the start point
        // is inside the viewport. This needs special treatment
        if ( processingLastNode ) {
            break;
        }
        ++itPoint;

        if ( itPoint == itEndPoint  && isClosed ) {
            itPoint = itStartPoint;
            processingLastNode = true;
        }
    }

    if(isClosed && !basePolygon.isEmpty()) {
        basePolygon.last()->setNextBasePolygonPoint(basePolygon.first());

        if(!intersections.isEmpty()) {

            //            qDebug() << "intersections count:" << intersections.size();
            //            qDebug() << "intersectionsTop count:" << intersectionsTop.size();
            //            qDebug() << "intersectionsRight count:" << intersectionsRight.size();
            //            qDebug() << "intersectionsBottom count:" << intersectionsBottom.size();
            //            qDebug() << "intersectionsLeft count:" << intersectionsLeft.size();

            clippedPolyObjects.clear();
            clippedPolyObject = QPolygonF();

            for(const auto& point : m_topEdge) {
                intersectionsTop << QSharedPointer<LinkedPoint>(new LinkedPoint(point));
            }
            std::sort(intersectionsTop.begin(), intersectionsTop.end(), [](const QSharedPointer<LinkedPoint>& A, const QSharedPointer<LinkedPoint>& B) {
                return A->point().x() < B->point().x();
            });

            for(const auto& point : m_rightEdge) {
                intersectionsRight << QSharedPointer<LinkedPoint>(new LinkedPoint(point));
            }
            std::sort(intersectionsRight.begin(), intersectionsRight.end(), [](const QSharedPointer<LinkedPoint>& A, const QSharedPointer<LinkedPoint>& B) {
                return A->point().y() < B->point().y();
            });

            for(const auto& point : m_bottomEdge) {
                intersectionsBottom << QSharedPointer<LinkedPoint>(new LinkedPoint(point));
            }
            std::sort(intersectionsBottom.begin(), intersectionsBottom.end(), [](const QSharedPointer<LinkedPoint>& A, const QSharedPointer<LinkedPoint>& B) {
                return B->point().x() < A->point().x();
            });

            for(const auto& point : m_leftEdge) {
                intersectionsLeft << QSharedPointer<LinkedPoint>(new LinkedPoint(point));
            }
            std::sort(intersectionsLeft.begin(), intersectionsLeft.end(), [](const QSharedPointer<LinkedPoint>& A, const QSharedPointer<LinkedPoint>& B) {
                return B->point().y() < A->point().y();
            });

            clipPolygon << QSharedPointer<LinkedPoint>(new LinkedPoint(m_topLeft))     << intersectionsTop     <<
                           QSharedPointer<LinkedPoint>(new LinkedPoint(m_topRight))    << intersectionsRight   <<
                           QSharedPointer<LinkedPoint>(new LinkedPoint(m_bottomRight)) << intersectionsBottom  <<
                           QSharedPointer<LinkedPoint>(new LinkedPoint(m_bottomLeft))  << intersectionsLeft;

            for(int i = 0; i < clipPolygon.size() - 1; ++i) {
                clipPolygon[i]->setNextClipPolygonPoint(clipPolygon[i+1]);
            }
            clipPolygon.last()->setNextClipPolygonPoint(clipPolygon.first());

            bool iterateBasePolygon = false;

            for(const auto& intersection : intersections) {
                if(intersection->isEntering() && !intersection->isProcessed()) {
                    iterateBasePolygon = true;

                    QSharedPointer<LinkedPoint> it = intersection;
                    clippedPolyObject << it->point();
                    it->setProcessed(true);

                    do {
                        if(iterateBasePolygon) {
                            it = it->nextBasePolygonPoint();
                            if(it->isLeaving()) {
                                iterateBasePolygon = false;
                                it->setProcessed(true);
                            } else if(it->isEntering()) {
                                it->setProcessed(true);
                            }
                        }  else {
                            it = it->nextClipPolygonPoint();
                            if(it->isEntering()) {
                                iterateBasePolygon = true;
                                it->setProcessed(true);
                            } else if(it->isLeaving()) {
                                it->setProcessed(true);
                            }
                        }
                        clippedPolyObject << it->point();
                        Q_ASSERT(clippedPolyObject.size() <= 20 * basePolygon.size());

                        //                        // To avoid crashes because of infinite loop.
                        //                        // Needs to be investigated
                        //                        if(clippedPolyObject.size() > basePolygon.size()) {
                        //                            qDebug() << "Something went wrong, exiting current clipping loop...";
                        //                            break;
                        //                        }

                    } while(clippedPolyObject.first() != clippedPolyObject.last());

                    clippedPolyObjects << clippedPolyObject;
                    clippedPolyObject = QPolygonF();
                }
            }
        } else {
            clippedPolyObjects.clear();
            clippedPolyObject = polygon.intersected(m_viewport);

            if (clippedPolyObject == m_viewport.intersected(m_viewport)) {
                clippedPolyObject = QPolygonF();
                clippedPolyObject << m_topLeft << m_topEdge
                                  << m_topRight << m_rightEdge
                                  << m_bottomRight << m_bottomEdge
                                  << m_bottomLeft << m_leftEdge;
                clippedPolyObjects << clippedPolyObject;
            } else if(!clippedPolyObject.isEmpty()) {
                clippedPolyObjects << polygon;
            }
        }
    } else if(!clippedPolyObject.isEmpty()) {
        clippedPolyObjects << clippedPolyObject;
    }

    // Break shared pointer deadlocks. Needed to free memory, without it no LinkedPoint instance would be deleted
    typedef QVector<QSharedPointer<LinkedPoint>> Container;
    auto containers = QVector<Container*>() << &clipPolygon << &basePolygon << &intersections;
    foreach (auto const & container, containers) {
        foreach (auto const & element, *container) {
            element->clear();
        }
    }
}


void BaseClipper::clipMultiple( QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                bool isClosed )
{
    Q_UNUSED( clippedPolyObjects )
    Q_UNUSED( isClosed )

    // Take care of adding nodes in the image corners if the iterator
    // traverses off screen sections.

    int numAddedPoints = 0;

    qreal  m = _m( m_previousPoint, m_currentPoint );

    switch ( m_currentSector ) {
    case 0:
        if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
            if ( pointTop.x() >= m_left && pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointTop.x() > m_left && pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }

            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom ) {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top ) {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
        }

        clippedPolyObject << QPointF( m_left, m_top );
        numAddedPoints++;
        break;

    case 1:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 7 ) {
            clippedPolyObject << clipBottom( m, m_previousPoint );
            numAddedPoints++;
            clippedPolyObject << clipTop( m, m_currentPoint );
            numAddedPoints++;
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
        }
        break;

    case 2:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
            if ( pointTop.x() > m_left && pointTop.x() <= m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointTop.x() > m_left && pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }

            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom ) {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top ) {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
        }

        clippedPolyObject << QPointF( m_right, m_top );
        numAddedPoints++;
        break;

    case 3:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 5 ) {
            clippedPolyObject << clipRight( m, m_previousPoint );
            numAddedPoints++;
            clippedPolyObject << clipLeft( m, m_currentPoint );
            numAddedPoints++;
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointTop.x() > m_left && pointTop.x() <= m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
        }
        break;

    case 5:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 3 ) {
            clippedPolyObject << clipLeft( m, m_previousPoint );
            numAddedPoints++;
            clippedPolyObject << clipRight( m, m_currentPoint );
            numAddedPoints++;
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointTop.x() >= m_left && pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
        }
        break;

    case 6:
        if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }

            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom ) {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top ) {
                clippedPolyObject << QPointF( m_left, m_top );
                numAddedPoints++;
            }
        }

        clippedPolyObject << QPointF( m_left, m_bottom );
        numAddedPoints++;
        break;

    case 7:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 1 ) {
            clippedPolyObject << clipTop( m, m_previousPoint );
            numAddedPoints++;
            clippedPolyObject << clipBottom( m, m_currentPoint );
            numAddedPoints++;
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
                numAddedPoints++;
            }
        }
        break;

    case 8:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }
            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
                numAddedPoints++;
            }
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
                numAddedPoints++;
            }
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
                numAddedPoints++;
            }
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
                numAddedPoints++;
            }

            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom ) {
                clippedPolyObject << QPointF( m_left, m_bottom );
                numAddedPoints++;
            }
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top ) {
                clippedPolyObject << QPointF( m_right, m_top );
                numAddedPoints++;
            }
        }

        clippedPolyObject << QPointF( m_right, m_bottom );
        numAddedPoints++;
        break;

    default:
        break;
    }

    if(numAddedPoints == 2) {
        m_clippedTwice = true;
    }
}

void BaseClipper::clipOnceCorner( QPolygonF & clippedPolyObject,
                                  QVector<QPolygonF> & clippedPolyObjects,
                                  const QPointF& corner,
                                  const QPointF& point,
                                  bool isClosed )
{
    Q_UNUSED( clippedPolyObjects )
    Q_UNUSED( isClosed )

    if ( m_currentSector == 4) {
        // Appearing
        clippedPolyObject << corner;
        clippedPolyObject << point;
    } else {
        // Disappearing
        clippedPolyObject << point;
        clippedPolyObject << corner;
    }
}

void BaseClipper::clipOnceEdge( QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                const QPointF& point,
                                bool isClosed )
{
    if ( m_currentSector == 4) {
        // Appearing
        if ( !isClosed ) {
            clippedPolyObject = QPolygonF();
        }
        clippedPolyObject << point;
    }
    else {
        // Disappearing
        clippedPolyObject << point;

        if ( !isClosed ) {
            clippedPolyObjects << clippedPolyObject;
            clippedPolyObject = QPolygonF();
        }
    }
}

void BaseClipper::clipOnce( QPolygonF & clippedPolyObject,
                            QVector<QPolygonF> & clippedPolyObjects,
                            bool isClosed )
{
    //	Interpolate border points (linear interpolation)
    QPointF point;

    // Calculating the slope.
    qreal m = _m( m_previousPoint, m_currentPoint );

    // Calculate in which sector the end of the line is located that is off screen
    int offscreenpos = ( m_currentSector == 4 ) ? m_previousSector : m_currentSector;

    // "Rise over run" for all possible situations .
    switch ( offscreenpos ) {
    case 0: // topleft
        point = clipTop( m, m_previousPoint );
        if ( point.x() < m_left ) {
            point = clipLeft( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_left, m_top ), point, isClosed );
        break;
    case 1: // top
        point = clipTop( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 2: // topright
        point = clipTop( m, m_previousPoint );
        if ( point.x() > m_right ) {
            point = clipRight( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_right, m_top ), point, isClosed );
        break;
    case 3: // left
        point = clipLeft( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 5: // right
        point = clipRight( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 6: // bottomleft
        point = clipBottom( m, m_previousPoint );
        if ( point.x() < m_left ) {
            point = clipLeft( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_left, m_bottom ), point, isClosed );
        break;
    case 7: // bottom
        point = clipBottom( m, m_previousPoint );
        clipOnceEdge( clippedPolyObject, clippedPolyObjects, point, isClosed );
        break;
    case 8: // bottomright
        point = clipBottom( m, m_previousPoint );
        if ( point.x() > m_right ) {
            point = clipRight( m, point );
        }
        clipOnceCorner( clippedPolyObject, clippedPolyObjects, QPointF( m_right, m_bottom ), point, isClosed );
        break;
    default:
        break;
    }
}

}
