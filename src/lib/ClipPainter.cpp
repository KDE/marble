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


// #define MARBLE_DEBUG

ClipPainter::ClipPainter(QPaintDevice * pd, bool clip)
    : QPainter(pd)
{
    m_imgwidth    = pd -> width(); 
    m_imgheight   = pd -> height();
    m_currentxpos = 0;
    m_currentypos = 0;

    m_left   = 0.0; 
    m_right  = (double)(m_imgwidth);
    m_top    = 0.0; 
    m_bottom = (double)(m_imgheight);	

    // m_debugNodeCount = 0;
    m_clip = clip;
    // penblue.setColor(QColor( 0, 0, 255, 255));
    // pengreen.setColor(QColor( 0, 255, 0, 255));
}


void ClipPainter::drawPolygon ( const QPolygonF & pa, Qt::FillRule fillRule )
{

    if ( m_clip == true ) {	
        drawPolyobject( pa );
        if (m_clipped.size() > 2) {
            // qDebug() << "Size: " << m_clipped.size();
            QPainter::drawPolygon ( m_clipped, fillRule );
            // qDebug() << "done";
        }
    }
    else
        QPainter::drawPolygon ( pa, fillRule );
}


void ClipPainter::drawPolyline( const QPolygonF & pa )
{
    if ( m_clip == true ) {	
        drawPolyobject( pa );
        if (m_clipped.size() > 1) {
            // qDebug() << "Size: " << m_clipped.size();
            QPainter::drawPolyline ( m_clipped );
            // qDebug() << "done";
        }
    }
    else
        QPainter::drawPolyline( pa );
}


void ClipPainter::drawPolyobject ( const QPolygonF & pa )
{

    /* If we think of the image borders as (infinetly long) parallels
     * then the on- and offscreen are divided into 9 sections. Each of
     * these sections is identified by a unique keynumber
     * (currentpos):
     *
     *	012
     *	345 <- keynumber "4" represents the onscreen section
     *	678
     */
    //	qDebug() << "ClipPainter enabled." ;
    m_clipped.clear();

    QVector<QPointF>::const_iterator        itPoint;
    const QVector<QPointF>::const_iterator  itStartPoint = pa.begin();
    const QVector<QPointF>::const_iterator  itEndPoint   = pa.end();

    for ( itPoint = itStartPoint; itPoint != itEndPoint; ++itPoint ) {

        m_currentPoint = (*itPoint);
        // qDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the section of the current point.
        m_currentxpos = 1;
        if ( m_currentPoint.x() < m_left )
            m_currentxpos = 0;
        if ( m_currentPoint.x() > m_right ) 
            m_currentxpos = 2;
							
        m_currentypos = 3;
        if ( m_currentPoint.y() < m_top ) 
            m_currentypos = 0;
        if ( m_currentPoint.y() > m_bottom ) 
            m_currentypos = 6;

        m_currentpos = m_currentypos + m_currentxpos;

        // Initialize a few remaining variables.
        if ( itPoint == itStartPoint ) {
            m_lastpos = m_currentpos;
        }

        // If the current point reaches a new section, take care of clipping.
        if ( m_currentpos != m_lastpos ) {
            if ( m_currentpos == 4 || m_lastpos == 4 ) {
                m_lastBorderPoint = borderPoint();
                m_clipped << m_lastBorderPoint;
            }
            else
                manageOffScreen();		

            m_lastpos = m_currentpos;
        }

        // If the current point is onscreen, just add it to our final polygon.
        if ( m_currentpos == 4 ) {
            m_clipped << m_currentPoint;
#ifdef MARBLE_DEBUG
            ++m_debugNodeCount;
#endif
        }

        m_lastPoint = m_currentPoint;
    }
}


void ClipPainter::manageOffScreen()
{
    // Take care of adding nodes in the image corners if the iterator 
    // traverses offscreen sections.

    // FIXME:	- bugs related to vertical and horizontal lines in corners  
    //		- borderpoint order

    double  xa = 0;
    double  ya = 0;

    // Calculating the slope
    double  divisor = m_currentPoint.x() - m_lastPoint.x();
    if ( fabs( divisor ) < 0.000001 )
        divisor = 0.000001;

    double  m = ( m_currentPoint.y() - m_lastPoint.y() ) / divisor;

    switch ( m_currentpos ) {
    case 0:
        m_clipped << QPointF( m_left, m_top );
        break;
    case 1:
        if ( m_lastpos == 3 ) { // case checked
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya < m_top )
                m_clipped << QPointF( m_left, m_top );
            else {
                if ( m_lastBorderPoint.x() == m_left )
                    m_clipped << QPointF( m_left, ya ) << QPointF( xa, m_top );
                else
                    m_clipped << QPointF( xa, m_top ) << QPointF( m_left, ya );
            }
        }
        if ( m_lastpos == 5 ) { // case checked
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya < m_top )
                m_clipped << QPointF( m_right, m_top );
            else {
                if ( m_lastBorderPoint.x() == m_right )
                    m_clipped << QPointF( m_right, ya )
                              << QPointF( xa, m_top );
                else
                    m_clipped << QPointF( xa, m_top )
                              << QPointF( m_right, ya );
            }
        }
        break;
    case 2:
        m_clipped << QPointF( m_right, m_top );
        break;
    case 3:
        if ( m_lastpos == 1 ) {
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya < m_top )
                m_clipped << QPointF( m_left, m_top );
            else {
                if ( m_lastBorderPoint.x() == m_left )
                    m_clipped << QPointF( m_left, ya ) << QPointF( xa, m_top );
                else
                    m_clipped << QPointF( xa, m_top ) << QPointF( m_left, ya );
            }
        }
        if ( m_lastpos == 7 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya > m_bottom )
                m_clipped << QPointF( m_left, m_bottom );
            else {
                if ( m_lastBorderPoint.x() == m_left )
                    m_clipped << QPointF( m_left, ya ) 
                              << QPointF( xa, m_bottom );
                else
                    m_clipped << QPointF( xa, m_bottom ) 
                              << QPointF( m_left, ya );
            }
        }
        break;
				
    case 5:
        if ( m_lastpos == 1 ) { // case checked
            xa = m_lastPoint.x() + ( m_top - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya < m_top )
                m_clipped << QPointF( m_right, m_top );
            else {
                if ( m_lastBorderPoint.x() == m_right )
                    m_clipped << QPointF( m_right, ya )
                              << QPointF( xa, m_top );
                else
                    m_clipped << QPointF( xa, m_top )
                              << QPointF( m_right, ya );
            }
        }
        if ( m_lastpos == 7 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya > m_bottom )
                m_clipped << QPointF( m_right, m_bottom );
            else {
                if ( m_lastBorderPoint.x() == m_right )
                    m_clipped << QPointF( m_right, ya )
                              << QPointF( xa, m_bottom );
                else
                    m_clipped << QPointF( xa, m_bottom )
                              << QPointF( m_right, ya );
            }
        }
        break;
    case 6:
        m_clipped << QPointF( m_left, m_bottom );
        break;
    case 7:
        if ( m_lastpos == 3 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_left - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa < m_left && ya > m_bottom )
                m_clipped << QPointF( m_left, m_bottom );
            else {
                if ( m_lastBorderPoint.x() == m_left )
                    m_clipped << QPointF( m_left, ya ) 
                              << QPointF( xa, m_bottom );
                else
                    m_clipped << QPointF( xa, m_bottom )
                              << QPointF( m_left, ya );
            }
        }
        if ( m_lastpos == 5 ) {
            xa = m_lastPoint.x() + ( m_bottom - m_lastPoint.y() ) / m;
            ya = m * ( m_right - m_lastPoint.x() ) + m_lastPoint.y();
            if ( xa > m_right && ya > m_bottom )
                m_clipped << QPointF( m_right, m_bottom );
            else {
                if ( m_lastBorderPoint.x() == m_right )
                    m_clipped << QPointF( m_right, ya )
                              << QPointF( xa, m_bottom );
                else
                    m_clipped << QPointF( xa, m_bottom )
                              << QPointF( m_right, ya );
            }
        }
        break;
    case 8:
        m_clipped << QPointF( m_right, m_bottom );
        break;
    default:
        break;				
    }
}


const QPointF ClipPainter::borderPoint()
{
    //	Interpolate border points (linear interpolation)

    double  xa = 0;
    double  ya = 0;

    // Calculating the slope.
    double m = ( m_currentPoint.y() - m_lastPoint.y() ) 
        / ( m_currentPoint.x() - m_lastPoint.x() );

    int offscreenpos = (m_currentpos == 4) ? m_lastpos : m_currentpos;

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
