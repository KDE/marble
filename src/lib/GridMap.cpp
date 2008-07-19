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

#include "GridMap.h"

#include <cmath>
#include <stdlib.h>

#include <QtCore/QVector>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtGui/QColor>

#include "global.h"
#include "GeoPainter.h"
#include "AbstractProjection.h"
#include "ViewParams.h"
#include "ViewportParams.h"

// Except for the equator the major circles of latitude are defined via 
// the earth's axial tilt, which currently measures about 23°26'21".
 
const double  AXIALTILT = DEG2RAD * ( 23.0
				      + 26.0 / 60.0
				      + 21.0 / 3600.0 );
const double  PIHALF    = M_PI / 2.0;


GridMap::GridMap()
{
    //	Initialising booleans for horizoncrossing
    m_lastVisible      = false;
    m_currentlyVisible = false;

    m_pen = QPen(QColor( 255, 255, 255, 128));
}

GridMap::~GridMap()
{
}


void GridMap::createTropics( ViewportParams* viewport )
{
    clear();
    viewport->planetAxis().inverse().toMatrix( m_planetAxisMatrix );

    // Turn on the major circles of latitude if we've zoomed in far
    // enough (radius > 400 pixels)
    if ( viewport->radius() >  400 ) {
        int  precision = getPrecision( viewport );

        // Arctic Circle
        createCircle( PIHALF - AXIALTILT, Latitude, precision, viewport );

        // Antarctic Circle
        createCircle( AXIALTILT - PIHALF, Latitude, precision, viewport );

        // Tropic of Cancer 
        createCircle( AXIALTILT, Latitude, precision, viewport ); 

        // Tropic of Capricorn
        createCircle( -AXIALTILT, Latitude, precision, viewport );
    }
}

void GridMap::createEquator( ViewportParams* viewport ) 
{
    clear();
    viewport->planetAxis().inverse().toMatrix( m_planetAxisMatrix );

    int  precision = getPrecision( viewport );
    createCircle( 0.0 , Latitude, precision, viewport );
}

void GridMap::createGrid( ViewportParams* viewport )
{
    clear();
    viewport->planetAxis().inverse().toMatrix( m_planetAxisMatrix );

    //	FIXME:	- Higher precision after optimization 
    //		  ( will keep grid lines from vanishing at high zoom levels ) 

    int  radius = viewport->radius();
    int  precision = getPrecision( viewport );
    if ( radius > 3200 ) {
        createCircles( 32, 24, precision, viewport );
    } 
    else if ( radius > 1600 ) {
        createCircles( 16, 12, precision, viewport );
    }	
    else if ( radius >  700 ) {
        createCircles( 8, 6, precision, viewport );
    }	
    else if ( radius >  400 ) {
        createCircles( 4, 3, precision, viewport );
    }	
    else if ( radius >  100 ) {
        createCircles( 2, 3, precision, viewport );
    }	
    else {
        createCircles( 2, 1, precision, viewport );
    }
}


void GridMap::createCircles( const int lonNum, const int latNum,
                             int precision,
                             ViewportParams *viewport )
{
    // latNum: number of latitude  circles between lat = 0 deg and lat < 90 deg
    // lonNum: number of longitude circles between lon = 0 deg and lon < 90 deg

    if ( latNum > 0 ) {

        // Circles of latitude:
        for ( int i = 1; i < latNum; ++i ) {
            createCircle( +i * PIHALF / latNum, Latitude, precision, viewport );
            createCircle( -i * PIHALF / latNum, Latitude, precision, viewport );
        } 
    } 

    if ( lonNum > 0 ) {

        // Universal prime meridian and its orthogonal great circle:
        createCircle( +0,      Longitude, precision, viewport );
        createCircle( +PIHALF, Longitude, precision, viewport );	

        for ( int i = 1; i < lonNum; ++i ) {
            double cutOff = PIHALF / (double)(latNum);
            createCircle( +i * PIHALF / lonNum, Longitude, 
                          precision, viewport, cutOff );
            createCircle( -i * PIHALF / lonNum, Longitude,
                          precision, viewport, cutOff );	
        }
    }
}

void GridMap::createCircle( double angle, SphereDim dim,
                            int precision,
                            ViewportParams *viewport, double cutOff)
{
    switch( viewport->projection() ) {
        case Spherical:
            sphericalCreateCircle( angle, dim, precision, viewport, cutOff );
            break;
        case Equirectangular:
        case Mercator:
	    flatCreateCircle( angle, dim, precision, viewport, cutOff );
            break;
    }
}

void GridMap::sphericalCreateCircle( double angle, SphereDim dim,
                                     int precision,
                                     ViewportParams *viewport, double cutOff )
{
    // cutOff: the amount of each quarter circle that is cut off at
    //         the pole in radians

    const double cutCoeff   = 1.0 - cutOff / PIHALF;

    // We draw each circle in quarters ( or parts of those ).
    // This is especially convenient for the great longitude circles which 
    // are being cut off close to the poles.
    // quartSteps: the number of nodes in a "quarter" of a circle.

    const double quartSteps = (double) precision;

    double coeff  = 1.0;
    double offset = 0.0;

    // Some convenience variables
    int  imgWidth  = viewport->width();
    int  imgHeight = viewport->height();
    int  radius    = viewport->radius();

    const int steps = (int) ( cutCoeff * quartSteps );
    for ( int i = 0; i < 4; ++i ) {

        if ( i > 1 ) 
            coeff = - 1.0;
        offset = ( i % 2 ) ? 1.0 : 0.0;

        m_polygon.clear();
        m_polygon.reserve( steps + 1 );

        Quaternion qpos;
        for ( int j = 0; j < steps + 1; ++j ) {

            double itval  = (j != steps) ? (double)(j) / quartSteps : cutCoeff;
            double dimVal = coeff * ( PIHALF * fabs( offset - itval ) + offset * PIHALF );

            double lat = ( dim == Latitude )  ? angle : dimVal;
            double lon = ( dim == Longitude ) ? angle : dimVal;

            qpos.set( lon, -lat );
            qpos.rotateAroundAxis(m_planetAxisMatrix);

            m_currentPoint = QPointF( (double)(imgWidth / 2 + radius * qpos.v[Q_X]),
                                      (double)(imgHeight / 2 - radius * qpos.v[Q_Y]) );
            //qDebug() << "Radius: " << radius
            //         << "QPointF(" << (double)(imgWidth / 2 + radius*qpos.v[Q_X])+1
            //        << ", " << (double)(imgHeight / 2 + radius*qpos.v[Q_Y])+1 << ")";

            // Take care of horizon crossings if horizon is visible.
            m_lastVisible = m_currentlyVisible;
            m_currentlyVisible = (qpos.v[Q_Z] >= 0) ? true : false;

            // Initialize crossing of the horizon.
            if ( j == 0 ) {

                m_lastVisible = m_currentlyVisible;

                // Initially m_lastPoint MUST NOT equal m_currentPoint
                m_lastPoint = QPointF( m_currentPoint.x(), 
                                       m_currentPoint.y() );
            }

            if ( m_currentlyVisible != m_lastVisible ) {
                m_polygon << horizonPoint( viewport );

                if ( m_polygon.size() >= 2 ) {
                    append( m_polygon );
                }

                m_polygon.clear();

                if ( m_lastVisible == true )
                    break;
            }

            // Take care of screencrossing crossings if horizon is visible.
            // Filter points which aren't on the visible hemisphere.
            if ( m_currentlyVisible ) {
                // most recent addition: m_currentPoint != m_lastPoint
                //			qDebug("accepted");
                m_polygon << m_currentPoint;
            }

            m_lastPoint = m_currentPoint;
        }

        if ( m_polygon.size() >= 2 ) {
            append( m_polygon );
        }
    }
}

// This function is used for all projections that are flat.
// Currently that means:
//  - Equirectangular
//  - Mercator
//
void GridMap::flatCreateCircle( double angle, SphereDim dim,
				int precision,
				ViewportParams *viewport, double cutOff )
{
    // Only used in spherical projection.
    Q_UNUSED( precision );
    Q_UNUSED( cutOff );

    // Convenience variables
    AbstractProjection  *currentProjection = viewport->currentProjection();
    int                  imgHeight = viewport->height();
    int                  imgWidth  = viewport->width();
    int                  radius    = viewport->radius();

    int  dummy;

    m_polygon.clear();
    if ( dim == Latitude ) {
        // FIXME: At some point add a
        //        AbstractProjection::screenCoordinates() that produce
        //        qreals, and then make y and x below qreals instead.
        int  y;

        currentProjection->screenCoordinates( 0.0, angle, viewport, 
                           dummy, y );

        QPointF  startPoint( 0.0f,     (double)y );
        QPointF  endPoint(   imgWidth, (double)y );
        m_polygon << startPoint << endPoint;
        append( m_polygon );
    }
    else {
        // dim == Longitude

        int  x;
        int  yTop;
        int  yBottom;

        currentProjection->screenCoordinates( angle, 0.0, viewport, 
                            x, dummy );

        // Get the top and bottom of the map.  Clip it to the screen
        currentProjection->screenCoordinates( 0.0, currentProjection->maxLat(),
                            viewport,
                            dummy, yTop );
        if ( yTop < 0 )
            yTop = 0;
        currentProjection->screenCoordinates( 0.0, -currentProjection->maxLat(),
                            viewport,
                            dummy, yBottom );
        if ( yBottom > imgHeight )
            yBottom = imgHeight;
        
        // If we are far zoomed out, then there may be repetition in
        // the X direction.

        // Normally we would add 2*PI (4*radius) but in this case a
        // circle is shown as a line every half (the front and the
        // back).

        while ( x > 2 * radius ) 
            x -= 2 * radius;
        while ( x < imgWidth ) {
            QPointF  startPoint( x, yTop );
            QPointF  endPoint(   x, yBottom );

            m_polygon << startPoint << endPoint;
            append( m_polygon );

            // Set up for next pass through the loop
            x += 2 * radius;
            m_polygon.clear();
        }
    }
}


void GridMap::paintGridMap(GeoPainter * painter, bool antialiasing)
{
    if ( size() == 0 )
        return;

    if ( antialiasing == true )
        painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setPen(m_pen);

    ScreenPolygon::Vector::const_iterator  itEndPolygon = end();
    for ( ScreenPolygon::Vector::const_iterator itPolygon=begin(); 
          itPolygon != itEndPolygon;
          ++itPolygon )
    {
        painter->drawPolyline(*itPolygon);
    }

    if (antialiasing)
        painter->setRenderHint( QPainter::Antialiasing, false );
}


int GridMap::getPrecision( ViewportParams *viewport )
{
    int  radius = viewport->radius();

    if ( radius > 3200 ) {
        return 40;
    } 
    else if ( radius > 1600 ) {
        return 30;
    }	
    else if ( radius >  700 ) {
        return 30;
    }	
    else if ( radius >  400 ) {
        return 20;
    }	
    else if ( radius >  100 ) {
        return 10;
    }

    return 10;
}

const QPointF GridMap::horizonPoint( ViewportParams *viewport)
{
    // qDebug("Interpolating");
    double  xa = 0;
    double  ya = 0;

    xa = m_currentPoint.x() - ( viewport->width() / 2 ) ;

    // Move the m_currentPoint along the y-axis to match the horizon.
    double  radius   = (double)(viewport->radius());
    double  radicant = radius * radius - xa * xa;
    if ( radicant > 0 )
        ya = sqrt( radicant );

    if ( ( m_currentPoint.y() - ( viewport->height() / 2 ) ) < 0 )
        ya = -ya; 

    return QPointF( (double)viewport->width()  / 2 + xa,
                    (double)viewport->height() / 2 + ya );
}
