//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "ClipPainter.h"

#include <cmath>

#include <QtCore/QDebug>

// #define DEBUG_DRAW_NODES

namespace Marble
{

class ClipPainterPrivate
{
 public:
    ClipPainterPrivate( ClipPainter * parent );

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
                                bool isClosed );
    inline void clipOnceEdge(   QPolygonF & clippedPolyObject,
                                QVector<QPolygonF> & clippedPolyObjects,
                                const QPointF& point,
                                bool isClosed );


    void labelPosition( const QPolygonF & polygon, QVector<QPointF>& labelNodes, 
                                LabelPositionFlags labelPositionFlags);

    bool pointAllowsLabel( const QPointF& point );
    QPointF interpolateLabelPoint( const QPointF& previousPoint, 
                                   const QPointF& currentPoint,
                                   LabelPositionFlags labelPositionFlags );

    inline qreal _m( const QPointF & start, const QPointF & end ) const;

#ifdef DEBUG_DRAW_NODES
    void debugDrawNodes( const QPolygonF & ); 
#endif

    qreal m_labelAreaMargin;
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


void ClipPainter::setClipping(bool enable)
{
    d->m_doClip = enable;
}


bool ClipPainter::isClipping() const
{
    return d->m_doClip;
}


void ClipPainter::drawPolygon ( const QPolygonF & polygon,
                                Qt::FillRule fillRule )
{
    d->initClipRect();

    if ( d->m_doClip ) {	
        QVector<QPolygonF> clippedPolyObjects;

        d->clipPolyObject( polygon, clippedPolyObjects, true );

        foreach( const QPolygonF & clippedPolyObject, clippedPolyObjects ) { 
            if ( clippedPolyObject.size() > 2 ) {
                // qDebug() << "Size: " << clippedPolyObject.size();
                QPainter::drawPolygon ( clippedPolyObject, fillRule );
                // qDebug() << "done";
            }
        }
    }
    else {
        QPainter::drawPolygon ( polygon, fillRule );
    }
}

void ClipPainter::drawPolyline( const QPolygonF & polygon )
{
    d->initClipRect();

    if ( d->m_doClip ) {	
        QVector<QPolygonF> clippedPolyObjects;

        d->clipPolyObject( polygon, clippedPolyObjects, false );

        foreach( const QPolygonF & clippedPolyObject, clippedPolyObjects ) { 
            if ( clippedPolyObject.size() > 1 ) {
                // qDebug() << "Size: " << clippedPolyObject.size();
                QPainter::drawPolyline ( clippedPolyObject );
                // qDebug() << "done";

                #ifdef DEBUG_DRAW_NODES
                    d->debugDrawNodes( clippedPolyObject );
                #endif
            }
        }
    }
    else {
        QPainter::drawPolyline( polygon );

        #ifdef DEBUG_DRAW_NODES
            d->debugDrawNodes( polygon );
        #endif
    }
}

void ClipPainter::drawPolyline( const QPolygonF & polygon, QVector<QPointF>& labelNodes,
                                LabelPositionFlags positionFlags)
{
    d->initClipRect();

    if ( d->m_doClip ) {    
 
        QVector<QPolygonF> clippedPolyObjects;

        d->clipPolyObject( polygon, clippedPolyObjects, false );

        foreach( const QPolygonF & clippedPolyObject, clippedPolyObjects ) { 
            if ( clippedPolyObject.size() > 1 ) {
                // qDebug() << "Size: " << clippedPolyObject.size();
                QPainter::drawPolyline ( clippedPolyObject );
                // qDebug() << "done";

                #ifdef DEBUG_DRAW_NODES
                    d->debugDrawNodes( clippedPolyObject );
                #endif

                d->labelPosition( clippedPolyObject, labelNodes, positionFlags );
            }
        }
    }
    else {
        QPainter::drawPolyline( polygon );

        #ifdef DEBUG_DRAW_NODES
            d->debugDrawNodes( polygon );
        #endif

        d->labelPosition( polygon, labelNodes, positionFlags );
    }
}

void ClipPainterPrivate::labelPosition( const QPolygonF & polygon, QVector<QPointF>& labelNodes, 
                                        LabelPositionFlags labelPositionFlags)
{
    int labelPosition = 0;

    bool currentAllowsLabel = false;
    bool previousAllowsLabel = false;

    if ( labelPositionFlags.testFlag( LineCenter ) ) {
        // The Label at the center of the polyline:
        labelPosition = static_cast<int>( polygon.size() / 2.0 );
        if ( polygon.size() > 0 ) {
            if ( labelPosition >= polygon.size() ) {
                labelPosition = polygon.size() - 1;
            }
            labelNodes << polygon.at( labelPosition );
        }
    }

    if ( labelPositionFlags.testFlag( LineStart ) ) {
        if ( polygon.size() > 0 ) {
            if ( pointAllowsLabel( polygon.at(0) ) ) {
                labelNodes << polygon.at( 0 );
            }
        }

        // The Label at the start of the polyline:
        for ( int it = 1; it != polygon.size(); ++it ) {
            currentAllowsLabel = pointAllowsLabel( polygon.at( it ) );

            if ( currentAllowsLabel ) {
                QPointF node = interpolateLabelPoint( polygon.at( it -1 ), polygon.at( it ),
                                                    labelPositionFlags );
                if ( node != QPointF( -1.0, -1.0 ) ) {
                    labelNodes << node;
                }
                break;
            }
            previousAllowsLabel = currentAllowsLabel;
        }
    }

    if ( labelPositionFlags.testFlag( LineEnd ) ) {
        if ( polygon.size() > 0 ) {
            if ( pointAllowsLabel( polygon.at( polygon.size() - 1 ) ) ) {
                labelNodes << polygon.at( polygon.size() - 1 );
            }
        }

        // The Label at the start of the polyline:
        for ( int it = polygon.size() - 1; it != 1; --it ) {
            currentAllowsLabel = pointAllowsLabel( polygon.at( it ) );

            if ( currentAllowsLabel ) {
                QPointF node = interpolateLabelPoint( polygon.at( it + 1 ), polygon.at( it ),
                                                    labelPositionFlags );
                if ( node != QPointF( -1.0, -1.0 ) ) {
                    labelNodes << node;
                }
                break;
            }
            previousAllowsLabel = currentAllowsLabel;
        }
    }
}

bool ClipPainterPrivate::pointAllowsLabel( const QPointF& point ){

    if ( point.x() > m_labelAreaMargin && point.x() < q->viewport().width() - m_labelAreaMargin 
         && point.y() > m_labelAreaMargin && point.y() < q->viewport().height() - m_labelAreaMargin ) {
        return true;
    }
    return false;
}

QPointF ClipPainterPrivate::interpolateLabelPoint( const QPointF& previousPoint, 
                                                   const QPointF& currentPoint,
                                                   LabelPositionFlags labelPositionFlags ) {
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

    qDebug() << Q_FUNC_INFO << "Previous and current node position are allowed!";

    return QPointF();
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
      m_labelAreaMargin(10.0)
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

qreal ClipPainterPrivate::_m( const QPointF & start, const QPointF & end ) const
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


void ClipPainterPrivate::clipPolyObject ( const QPolygonF & polygon, 
                                          QVector<QPolygonF> & clippedPolyObjects,
                                          bool isClosed )
{
    // If we think of the image borders as (infinitly long) parallel
    // lines then the plane is divided into 9 sectors.  Each of these
    // sections is identified by a unique keynumber (currentSector):
    //
    //	0 | 1 | 2
    //  --+---+--
    //	3 | 4 | 5 <- sector number "4" represents the onscreen sector / viewport
    //  --+---+--
    //	6 | 7 | 8
    //

    //	qDebug() << "ClipPainter enabled." ;
    // clippedPolyObjects.clear();

    // Only create a new polyObject as soon as we know for sure that 
    // the current point is on the screen. 
    QPolygonF clippedPolyObject = QPolygonF();

    const QVector<QPointF>::const_iterator  itStartPoint = polygon.constBegin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.constEnd();
    QVector<QPointF>::const_iterator        itPoint      = itStartPoint;

    for (; itPoint != itEndPoint; ++itPoint ) {

        m_currentPoint = (*itPoint);
        // qDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the section of the current point.
        int currentXSector = 1;
        if ( m_currentPoint.x() < m_left )
            currentXSector = 0;
        else if ( m_currentPoint.x() > m_right ) 
            currentXSector = 2;
							
        int currentYSector = 3;
        if ( m_currentPoint.y() < m_top ) 
            currentYSector = 0;
        else if ( m_currentPoint.y() > m_bottom ) 
            currentYSector = 6;

        // By adding m_currentXSector and m_currentYSector we get a 
        // sector number of the values shown in the ASCII-art graph above.
        m_currentSector = currentYSector + currentXSector;

        // Initialize a few remaining variables.
        if ( itPoint == itStartPoint ) {
            m_previousSector = m_currentSector;
        }

        // If the current point reaches a new sector, take care of clipping.
        if ( m_currentSector != m_previousSector ) {
            if ( m_currentSector == 4 || m_previousSector == 4 ) {
                // This is just the case where either the current or the 
                // previous point is visible on the screen but not both. 
                // Hence we only need to clip once and require only one interpolation 
                // for both cases.

                clipOnce( clippedPolyObject, clippedPolyObjects, isClosed );
            }
            else {
                // This case mostly deals with lines that reach from one
                // sector that is located offscreen to another one that
                // is located offscreen. In this situation the line 
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
    // Take care of adding nodes in the image corners if the iterator 
    // traverses offscreen sections.

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
                                         bool isClosed )
{
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

    // Calculate in which sector the end of the line is located that is offscreen 
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

#ifdef DEBUG_DRAW_NODES

void ClipPainterPrivate::debugDrawNodes( const QPolygonF & polygon ) {

    q->save();
    q->setRenderHint( QPainter::Antialiasing, false );

    q->setPen( Qt::red );
    q->setBrush( Qt::transparent );

    const QVector<QPointF>::const_iterator  itStartPoint = polygon.constBegin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.constEnd();
    QVector<QPointF>::const_iterator        itPoint      = itStartPoint;

    for (; itPoint != itEndPoint; ++itPoint ) {
        
        if ( itPoint == itStartPoint || itPoint == itStartPoint + 1 || itPoint == itStartPoint + 2 ) {
            q->setPen( Qt::darkGreen );
            if ( itPoint == itStartPoint ) {
                q->drawRect( itPoint->x() - 3.0, itPoint->y() - 3.0 , 6.0, 6.0 );
            }
            else if ( itPoint == itStartPoint + 1 ) {
                q->drawRect( itPoint->x() - 2.0, itPoint->y() - 2.0 , 4.0, 4.0 );
            }
            else {
                q->drawRect( itPoint->x() - 1.0, itPoint->y() - 1.0 , 2.0, 2.0 );
            }
            q->setPen( Qt::red );
        }
        else if ( itPoint == itEndPoint - 1 || itPoint == itEndPoint - 2 || itPoint == itEndPoint - 3 ) {
            q->setPen( Qt::blue );
            if ( itPoint == itEndPoint - 3 ) {
                q->drawRect( itPoint->x() - 3.0, itPoint->y() - 3.0 , 6.0, 6.0 );
            }
            else if ( itPoint == itEndPoint - 2 ) {
                q->drawRect( itPoint->x() - 2.0, itPoint->y() - 2.0 , 4.0, 4.0 );
            }
            else {
                q->drawRect( itPoint->x() - 1.0, itPoint->y() - 1.0 , 2.0, 2.0 );
            }
            q->setPen( Qt::red );
        }
        else {
            q->drawRect( itPoint->x() - 1.5, itPoint->y() - 1.5 , 3.0, 3.0 );
        }

    }
    q->restore();
}

#endif
