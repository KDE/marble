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
    : QPainter( pd )
{
    m_imgWidth       = pd->width(); 
    m_imgHeight      = pd->height();
    m_currentXSector = 0;
    m_currentYSector = 0;

    m_left   = -1.0; 
    m_right  = (double)(m_imgWidth);
    m_top    = -1.0; 
    m_bottom = (double)(m_imgHeight);	

    // m_debugNodeCount = 0;
    m_doClip = clip;
    // penblue.setColor(QColor( 0, 0, 255, 255));
    // pengreen.setColor(QColor( 0, 255, 0, 255));
}


void ClipPainter::setClipping(bool enable)
{
    m_doClip = enable;
}


bool ClipPainter::isClipping() const
{
    return m_doClip;
}


void ClipPainter::drawPolygon ( const QPolygonF & polygon,
                                Qt::FillRule fillRule )
{
    if ( m_doClip ) {	
        clipPolyObject( polygon );
        if ( m_clippedObject.size() > 2 ) {
            // qDebug() << "Size: " << m_clippedObject.size();
            QPainter::drawPolygon ( m_clippedObject, fillRule );
            // qDebug() << "done";
        }
    }
    else
        QPainter::drawPolygon ( polygon, fillRule );
}


void ClipPainter::drawPolyline( const QPolygonF & polygon )
{
    if ( m_doClip ) {	
        clipPolyObject( polygon );
        if ( m_clippedObject.size() > 1 ) {
            // qDebug() << "Size: " << m_clippedObject.size();
            QPainter::drawPolyline ( m_clippedObject );
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
    m_clippedObject.clear();

    QVector<QPointF>::const_iterator        itPoint;
    const QVector<QPointF>::const_iterator  itStartPoint = polygon.begin();
    const QVector<QPointF>::const_iterator  itEndPoint   = polygon.end();

    for ( itPoint = itStartPoint; itPoint != itEndPoint; ++itPoint ) {

        m_currentPoint = (*itPoint);
        // qDebug() << "m_currentPoint.x()" << m_currentPoint.x() << "m_currentPOint.y()" << m_currentPoint.y();

        // Figure out the section of the current point.
        m_currentXSector = 1;
        if ( m_currentPoint.x() < m_left )
            m_currentXSector = 0;
        else if ( m_currentPoint.x() > m_right ) 
            m_currentXSector = 2;
							
        m_currentYSector = 3;
        if ( m_currentPoint.y() < m_top ) 
            m_currentYSector = 0;
        else if ( m_currentPoint.y() > m_bottom ) 
            m_currentYSector = 6;

        m_currentSector = m_currentYSector + m_currentXSector;

        // Initialize a few remaining variables.
        if ( itPoint == itStartPoint ) {
            m_lastSector = m_currentSector;
        }

        // If the current point reaches a new sector, take care of clipping.
        if ( m_currentSector != m_lastSector ) {
            if ( m_currentSector == 4 || m_lastSector == 4 ) {
                m_lastBorderPoint = borderPoint();
                m_clippedObject << m_lastBorderPoint;
            }
            else
                manageOffScreen();		

            m_lastSector = m_currentSector;
        }

        // If the current point is onscreen, just add it to our final polygon.
        if ( m_currentSector == 4 ) {
            m_clippedObject << m_currentPoint;
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
    if ( std::fabs( divisor ) < 0.000001 )
        // FIXME: Is this ok even if divisor < 0?
        divisor = 0.000001;

    double  m = ( m_currentPoint.y() - m_lastPoint.y() ) / divisor;

    switch ( m_currentSector ) {
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

    double  xa = 0;
    double  ya = 0;

    // Calculating the slope.
    double m = ( m_currentPoint.y() - m_lastPoint.y() ) 
        / ( m_currentPoint.x() - m_lastPoint.x() );

    int offscreenpos = ( m_currentSector == 4 ) ? m_lastSector : m_currentSector;

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
