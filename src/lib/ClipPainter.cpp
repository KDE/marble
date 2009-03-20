//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "ClipPainter.h"

#include <cmath>

#include <QtCore/QDebug>

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

    // The resulting object from the clipping operation
    QPolygonF  m_clippedObject;

    inline QPointF clipTop( qreal m, const QPointF & point ) const;
    inline QPointF clipLeft( qreal m, const QPointF & point ) const;
    inline QPointF clipBottom( qreal m, const QPointF & point ) const;
    inline QPointF clipRight( qreal m, const QPointF & point ) const;

    inline void initClipRect();

    inline void clipPolyObject ( const QPolygonF & );

    inline void clipMultiple();
    inline void clipOnce();

    inline qreal _m( const QPointF & start, const QPointF & end ) const;
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
        d->clipPolyObject( polygon );
        if ( d->m_clippedObject.size() > 2 ) {
            // qDebug() << "Size: " << m_clippedObject.size();
            QPainter::drawPolygon ( d->m_clippedObject, fillRule );
            // qDebug() << "done";
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
        d->clipPolyObject( polygon );
        if ( d->m_clippedObject.size() > 1 ) {
            // qDebug() << "Size: " << m_clippedObject.size();
            QPainter::drawPolyline ( d->m_clippedObject );
            // qDebug() << "done";
        }
    }
    else {
        QPainter::drawPolyline( polygon );
    }
}

ClipPainterPrivate::ClipPainterPrivate( ClipPainter * parent )
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


void ClipPainterPrivate::clipPolyObject ( const QPolygonF & polygon )
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
    m_clippedObject.clear();

    const QVector<QPointF>::const_iterator  itStartPoint = polygon.begin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.end();
    QVector<QPointF>::const_iterator        itPoint      = itStartPoint;

    for (; itPoint != itEndPoint; ++itPoint ) {

        m_currentPoint = (*itPoint);
        // qDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the section of the current point.
        int m_currentXSector = 1;
        if ( m_currentPoint.x() < m_left )
            m_currentXSector = 0;
        else if ( m_currentPoint.x() > m_right ) 
            m_currentXSector = 2;
							
        int m_currentYSector = 3;
        if ( m_currentPoint.y() < m_top ) 
            m_currentYSector = 0;
        else if ( m_currentPoint.y() > m_bottom ) 
            m_currentYSector = 6;

        // By adding m_currentXSector and m_currentYSector we get a 
        // sector number of the values shown in the ASCII-art graph above.
        m_currentSector = m_currentYSector + m_currentXSector;

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
                clipOnce();
            }
            else {
                // This case mostly deals with lines that reach from one
                // sector that is located offscreen to another one that
                // is located offscreen. In this situation the line 
                // can get clipped once, twice, or not at all.
                clipMultiple();		
            }

            m_previousSector = m_currentSector;
        }

        // If the current point is onscreen, just add it to our final polygon.
        if ( m_currentSector == 4 ) {
            m_clippedObject << m_currentPoint;
#ifdef MARBLE_DEBUG
            ++(m_debugNodeCount);
#endif
        }

        m_previousPoint = m_currentPoint;
    }
}


void ClipPainterPrivate::clipMultiple()
{
    // Take care of adding nodes in the image corners if the iterator 
    // traverses offscreen sections.

    // FIXME:	- bugs related to vertical and horizontal lines in corners  
    //		- borderpoint order

    qreal  m = _m( m_previousPoint, m_currentPoint );

    switch ( m_currentSector ) {
    case 0:
        if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_top );
            }
            if ( pointTop.x() >= m_left && pointTop.x() < m_right )
                m_clippedObject << pointTop;
            if ( pointLeft.y() > m_top ) 
                m_clippedObject << pointLeft;
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left ) {
                m_clippedObject << pointBottom;
            } else {
                m_clippedObject << QPointF( m_left, m_bottom );
            }
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
            if ( pointTop.x() > m_left )
                m_clippedObject << pointTop;
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                m_clippedObject << pointTop;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
   
            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom )
                m_clippedObject << QPointF( m_left, m_bottom );
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top )
                m_clippedObject << QPointF( m_right, m_top );
        }

        m_clippedObject << QPointF( m_left, m_top );
        break;

    case 1:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_top );
            }
            if ( pointTop.x() > m_left )
                m_clippedObject << pointTop;
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointRight.y() > m_top ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_top );
            }
            if ( pointTop.x() < m_right )
                m_clippedObject << pointTop;
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() > m_left )
                m_clippedObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom ) 
                m_clippedObject << pointLeft;
            if ( pointTop.x() > m_left ) {
                m_clippedObject << pointTop;
            } else {
                m_clippedObject << QPointF( m_left, m_top );
            }
        }
        else if ( m_previousSector == 7 ) {
            m_clippedObject << clipBottom( m, m_previousPoint );
            m_clippedObject << clipTop( m, m_currentPoint );
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );

            if ( pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom ) 
                m_clippedObject << pointRight;
            if ( pointTop.x() < m_right ) {
                m_clippedObject << pointTop;
            } else {
                m_clippedObject << QPointF( m_right, m_top );
            }
        }
        break;

    case 2:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_top );
            }
            if ( pointTop.x() > m_left && pointTop.x() <= m_right )
                m_clippedObject << pointTop;
            if ( pointRight.y() > m_top ) 
                m_clippedObject << pointRight;
        }
        else if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right ) {
                m_clippedObject << pointBottom;
            } else {
                m_clippedObject << QPointF( m_right, m_bottom );
            }
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
            if ( pointTop.x() < m_right )
                m_clippedObject << pointTop;
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                m_clippedObject << pointTop;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
   
            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom )
                m_clippedObject << QPointF( m_right, m_bottom );
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top )
                m_clippedObject << QPointF( m_left, m_top );
        }

        m_clippedObject << QPointF( m_right, m_top );
        break;

    case 3:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointBottom.x() > m_left )
                m_clippedObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left )
                m_clippedObject << pointTop;
            if ( pointLeft.y() > m_top ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_top );
            }
        }
        else if ( m_previousSector == 8 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right )
                m_clippedObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 5 ) {
            m_clippedObject << clipRight( m, m_previousPoint );
            m_clippedObject << clipLeft( m, m_currentPoint );
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() > m_top ) 
                m_clippedObject << pointRight;
            if ( pointTop.x() > m_left && pointTop.x() <= m_right )
                m_clippedObject << pointTop;
            if ( pointLeft.y() > m_top ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_top );
            }
        }
        break;

    case 5:
        if ( m_previousSector == 7 ) {
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointRight.y() < m_bottom ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointTop.x() < m_right )
                m_clippedObject << pointTop;
            if ( pointRight.y() > m_top ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_top );
            }
        }
        else if ( m_previousSector == 6 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointRight.y() < m_bottom ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_bottom );
            }
        }
        else if ( m_previousSector == 3 ) {
            m_clippedObject << clipLeft( m, m_previousPoint );
            m_clippedObject << clipRight( m, m_currentPoint );
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() > m_top ) 
                m_clippedObject << pointLeft;
            if ( pointTop.x() >= m_left && pointTop.x() < m_right )
                m_clippedObject << pointTop;
            if ( pointRight.y() > m_top ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_top );
            }
        }
        break;

    case 6:
        if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_bottom );
            }
            if ( pointBottom.x() >= m_left && pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left ) {
                m_clippedObject << pointTop;
            } else {
                m_clippedObject << QPointF( m_left, m_top );
            }
            if ( pointLeft.y() > m_top && pointLeft.y() <= m_bottom ) 
                m_clippedObject << pointLeft;
            if ( pointBottom.x() > m_left )
                m_clippedObject << pointBottom;
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                m_clippedObject << pointTop;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
   
            if ( pointBottom.x() >= m_right && pointRight.y() >= m_bottom )
                m_clippedObject << QPointF( m_right, m_bottom );
            if ( pointTop.x() <= m_left && pointLeft.y() <= m_top )
                m_clippedObject << QPointF( m_left, m_top );
        }

        m_clippedObject << QPointF( m_left, m_bottom );
        break;

    case 7:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_bottom );
            }
            if ( pointBottom.x() > m_left )
                m_clippedObject << pointBottom;
        }
        else if ( m_previousSector == 5 ) {
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointRight.y() < m_bottom ) {
                m_clippedObject << pointRight;
            } else {
                m_clippedObject << QPointF( m_right, m_bottom );
            }
            if ( pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() > m_left )
                m_clippedObject << pointTop;
            if ( pointLeft.y() >= m_top && pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
            if ( pointBottom.x() > m_left ) {
                m_clippedObject << pointBottom;
            } else {
                m_clippedObject << QPointF( m_left, m_bottom );
            }
        }
        else if ( m_previousSector == 1 ) {
            m_clippedObject << clipTop( m, m_previousPoint );
            m_clippedObject << clipBottom( m, m_currentPoint );
        }
        else if ( m_previousSector == 2 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right )
                m_clippedObject << pointTop;
            if ( pointRight.y() >= m_top && pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
            if ( pointBottom.x() < m_right ) {
                m_clippedObject << pointBottom;
            } else {
                m_clippedObject << QPointF( m_right, m_bottom );
            }
        }
        break;

    case 8:
        if ( m_previousSector == 3 ) {
            QPointF pointLeft = clipLeft( m, m_previousPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );

            if ( pointLeft.y() < m_bottom ) {
                m_clippedObject << pointLeft;
            } else {
                m_clippedObject << QPointF( m_left, m_bottom );
            }
            if ( pointBottom.x() > m_left && pointBottom.x() <= m_right )
                m_clippedObject << pointBottom;
            if ( pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
        }
        else if ( m_previousSector == 1 ) {
            QPointF pointTop = clipTop( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_currentPoint );

            if ( pointTop.x() < m_right ) {
                m_clippedObject << pointTop;
            } else {
                m_clippedObject << QPointF( m_right, m_top );
            }
            if ( pointRight.y() > m_top && pointRight.y() <= m_bottom ) 
                m_clippedObject << pointRight;
            if ( pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
        }
        else if ( m_previousSector == 0 ) {
            QPointF pointTop = clipTop( m, m_currentPoint );
            QPointF pointLeft = clipLeft( m, m_currentPoint );
            QPointF pointBottom = clipBottom( m, m_previousPoint );
            QPointF pointRight = clipRight( m, m_previousPoint );

            if ( pointTop.x() > m_left && pointTop.x() < m_right ) 
                m_clippedObject << pointTop;
            if ( pointLeft.y() > m_top && pointLeft.y() < m_bottom ) 
                m_clippedObject << pointLeft;
            if ( pointBottom.x() > m_left && pointBottom.x() < m_right )
                m_clippedObject << pointBottom;
            if ( pointRight.y() > m_top && pointRight.y() < m_bottom ) 
                m_clippedObject << pointRight;
   
            if ( pointBottom.x() <= m_left && pointLeft.y() >= m_bottom )
                m_clippedObject << QPointF( m_left, m_bottom );
            if ( pointTop.x() >= m_right && pointRight.y() <= m_top )
                m_clippedObject << QPointF( m_right, m_top );
        }

        m_clippedObject << QPointF( m_right, m_bottom );
        break;

    default:
        break;				
    }
}


void ClipPainterPrivate::clipOnce()
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
        if ( m_currentSector == 4) {
            m_clippedObject << QPoint( m_left, m_top );
            m_clippedObject << point;
        } else {
            m_clippedObject << point;
            m_clippedObject << QPoint( m_left, m_top );
        }
        break;
    case 1: // top
        point = clipTop( m, m_previousPoint );
        m_clippedObject << point;
        break;
    case 2: // topright
        point = clipTop( m, m_previousPoint );
        if ( point.x() > m_right ) {
            point = clipRight( m, point );
        }
        if ( m_currentSector == 4) {
            m_clippedObject << QPoint( m_right, m_top );
            m_clippedObject << point;
        } else {
            m_clippedObject << point;
            m_clippedObject << QPoint( m_right, m_top );
        }
        break;
    case 3: // left
        point = clipLeft( m, m_previousPoint );
        m_clippedObject << point;
        break;
    case 5: // right
        point = clipRight( m, m_previousPoint );
        m_clippedObject << point;
        break;
    case 6: // bottomleft
        point = clipBottom( m, m_previousPoint );
        if ( point.x() < m_left ) {
            point = clipLeft( m, point );
        }
        if ( m_currentSector == 4) {
            m_clippedObject << QPoint( m_left, m_bottom );
            m_clippedObject << point;
        } else {
            m_clippedObject << point;
            m_clippedObject << QPoint( m_left, m_bottom );
        }
        break;
    case 7: // bottom
        point = clipBottom( m, m_previousPoint );
        m_clippedObject << point;
        break;
    case 8: // bottomright
        point = clipBottom( m, m_previousPoint );
        if ( point.x() > m_right ) {
            point = clipRight( m, point );
        }
        if ( m_currentSector == 4) {
            m_clippedObject << QPoint( m_right, m_bottom );
            m_clippedObject << point;
        } else {
            m_clippedObject << point;
            m_clippedObject << QPoint( m_right, m_bottom );
        }
        break;
    default:
        break;			
    }

}
