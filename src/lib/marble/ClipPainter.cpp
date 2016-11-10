//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include "ClipPainter.h"

#include <cmath>

#include "MarbleDebug.h"


namespace Marble
{

class ClipPainterPrivate
{
 public:
    explicit ClipPainterPrivate( ClipPainter * parent );

    ClipPainter * q;

    // true if clipping is on.
    bool    m_doClip;

    // The limits
    qreal  m_left;
    qreal  m_right;
    qreal  m_top;
    qreal  m_bottom;

    // Used in the paint process of vectors..
    int     m_currentSector;
    int     m_previousSector;

    //	int m_debugNodeCount;

    QPointF    m_currentPoint;
    QPointF    m_previousPoint; 

    inline int sector( const QPointF & point ) const;

    inline QPointF clipTop( qreal m, const QPointF & point ) const;
    inline QPointF clipLeft( qreal m, const QPointF & point ) const;
    inline QPointF clipBottom( qreal m, const QPointF & point ) const;
    inline QPointF clipRight( qreal m, const QPointF & point ) const;

    inline void initClipRect();

    inline void clipPolyObject ( const QPolygonF & sourcePolygon, 
                                 QVector<QPolygonF> & clippedPolyObjects,
                                 bool isClosed );

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


    void labelPosition(const QPolygonF& polygon, QVector<QPointF>& labelNodes,
                                LabelPositionFlags labelPositionFlags);

    bool pointAllowsLabel( const QPointF& point );
    QPointF interpolateLabelPoint( const QPointF& previousPoint, 
                                   const QPointF& currentPoint,
                                   LabelPositionFlags labelPositionFlags );

    static inline qreal _m( const QPointF & start, const QPointF & end );

    void debugDrawNodes( const QPolygonF & );

    qreal m_labelAreaMargin;

    int m_debugPenBatchColor;
    int m_debugBrushBatchColor;
    int m_debugPolygonsLevel;
    bool m_debugBatchRender;
};

}

using namespace Marble;

// #define MARBLE_DEBUG

ClipPainter::ClipPainter(QPaintDevice * pd, bool clip)
    : QPainter( pd ), d( new ClipPainterPrivate( this ) )
{
    d->initClipRect();

    // m_debugNodeCount = 0;
    d->m_doClip = clip;
}


ClipPainter::ClipPainter()
    : d( new ClipPainterPrivate( this ) )
{
}


ClipPainter::~ClipPainter()
{
    delete d;
}


void ClipPainter::setScreenClip(bool enable)
{
    d->m_doClip = enable;
}


bool ClipPainter::hasScreenClip() const
{
    return d->m_doClip;
}


void ClipPainter::drawPolygon ( const QPolygonF & polygon,
                                Qt::FillRule fillRule )
{
    if ( d->m_doClip ) {	
        d->initClipRect();
        QVector<QPolygonF> clippedPolyObjects;

        d->clipPolyObject( polygon, clippedPolyObjects, true );

        foreach( const QPolygonF & clippedPolyObject, clippedPolyObjects ) { 
            if ( clippedPolyObject.size() > 2 ) {
                // mDebug() << "Size: " << clippedPolyObject.size();
                if (d->m_debugPolygonsLevel) {
                    QBrush brush = QPainter::brush();
                    QBrush originalBrush = brush;
                    QColor color = brush.color();
                    color.setAlpha(color.alpha()*0.75);
                    brush.setColor(color);
                    QPainter::setBrush(brush);

                    QPainter::drawPolygon ( clippedPolyObject, fillRule );

                    QPainter::setBrush(originalBrush);

                    d->debugDrawNodes( clippedPolyObject );
                }
                else {
                    QPainter::drawPolygon ( clippedPolyObject, fillRule );
                }
            }
        }
    }
    else {
        if (d->m_debugPolygonsLevel) {
            QBrush brush = QPainter::brush();
            QBrush originalBrush = brush;
            QColor color = brush.color();
            color.setAlpha(color.alpha()*0.75);
            brush.setColor(color);
            QPainter::setBrush(brush);

            QPainter::drawPolygon ( polygon, fillRule );

            QPainter::setBrush(originalBrush);

            d->debugDrawNodes( polygon );
        }
        else {
            QPainter::drawPolygon ( polygon, fillRule );
        }
    }
}

void ClipPainter::drawPolyline( const QPolygonF & polygon )
{
    if ( d->m_doClip ) {
        d->initClipRect();
        QVector<QPolygonF> clippedPolyObjects;

        d->clipPolyObject( polygon, clippedPolyObjects, false );

        foreach( const QPolygonF & clippedPolyObject, clippedPolyObjects ) { 
            if ( clippedPolyObject.size() > 1 ) {
                if (d->m_debugPolygonsLevel) {
                    QPen pen = QPainter::pen();
                    QPen originalPen = pen;
                    QColor color = pen.color();
                    color.setAlpha(color.alpha()*0.75);
                    pen.setColor(color);
                    QPainter::setPen(pen);

                    QPainter::drawPolyline ( clippedPolyObject );

                    QPainter::setPen(originalPen);

                    d->debugDrawNodes( clippedPolyObject );
                }
                else {
                    QPainter::drawPolyline ( clippedPolyObject );
                }
            }
        }
    }
    else {
        if (d->m_debugPolygonsLevel) {
            QPen pen = QPainter::pen();
            QPen originalPen = pen;
            QColor color = pen.color();
            color.setAlpha(color.alpha()*0.75);
            pen.setColor(color);
            QPainter::setPen(pen);

            QPainter::drawPolyline ( polygon );

            QPainter::setPen(originalPen);

            d->debugDrawNodes( polygon );
        }
        else {
            QPainter::drawPolyline ( polygon );
        }
    }
}

void ClipPainter::drawPolyline(const QPolygonF & polygon, QVector<QPointF>& labelNodes,
                               LabelPositionFlags positionFlags)
{
    if ( d->m_doClip ) {
        d->initClipRect();
        QVector<QPolygonF> clippedPolyObjects;

        d->clipPolyObject( polygon, clippedPolyObjects, false );

        foreach( const QPolygonF & clippedPolyObject, clippedPolyObjects ) { 
            if (d->m_debugPolygonsLevel) {
                QPen pen = QPainter::pen();
                QPen originalPen = pen;
                QColor color = pen.color();
                color.setAlpha(color.alpha()*0.75);
                pen.setColor(color);
                QPainter::setPen(pen);

                QPainter::drawPolyline ( clippedPolyObject );

                QPainter::setPen(originalPen);

                d->debugDrawNodes( clippedPolyObject );
            }
            else {
                QPainter::drawPolyline ( clippedPolyObject );
            }
        }
    }
    else {
        if (d->m_debugPolygonsLevel) {
            QPen pen = QPainter::pen();
            QPen originalPen = pen;
            QColor color = pen.color();
            color.setAlpha(color.alpha()*0.75);
            pen.setColor(color);
            QPainter::setPen(pen);

            QPainter::drawPolyline ( polygon );

            QPainter::setPen(originalPen);

            d->debugDrawNodes( polygon );
        }
        else {
            QPainter::drawPolyline ( polygon );
        }

        d->labelPosition( polygon, labelNodes, positionFlags );
    }
}

void ClipPainter::labelPosition(const QPolygonF & polygon, QVector<QPointF>& labelNodes,
                                       LabelPositionFlags labelPositionFlags) {
    d->labelPosition(polygon, labelNodes, labelPositionFlags);
}

void ClipPainter::setPen(const QColor &color) {
    if (d->m_debugBatchRender) {
        qDebug() << Q_FUNC_INFO;
    }
    setPen(QPen(color));
}

void ClipPainter::setPen(Qt::PenStyle style) {
    if (d->m_debugBatchRender) {
        qDebug() << Q_FUNC_INFO;
    }
    setPen(QPen(style));
}

void ClipPainter::setPen(const QPen & pen) {
    if (d->m_debugBatchRender) {
        qDebug() << Q_FUNC_INFO;
        if (pen != QPainter::pen()) {
            qDebug() << "--" << pen.color()  << QPainter::pen().color() ;
            QPen newPen = pen;
            newPen.setColor((Qt::GlobalColor)(d->m_debugPenBatchColor));
            QPainter::setPen(newPen);
            d->m_debugPenBatchColor++;
            d->m_debugPenBatchColor %= 20;
        }
        else {
            qDebug() << "++";
            QPainter::setPen(pen);
        }
    }
    else {
        QPainter::setPen(pen);
    }
}

void ClipPainter::setBrush(const QBrush & brush) {
    if (d->m_debugBatchRender) {
        qDebug() << Q_FUNC_INFO;
        if (brush != QPainter::brush()) {
            qDebug() << "--" << brush.color()  << QPainter::brush().color() ;
            QBrush batchColor(QColor((Qt::GlobalColor)(d->m_debugBrushBatchColor)));
            QPainter::setBrush(batchColor);
            d->m_debugBrushBatchColor++;
            d->m_debugBrushBatchColor %= 20;
        }
        else {
            qDebug() << "++";
            QPainter::setBrush(brush);
        }
    }
    else {
        QPainter::setBrush(brush);
    }
}

void ClipPainterPrivate::labelPosition(const QPolygonF & polygon, QVector<QPointF>& labelNodes,
                                       LabelPositionFlags labelPositionFlags)
{
    bool currentAllowsLabel = false;

    if ( labelPositionFlags.testFlag( LineCenter ) ) {
        // The Label at the center of the polyline:
        int labelPosition = static_cast<int>( polygon.size() / 2.0 );
        if ( polygon.size() > 0 ) {
            if ( labelPosition >= polygon.size() ) {
                labelPosition = polygon.size() - 1;
            }
            labelNodes << polygon.at( labelPosition );
        }
    }

    if ( polygon.size() > 0 && labelPositionFlags.testFlag( LineStart ) ) {
        if ( pointAllowsLabel( polygon.first() ) ) {
            labelNodes << polygon.first();
        }

        // The Label at the start of the polyline:
        for ( int it = 1; it < polygon.size(); ++it ) {
            currentAllowsLabel = pointAllowsLabel( polygon.at( it ) );

            if ( currentAllowsLabel ) {
                // As polygon.size() > 0 it's ensured that it-1 exists.
                QPointF node = interpolateLabelPoint( polygon.at( it -1 ), polygon.at( it ),
                                                    labelPositionFlags );
                if ( node != QPointF( -1.0, -1.0 ) ) {
                    labelNodes << node;
                }
                break;
            }
        }
    }

    if ( polygon.size() > 1 && labelPositionFlags.testFlag( LineEnd ) ) {
        if ( pointAllowsLabel( polygon.at( polygon.size() - 1 ) ) ) {
            labelNodes << polygon.at( polygon.size() - 1 );
        }

        // The Label at the end of the polyline:
        for ( int it = polygon.size() - 2; it > 0; --it ) {
            currentAllowsLabel = pointAllowsLabel( polygon.at( it ) );

            if ( currentAllowsLabel ) {
                QPointF node = interpolateLabelPoint( polygon.at( it + 1 ), polygon.at( it ),
                                                    labelPositionFlags );
                if ( node != QPointF( -1.0, -1.0 ) ) {
                    labelNodes << node;
                }
                break;
            }
        }
    }
}

bool ClipPainterPrivate::pointAllowsLabel( const QPointF& point )
{

    if ( point.x() > m_labelAreaMargin && point.x() < q->viewport().width() - m_labelAreaMargin 
         && point.y() > m_labelAreaMargin && point.y() < q->viewport().height() - m_labelAreaMargin ) {
        return true;
    }
    return false;
}

QPointF ClipPainterPrivate::interpolateLabelPoint( const QPointF& previousPoint, 
                                                   const QPointF& currentPoint,
                                                   LabelPositionFlags labelPositionFlags )
{
    qreal m = _m( previousPoint, currentPoint );
    if ( previousPoint.x() <= m_labelAreaMargin ) {
        if ( labelPositionFlags.testFlag( IgnoreXMargin ) ) {
            return QPointF( -1.0, -1.0 );
        }
        return QPointF( m_labelAreaMargin, 
                        previousPoint.y() + ( m_labelAreaMargin - previousPoint.x() ) * m );
    }
    else if ( previousPoint.x() >= q->viewport().width() - m_labelAreaMargin  ) {
        if ( labelPositionFlags.testFlag( IgnoreXMargin ) ) {
            return QPointF( -1.0, -1.0 );
        }
        return QPointF( q->viewport().width() - m_labelAreaMargin,
                        previousPoint.y() - 
                        ( previousPoint.x() - q->viewport().width() + m_labelAreaMargin ) * m );        
    }

    if ( previousPoint.y() <= m_labelAreaMargin ) {
        if ( labelPositionFlags.testFlag( IgnoreYMargin ) ) {
            return QPointF( -1.0, -1.0 );
        }
        return QPointF( previousPoint.x() + ( m_labelAreaMargin - previousPoint.y() ) / m, 
                        m_labelAreaMargin );
    } 
    else if ( previousPoint.y() >= q->viewport().height() - m_labelAreaMargin  ) {
        if ( labelPositionFlags.testFlag( IgnoreYMargin ) ) {
            return QPointF( -1.0, -1.0 );
        }
        return QPointF(   previousPoint.x() - 
                        ( previousPoint.y() - q->viewport().height() + m_labelAreaMargin ) / m,
                          q->viewport().height() - m_labelAreaMargin );        
    }

//    mDebug() << Q_FUNC_INFO << "Previous and current node position are allowed!";

    return QPointF( -1.0, -1.0 );
}

ClipPainterPrivate::ClipPainterPrivate( ClipPainter * parent )
    : m_doClip( true ),
      m_left(0.0),
      m_right(0.0),
      m_top(0.0),
      m_bottom(0.0),
      m_currentSector(4),
      m_previousSector(4),
      m_currentPoint(QPointF()),
      m_previousPoint(QPointF()), 
      m_labelAreaMargin(10.0),
      m_debugPenBatchColor(0),
      m_debugBrushBatchColor(0),
      m_debugPolygonsLevel(0),
      m_debugBatchRender(false)
{
    q = parent;
}

void ClipPainterPrivate::initClipRect ()
{
    qreal penHalfWidth = q->pen().widthF() / 2.0 + 1.0;

    m_left   = -penHalfWidth; 
    m_right  = (qreal)(q->device()->width()) + penHalfWidth;
    m_top    = -penHalfWidth; 
    m_bottom = (qreal)(q->device()->height()) + penHalfWidth;
}

qreal ClipPainterPrivate::_m( const QPointF & start, const QPointF & end )
{
    qreal  divisor = end.x() - start.x();
    if ( std::fabs( divisor ) < 0.000001 ) {
        // this is in screencoordinates so the difference
        // between 0, 0.000001 and -0.000001 isn't visible at all 
        divisor = 0.000001;
    }

    return ( end.y() - start.y() ) 
         / divisor;
}


QPointF ClipPainterPrivate::clipTop( qreal m, const QPointF & point ) const
{
    return QPointF( ( m_top - point.y() ) / m + point.x(), m_top );
}

QPointF ClipPainterPrivate::clipLeft( qreal m, const QPointF & point ) const
{
    return QPointF( m_left, ( m_left - point.x() ) * m + point.y() );
}

QPointF ClipPainterPrivate::clipBottom( qreal m, const QPointF & point ) const
{
    return QPointF( ( m_bottom - point.y() ) / m + point.x(), m_bottom );
}

QPointF ClipPainterPrivate::clipRight( qreal m, const QPointF & point ) const
{
    return QPointF( m_right, ( m_right - point.x() ) * m + point.y() );
}

int ClipPainterPrivate::sector( const QPointF & point ) const
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

void ClipPainterPrivate::clipPolyObject ( const QPolygonF & polygon, 
                                          QVector<QPolygonF> & clippedPolyObjects,
                                          bool isClosed )
{
    //	mDebug() << "ClipPainter enabled." ;

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

    while ( itPoint != itEndPoint ) {
        m_currentPoint = (*itPoint);
        // mDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the sector of the current point.
        m_currentSector = sector( m_currentPoint );

        // Initialize the variables related to the previous point.
        if ( itPoint == itStartPoint && processingLastNode == false ) {
            if ( isClosed ) {
                m_previousPoint = polygon.last();

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
            }
            else {
                // This case mostly deals with lines that reach from one
                // sector that is located off screen to another one that
                // is located off screen. In this situation the line 
                // can get clipped once, twice, or not at all.
                clipMultiple( clippedPolyObject, clippedPolyObjects, isClosed );
            }

            m_previousSector = m_currentSector;
        }

        // If the current point is onscreen, just add it to our final polygon.
        if ( m_currentSector == 4 ) {

            clippedPolyObject << m_currentPoint;
#ifdef MARBLE_DEBUG
            ++(m_debugNodeCount);
#endif
        }

        m_previousPoint = m_currentPoint;

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

    // Only add the pointer if there's node data available.
    if ( !clippedPolyObject.isEmpty() ) {
        clippedPolyObjects << clippedPolyObject;
    }
}


void ClipPainterPrivate::clipMultiple( QPolygonF & clippedPolyObject,
                                       QVector<QPolygonF> & clippedPolyObjects,
                                       bool isClosed )
{
    Q_UNUSED( clippedPolyObjects )
    Q_UNUSED( isClosed )

    // Take care of adding nodes in the image corners if the iterator 
    // traverses off screen sections.

    qreal  m = _m( m_previousPoint, m_currentPoint );

    switch ( m_currentSector ) {
    case 0:
        if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
            if ( pointTop.x() >= m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top ) 
                clippedPolyObject << pointLeft;
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
   
            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom )
                clippedPolyObject << QPointF( m_left, m_bottom );
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top )
                clippedPolyObject << QPointF( m_right, m_top );
        }

        clippedPolyObject << QPointF( m_left, m_top );
        break;

    case 1:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom ) 
                clippedPolyObject << pointLeft;
            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
        }
        else if ( m_previousSector == 7 ) {
            clippedPolyObject << clipBottom( m, m_previousPoint );
            clippedPolyObject << clipTop( m, m_currentPoint );
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom ) 
                clippedPolyObject << pointRight;
            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
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
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
            if ( pointTop.x() > m_left && pointTop.x() <= m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top ) 
                clippedPolyObject << pointRight;
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
   
            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom )
                clippedPolyObject << QPointF( m_right, m_bottom );
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top )
                clippedPolyObject << QPointF( m_left, m_top );
        }

        clippedPolyObject << QPointF( m_right, m_top );
        break;

    case 3:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 5 ) {
            clippedPolyObject << clipRight( m, m_previousPoint );
            clippedPolyObject << clipLeft( m, m_currentPoint );
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top ) 
                clippedPolyObject << pointRight;
            if ( pointTop.x() > m_left && pointTop.x() <= m_right )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
        }
        break;

    case 5:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
        }
        else if ( m_previousSector == 3 ) {
            clippedPolyObject << clipLeft( m, m_previousPoint );
            clippedPolyObject << clipRight( m, m_currentPoint );
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) 
                clippedPolyObject << pointLeft;
            if ( pointTop.x() >= m_left && pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
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
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_left, m_top );
            }
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom ) 
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                clippedPolyObject << pointTop;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
   
            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom )
                clippedPolyObject << QPointF( m_right, m_bottom );
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top )
                clippedPolyObject << QPointF( m_left, m_top );
        }

        clippedPolyObject << QPointF( m_left, m_bottom );
        break;

    case 7:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                clippedPolyObject << pointLeft;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
            if ( pointBottom.x() > m_left )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                clippedPolyObject << pointRight;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
            }
            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left )
                clippedPolyObject << pointTop;
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() > m_left ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            clippedPolyObject << clipTop( m, m_previousPoint );
            clippedPolyObject << clipBottom( m, m_currentPoint );
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right )
                clippedPolyObject << pointTop;
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
            if ( pointBottom.x() < m_right ) {
                clippedPolyObject << pointBottom;
            } else {
                clippedPolyObject << QPointF( m_right, m_bottom );
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
            } else {
                clippedPolyObject << QPointF( m_left, m_bottom );
            }
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right ) {
                clippedPolyObject << pointTop;
            } else {
                clippedPolyObject << QPointF( m_right, m_top );
            }
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom ) 
                clippedPolyObject << pointRight;
            if ( pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                clippedPolyObject << pointTop;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                clippedPolyObject << pointLeft;
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                clippedPolyObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                clippedPolyObject << pointRight;
   
            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom )
                clippedPolyObject << QPointF( m_left, m_bottom );
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top )
                clippedPolyObject << QPointF( m_right, m_top );
        }

        clippedPolyObject << QPointF( m_right, m_bottom );
        break;

    default:
        break;				
    }
}

void ClipPainterPrivate::clipOnceCorner( QPolygonF & clippedPolyObject,
                                         QVector<QPolygonF> & clippedPolyObjects,
                                         const QPointF& corner,
                                         const QPointF& point, 
                                         bool isClosed ) const
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

void ClipPainterPrivate::clipOnceEdge( QPolygonF & clippedPolyObject,
                                       QVector<QPolygonF> & clippedPolyObjects,
                                       const QPointF& point,
                                       bool isClosed ) const
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
        }
    }
}

void ClipPainterPrivate::clipOnce( QPolygonF & clippedPolyObject,
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

void ClipPainter::setDebugPolygonsLevel( int level ) {
    d->m_debugPolygonsLevel = level;
}

void ClipPainter::setDebugBatchRender( bool enabled ) {
    d->m_debugBatchRender = enabled;
}


void ClipPainterPrivate::debugDrawNodes( const QPolygonF & polygon )
{

    q->save();
    q->setRenderHint( QPainter::Antialiasing, false );

    q->setPen( Qt::red );
    q->setBrush(QBrush("#40FF0000"));

    const QVector<QPointF>::const_iterator  itStartPoint = polygon.constBegin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.constEnd();
    QVector<QPointF>::const_iterator        itPoint      = itStartPoint;

    int i = 0;

    for (; itPoint != itEndPoint; ++itPoint ) {

        ++i;
        
        if ( itPoint == itStartPoint || itPoint == itStartPoint + 1 || itPoint == itStartPoint + 2 ) {
            q->setPen( Qt::darkGreen );
            q->setBrush(QBrush("#4000FF00"));
            if ( itPoint == itStartPoint ) {
                q->drawRect( itPoint->x() - 6.0, itPoint->y() - 6.0 , 12.0, 12.0 );
            }
            else if ( itPoint == itStartPoint + 1 ) {
                q->drawRect( itPoint->x() - 4.0, itPoint->y() - 4.0 , 8.0, 8.0 );
            }
            else {
                q->drawRect( itPoint->x() - 2.0, itPoint->y() - 2.0 , 4.0, 4.0 );
            }
            q->setPen( Qt::red );
            q->setBrush(QBrush("#40FF0000"));
        }
        else if ( itPoint == itEndPoint - 1 || itPoint == itEndPoint - 2 || itPoint == itEndPoint - 3 ) {
            q->setPen( Qt::blue );
            q->setBrush(QBrush("#400000FF"));
            if ( itPoint == itEndPoint - 3 ) {
                q->drawRect( itPoint->x() - 6.0, itPoint->y() - 6.0 , 12.0, 12.0 );
            }
            else if ( itPoint == itEndPoint - 2 ) {
                q->drawRect( itPoint->x() - 4.0, itPoint->y() - 4.0 , 8.0, 8.0 );
            }
            else {
                q->drawRect( itPoint->x() - 2.0, itPoint->y() - 2.0 , 4.0, 4.0 );
            }
            q->setPen( Qt::red );
            q->setBrush(QBrush("#400000FF"));
        }
        else {
            q->drawRect( itPoint->x() - 4, itPoint->y() - 4 , 8.0, 8.0 );
        }
        if (m_debugPolygonsLevel == 2) {
            q->setFont(QFont(QStringLiteral("Sans Serif"), 7));
            q->setPen("black");
            q->drawText(itPoint->x() + 6.0, itPoint->y() + (15 - (i * 5) % 30) , QString::number(i));
        }
    }
    q->restore();
}
