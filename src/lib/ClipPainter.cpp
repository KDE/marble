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
    // true if clipping is on.
    bool    m_doClip;

    // The limits
    qreal  m_left;
    qreal  m_right;
    qreal  m_top;
    qreal  m_bottom;

    // Used in the paint process of vectors..
    int     m_currentSector;
    int     m_lastSector;

    //	int m_debugNodeCount;

    QPointF    m_lastBorderPoint;
    QPointF    m_currentPoint;
    QPointF    m_lastPoint; 

    // The resulting object from the clipping operation
    QPolygonF  m_clippedObject;
};

}

using namespace Marble;

// #define MARBLE_DEBUG

ClipPainter::ClipPainter(QPaintDevice * pd, bool clip)
    : QPainter( pd ), d(new ClipPainterPrivate)
{
    d->m_left   = -1.0; 
    d->m_right  = (qreal)(pd->width());
    d->m_top    = -1.0; 
    d->m_bottom = (qreal)(pd->height());

    // m_debugNodeCount = 0;
    d->m_doClip = clip;
    // penblue.setColor(QColor( 0, 0, 255, 255));
    // pengreen.setColor(QColor( 0, 255, 0, 255));
}


ClipPainter::ClipPainter()
    : d(new ClipPainterPrivate)
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
    if ( d->m_doClip ) {	
        clipPolyObject( polygon );
        if ( d->m_clippedObject.size() > 2 ) {
            // qDebug() << "Size: " << m_clippedObject.size();
            QPainter::drawPolygon ( d->m_clippedObject, fillRule );
            // qDebug() << "done";
        }
    }
    else
        QPainter::drawPolygon ( polygon, fillRule );
}


void ClipPainter::drawPolyline( const QPolygonF & polygon )
{
    if ( d->m_doClip ) {	
        clipPolyObject( polygon );
        if ( d->m_clippedObject.size() > 1 ) {
            // qDebug() << "Size: " << m_clippedObject.size();
            QPainter::drawPolyline ( d->m_clippedObject );
            // qDebug() << "done";
        }
    }
    else
        QPainter::drawPolyline( polygon );
}


void ClipPainter::clipPolyObject ( const QPolygonF & polygon )
{

    // If we think of the image borders as (infinitly long) parallel
    // lines then the plane is divided into 9 sectors.  Each of these
    // sections is identified by a unique keynumber (currentSector):
    //
    //	0 | 1 | 2
    //  --+---+--
    //	3 | 4 | 5 <- sector number "4" represents the onscreen sector
    //  --+---+--
    //	6 | 7 | 8
    //

    //	qDebug() << "ClipPainter enabled." ;
    d->m_clippedObject.clear();

    QVector<QPointF>::const_iterator        itPoint;
    const QVector<QPointF>::const_iterator  itStartPoint = polygon.begin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.end();

    for ( itPoint = itStartPoint; itPoint != itEndPoint; ++itPoint ) {

        d->m_currentPoint = (*itPoint);
        // qDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the section of the current point.
        int m_currentXSector = 1;
        if ( d->m_currentPoint.x() < d->m_left )
            m_currentXSector = 0;
        else if ( d->m_currentPoint.x() > d->m_right ) 
            m_currentXSector = 2;
							
        int m_currentYSector = 3;
        if ( d->m_currentPoint.y() < d->m_top ) 
            m_currentYSector = 0;
        else if ( d->m_currentPoint.y() > d->m_bottom ) 
            m_currentYSector = 6;

        d->m_currentSector = m_currentYSector + m_currentXSector;

        // Initialize a few remaining variables.
        if ( itPoint == itStartPoint ) {
            d->m_lastSector = d->m_currentSector;
        }

        // If the current point reaches a new sector, take care of clipping.
        if ( d->m_currentSector != d->m_lastSector ) {
            if ( d->m_currentSector == 4 || d->m_lastSector == 4 ) {
                d->m_lastBorderPoint = borderPoint();
                d->m_clippedObject << d->m_lastBorderPoint;
            }
            else
                manageOffScreen();		

            d->m_lastSector = d->m_currentSector;
        }

        // If the current point is onscreen, just add it to our final polygon.
        if ( d->m_currentSector == 4 ) {
            d->m_clippedObject << d->m_currentPoint;
#ifdef MARBLE_DEBUG
            ++(d->m_debugNodeCount);
#endif
        }

        d->m_lastPoint = d->m_currentPoint;
    }
}


void ClipPainter::manageOffScreen()
{
    // Take care of adding nodes in the image corners if the iterator 
    // traverses offscreen sections.

    // FIXME:	- bugs related to vertical and horizontal lines in corners  
    //		- borderpoint order

    qreal  xa = 0;
    qreal  ya = 0;

    // Avoid dereferencing all the time
    const qreal m_left = d->m_left;
    const qreal m_right = d->m_right;
    const qreal m_top = d->m_top;
    const qreal m_bottom = d->m_bottom;
    int m_lastSector = d->m_lastSector;

    QPolygonF&  m_clippedObject = d->m_clippedObject;
    QPointF&    m_lastBorderPoint = d->m_lastBorderPoint;
    QPointF&    m_currentPoint = d->m_currentPoint;
    QPointF&    m_lastPoint = d->m_lastPoint;

    // Calculating the slope
    qreal  divisor = m_currentPoint.x() - m_lastPoint.x();
    if ( std::fabs( divisor ) < 0.000001 )
        // FIXME: Is this ok even if divisor < 0?
        divisor = 0.000001;

    qreal  m = ( m_currentPoint.y() - m_lastPoint.y() ) / divisor;

    switch ( d->m_currentSector ) {
    case 0:
        m_clippedObject << QPointF( m_left, m_top );
        break;

    case 1:
        if ( m_lastSector == 3 ) { // case checked
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya < m_top )
                m_clippedObject << QPointF( m_left, m_top );
            else if ( m_lastBorderPoint.x() == m_left )
                m_clippedObject << QPointF( m_left, ya )
                                << QPointF( xa, m_top );
            else
                m_clippedObject << QPointF( xa, m_top )
                                << QPointF( m_left, ya );
        }
        else if ( m_lastSector == 5 ) { // case checked
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya < m_top )
                m_clippedObject << QPointF( m_right, m_top );
            else if ( m_lastBorderPoint.x() == m_right )
                m_clippedObject << QPointF( m_right, ya )
                                << QPointF( xa, m_top );
            else
                m_clippedObject << QPointF( xa, m_top )
                                << QPointF( m_right, ya );
        }
        break;

    case 2:
        m_clippedObject << QPointF( m_right, m_top );
        break;

    case 3:
        if ( m_lastSector == 1 ) {
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya < m_top )
                m_clippedObject << QPointF( m_left, m_top );
            else if ( m_lastBorderPoint.x() == m_left )
                m_clippedObject << QPointF( m_left, ya )
                                << QPointF( xa, m_top );
            else
                m_clippedObject << QPointF( xa, m_top )
                                << QPointF( m_left, ya );
        }
        else if ( m_lastSector == 7 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya > m_bottom )
                m_clippedObject << QPointF( m_left, m_bottom );
            else {
                if ( m_lastBorderPoint.x() == m_left )
                    m_clippedObject << QPointF( m_left, ya ) 
                              << QPointF( xa, m_bottom );
                else
                    m_clippedObject << QPointF( xa, m_bottom ) 
                              << QPointF( m_left, ya );
            }
        }
        break;
				
    case 5:
        if ( m_lastSector == 1 ) { // case checked
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya < m_top )
                m_clippedObject << QPointF( m_right, m_top );
            else if ( m_lastBorderPoint.x() == m_right )
                m_clippedObject << QPointF( m_right, ya )
                                << QPointF( xa, m_top );
            else
                m_clippedObject << QPointF( xa, m_top )
                                << QPointF( m_right, ya );
        }
        else if ( m_lastSector == 7 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya > m_bottom )
                m_clippedObject << QPointF( m_right, m_bottom );
            else if ( m_lastBorderPoint.x() == m_right )
                m_clippedObject << QPointF( m_right, ya )
                                << QPointF( xa, m_bottom );
            else
                m_clippedObject << QPointF( xa, m_bottom )
                                << QPointF( m_right, ya );
        }
        break;

    case 6:
        m_clippedObject << QPointF( m_left, m_bottom );
        break;

    case 7:
        if ( m_lastSector == 3 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya > m_bottom )
                m_clippedObject << QPointF( m_left, m_bottom );
            else if ( m_lastBorderPoint.x() == m_left )
                m_clippedObject << QPointF( m_left, ya ) 
                                << QPointF( xa, m_bottom );
            else
                m_clippedObject << QPointF( xa, m_bottom )
                                << QPointF( m_left, ya );
        }
        else if ( m_lastSector == 5 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya > m_bottom )
                m_clippedObject << QPointF( m_right, m_bottom );
            else if ( m_lastBorderPoint.x() == m_right )
                m_clippedObject << QPointF( m_right, ya )
                                << QPointF( xa, m_bottom );
            else
                m_clippedObject << QPointF( xa, m_bottom )
                                << QPointF( m_right, ya );
        }
        break;

    case 8:
        m_clippedObject << QPointF( m_right, m_bottom );
        break;

    default:
        break;				
    }
}


const QPointF ClipPainter::borderPoint()
{
    //	Interpolate border points (linear interpolation)

    qreal  xa = 0;
    qreal  ya = 0;

    // Avoid dereferencing all the time
    const qreal m_left = d->m_left;
    const qreal m_right = d->m_right;
    const qreal m_top = d->m_top;
    const qreal m_bottom = d->m_bottom;

    const QPointF&    m_currentPoint = d->m_currentPoint;
    const QPointF&    m_lastPoint = d->m_lastPoint;

    // Calculating the slope.
    qreal m = ( m_currentPoint.y() - m_lastPoint.y() ) 
        / ( m_currentPoint.x() - m_lastPoint.x() );

    int offscreenpos = ( d->m_currentSector == 4 ) ? d->m_lastSector : d->m_currentSector;

    // "Rise over run" for all possible situations .
    switch ( offscreenpos ) {
    case 0: // topleft
        xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
        ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
        xa = ( xa < m_left ) ? m_left : xa; 
        ya = ( ya < m_top ) ? m_top : ya; 
        break;
    case 1: // top
        xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
        ya = m_top;
        break;
    case 2: // topright
        xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
        ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
        xa = ( xa > m_right ) ? m_right : xa; 
        ya = ( ya < m_top ) ? m_top : ya; 
        break;
    case 3: // left
        xa = m_left;
        ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
        break;
    case 5: // right
        xa = m_right;
        ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
        break;
    case 6: // bottomleft
        xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
        ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
        xa = ( xa < m_left ) ? m_left : xa; 
        ya = ( ya > m_bottom ) ? m_bottom : ya; 
        break;
    case 7: // bottom
        xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
        ya = m_bottom;
        break;
    case 8: // bottomright
        xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
        ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
        xa = ( xa > m_right ) ? m_right : xa; 
        ya = ( ya > m_bottom ) ? m_bottom : ya; 
        break;
    default:
        break;			
    }

    return QPointF( xa, ya ); 	
}
