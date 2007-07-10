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

#include "clippainter.h"
// Except for the equator the major circles of latitude are defined via 
// the earth's axial tilt, which currently measures about 23°26'21".
 
const double  AXIALTILT = M_PI / 180.0 * ( 23.0
                                           + 26.0 / 60.0
                                           + 21.0 / 3600.0 );
const double  PIHALF    = M_PI / 2;


GridMap::GridMap()
{
    m_imageHalfWidth = 0;
    m_imageHalfHeight = 0;
    m_imageRadius = 0;

    //	Initialising booleans for horizoncrossing
    m_lastVisible      = false;
    m_currentlyVisible = false;

    m_radius = 0; 

    m_pen = QPen(QColor( 255, 255, 255, 128));
    m_precision = 10;
}

GridMap::~GridMap()
{
}


void GridMap::createTropics(const int& radius, Quaternion& planetAxis)
{
    clear();
    m_radius = radius - 1;
    planetAxis.inverse().toMatrix( m_planetAxisMatrix );
	
    // Turn on the major circles of latitude if we've zoomed in far
    // enough (radius > 400 pixels)
    if ( m_radius >  400 ) {
        createCircle( PIHALF - AXIALTILT , Latitude ); // Arctic Circle
        createCircle( AXIALTILT - PIHALF , Latitude ); // Antarctic Circle
        createCircle( AXIALTILT , Latitude );          // Tropic of Cancer 
        createCircle( -AXIALTILT , Latitude );         // Tropic of Capricorn
    }
}


void GridMap::createGrid(const int& radius, Quaternion& planetAxis)
{
    clear();

    m_radius = radius - 1;
    planetAxis.inverse().toMatrix( m_planetAxisMatrix );
#ifdef FLAT_PROJ
    m_planetAxis = planetAxis;
#endif

    //	FIXME:	- Higher precision after optimization 
    //		  ( will keep grid lines from vanishing at high zoom levels ) 

    //	if ( m_radius > 6400 ) { m_precision = 30; createCircles( 64, 48 ); return; } else 
    if ( m_radius > 3200 ) {
        m_precision = 40;
        createCircles( 32, 24 ); 
        return;
    } 
    else if ( m_radius > 1600 ) {
        m_precision = 30;
        createCircles( 16, 12 );
        return;
    }	
    else if ( m_radius >  700 ) {
        m_precision = 30;
        createCircles( 8, 6 );
        return;
    }	
    else if ( m_radius >  400 ) {
        m_precision = 20;
        createCircles( 4, 3 );
        return;
    }	
    else if ( m_radius >  100 ) {
        m_precision = 10;
        createCircles( 2, 3 );
        return;
    }	

    createCircles( 2, 1 );	
}


void GridMap::createCircles( const int lngNum, const int latNum )
{

    // latNum: number of latitude circles between lat = 0 deg and lat < 90 deg
    // lngNum: number of longitude circles between lng = 0 deg and lng < 90 deg

    if ( latNum != 0 ) {

        // Equator:
        createCircle( 0.0 , Latitude );

        // Circles of latitude:
        for ( int i = 1; i < latNum; ++i ) {
            createCircle( + (double)(i) * PIHALF / (double)(latNum), Latitude );
            createCircle( - (double)(i) * PIHALF / (double)(latNum), Latitude );
        } 
    } 

    if ( lngNum == 0 )
        return;

    // Universal prime meridian and its orthogonal great circle:
    createCircle( + 0,      Longitude );
    createCircle( + PIHALF, Longitude );	

    for ( int i = 1; i < lngNum; ++i ) {
        double cutOff = PIHALF / (double)(latNum);
        createCircle( i * PIHALF / lngNum,          Longitude, cutOff );
        createCircle( i * PIHALF / lngNum + PIHALF, Longitude, cutOff );	
    }
}


void GridMap::createCircle( double val, SphereDim dim, double cutOff )
{
#ifndef FLAT_PROJ
    // cutoff: the amount of each quarter circle that is cut off at
    // the pole in radians

    const double cutCoeff   = 1 - cutOff / PIHALF;

    // We draw each circle in quarters ( or parts of those ).
    // This is especially convenient for the great longitude circles which 
    // are being cut off close to the poles.
    // quartSteps: the number of nodes in a "quarter" of a circle.

    const double quartSteps = (double) m_precision;

    double coeff  = 1.0;
    double offset = 0.0;

    for ( int i = 0; i < 4; ++i ) {

        m_polygon.clear();

        if ( i > 1 ) 
            coeff = - 1.0;
        offset = ( i % 2 ) ? 1.0 : 0.0;

        const int steps = (int) ( cutCoeff * quartSteps );

        for ( int j = 0; j < steps + 1; ++j ) {

            double itval  = (j != steps) ? (double)(j) / quartSteps : cutCoeff;
            double dimVal = coeff * ( PIHALF * fabs( offset - itval ) + offset * PIHALF );

            double lat = ( dim == Latitude ) ? val : dimVal;
            double lng = ( dim == Longitude ) ? val : dimVal;

            GeoPoint    geoit( lng, -lat );
            Quaternion  qpos = geoit.quaternion();
            qpos.rotateAroundAxis(m_planetAxisMatrix);

            m_currentPoint = QPointF( (double)(m_imageHalfWidth + m_radius * qpos.v[Q_X]) + 1,
                                      (double)(m_imageHalfHeight + m_radius * qpos.v[Q_Y]) + 1 );
            //qDebug() << "Radius: " << m_radius
            //         << "QPointF(" << (double)(m_imageHalfWidth+ m_radius*qpos.v[Q_X])+1
            //        << ", " << (double)(m_imageHalfHeight+ m_radius*qpos.v[Q_Y])+1 << ")";

            // Take care of horizon crossings if horizon is visible
            m_lastVisible = m_currentlyVisible;
            m_currentlyVisible = (qpos.v[Q_Z] >= 0) ? true : false;

            if ( j == 0 )
                initCrossHorizon();

            if (m_currentlyVisible != m_lastVisible) {
                m_polygon << horizonPoint();

                if (m_polygon.size() >= 2) {
                    append(m_polygon);
                }

                m_polygon.clear();

                if ( m_lastVisible == true )
                    break;
            }

            // Take care of screencrossing crossings if horizon is visible.
            // Filter points which aren't on the visible hemisphere.
            if ( m_currentlyVisible && m_currentPoint != m_lastPoint ) {
                // most recent addition: m_currentPoint != m_lastPoint
                //			qDebug("accepted");
                m_polygon << m_currentPoint;
            }

            m_lastPoint = m_currentPoint;
        }

        if (m_polygon.size() >= 2) {
            append(m_polygon);
        }
    }
#else
    float const centerLat=m_planetAxis.pitch();
    float const centerLng=-m_planetAxis.yaw();
    double xyFactor = (float)(2*m_radius)/M_PI;
    m_polygon.clear();

    if(dim == Latitude)
    {
        QPointF beginPoint(0.0f,m_imageHalfHeight + (centerLat + val)*xyFactor );
        QPointF endPoint( m_imageHalfWidth*2, m_imageHalfHeight + (centerLat + val)*xyFactor );
        m_polygon<<beginPoint<<endPoint;
    }
    else
    {
        float begin = m_imageHalfHeight - m_radius + centerLat*xyFactor;
        float end = begin + 2*m_radius;
        begin = (begin >= 0)? begin : 0;
        end = (end <= m_imageHalfHeight*2)? end : m_imageHalfHeight*2;

        QPointF beginPoint( m_imageHalfWidth + (centerLng + val)*xyFactor,begin);
        QPointF endPoint(m_imageHalfWidth + (centerLng + val)*xyFactor,end);
        m_polygon<<beginPoint<<endPoint;
    }
    append(m_polygon);
#endif
}


void GridMap::paintGridMap(ClipPainter * painter, bool antialiasing)
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

    if (antialiasing == true)
        painter->setRenderHint(QPainter::Antialiasing, false);
}


void GridMap::initCrossHorizon()
{
    m_lastVisible = m_currentlyVisible;

    // Initially m_lastPoint MUST NOT equal m_currentPoint
    m_lastPoint = QPointF( m_currentPoint.x() + 1.0, 
                           m_currentPoint.y() + 1.0 );
}


const QPointF GridMap::horizonPoint()
{
    // qDebug("Interpolating");
    double  xa = 0;
    double  ya = 0;

    xa = m_currentPoint.x() - (m_imageHalfWidth +1) ;

    // Move the m_currentPoint along the y-axis to match the horizon.
    double  radicant = (double)(m_radius +1) * (double)( m_radius +1) - xa*xa;
    if ( radicant > 0 )
        ya = sqrt( radicant );

    if ( ( m_currentPoint.y() - ( m_imageHalfHeight + 1 ) ) < 0 )
        ya = -ya; 

    return QPointF( (double)m_imageHalfWidth  + xa + 1,
                    (double)m_imageHalfHeight + ya + 1 );
}



void GridMap::resizeMap(const QPaintDevice * imageCanvas)
{
    m_imageHalfWidth  = imageCanvas -> width() / 2;
    m_imageHalfHeight = imageCanvas -> height() / 2;
    m_imageRadius     = ( m_imageHalfWidth * m_imageHalfWidth
                          + m_imageHalfHeight * m_imageHalfHeight );
}
