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
#include "ClipPainter.h"


// Except for the equator the major circles of latitude are defined via 
// the earth's axial tilt, which currently measures about 23°26'21".
 
const double  AXIALTILT = DEG2RAD * ( 23.0
                                           + 26.0 / 60.0
                                           + 21.0 / 3600.0 );
const double  PIHALF    = M_PI / 2;


GridMap::GridMap()
{
    m_imageWidth  = 0;
    m_imageHeight = 0;
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


void GridMap::createTropics(const int& radius, Quaternion& planetAxis, Projection currentProjection)
{
    clear();
    m_radius = radius;
    planetAxis.inverse().toMatrix( m_planetAxisMatrix );

    // Turn on the major circles of latitude if we've zoomed in far
    // enough (radius > 400 pixels)
    if ( m_radius >  400 ) {
        createCircle( PIHALF - AXIALTILT , Latitude, currentProjection ); // Arctic Circle
        createCircle( AXIALTILT - PIHALF , Latitude, currentProjection ); // Antarctic Circle
        createCircle( AXIALTILT , Latitude, currentProjection );          // Tropic of Cancer 
        createCircle( -AXIALTILT , Latitude, currentProjection );         // Tropic of Capricorn
    }
}

void GridMap::createEquator(const int& radius, Quaternion& planetAxis, Projection currentProjection) 
{
    clear();
    m_radius = radius;
    planetAxis.inverse().toMatrix( m_planetAxisMatrix );

    if( currentProjection == Equirectangular )
        m_planetAxis = planetAxis;

    createCircle( 0.0 , Latitude, currentProjection );
}

void GridMap::createGrid(const int& radius, Quaternion& planetAxis, Projection currentProjection)
{
    clear();

    m_radius = radius;
    planetAxis.inverse().toMatrix( m_planetAxisMatrix );
    if( currentProjection == Equirectangular )
        m_planetAxis = planetAxis;

    //	FIXME:	- Higher precision after optimization 
    //		  ( will keep grid lines from vanishing at high zoom levels ) 

    //	if ( m_radius > 6400 ) { m_precision = 30; createCircles( 64, 48 ); return; } else 
    if ( m_radius > 3200 ) {
        m_precision = 40;
        createCircles( 32, 24, currentProjection ); 
        return;
    } 
    else if ( m_radius > 1600 ) {
        m_precision = 30;
        createCircles( 16, 12, currentProjection );
        return;
    }	
    else if ( m_radius >  700 ) {
        m_precision = 30;
        createCircles( 8, 6, currentProjection );
        return;
    }	
    else if ( m_radius >  400 ) {
        m_precision = 20;
        createCircles( 4, 3, currentProjection );
        return;
    }	
    else if ( m_radius >  100 ) {
        m_precision = 10;
        createCircles( 2, 3, currentProjection );
        return;
    }	

    createCircles( 2, 1, currentProjection );	
}


void GridMap::createCircles( const int lonNum, const int latNum, Projection currentProjection )
{
    switch( currentProjection ) {
        case Spherical:
            sphericalCreateCircles( lonNum, latNum );
            break;
        case Equirectangular:
            rectangularCreateCircles( lonNum, latNum );
            break;
    }
}

void GridMap::sphericalCreateCircles( const int lonNum, const int latNum )
{
    // latNum: number of latitude circles between lat = 0 deg and lat < 90 deg
    // lonNum: number of longitude circles between lon = 0 deg and lon < 90 deg

    if ( latNum != 0 ) {

        // Circles of latitude:
        for ( int i = 1; i < latNum; ++i ) {
            createCircle( + (double)(i) * PIHALF / (double)(latNum), Latitude, Spherical );
            createCircle( - (double)(i) * PIHALF / (double)(latNum), Latitude, Spherical );
        } 
    } 

    if ( lonNum == 0 )
        return;

    // Universal prime meridian and its orthogonal great circle:
    createCircle( + 0,      Longitude, Spherical );
    createCircle( + PIHALF, Longitude, Spherical );	

    for ( int i = 1; i < lonNum; ++i ) {
        double cutOff = PIHALF / (double)(latNum);
        createCircle( i * PIHALF / lonNum,          Longitude, Spherical, cutOff );
        createCircle( i * PIHALF / lonNum + PIHALF, Longitude, Spherical, cutOff );	
    }
}

void GridMap::rectangularCreateCircles( const int lonNum, const int latNum )
{

    // latNum: number of latitude circles between lat = 0 deg and lat < 90 deg
    // lonNum: number of longitude circles between lon = 0 deg and lon < 90 deg

    if ( latNum != 0 ) {

        // Circles of latitude:
        for ( int i = 1; i < latNum; ++i ) {
            createCircle( + (double)(i) * PIHALF / (double)(latNum), Latitude, Equirectangular );
            createCircle( - (double)(i) * PIHALF / (double)(latNum), Latitude, Equirectangular );
        } 
    } 

    if ( lonNum == 0 )
        return;

    // Universal prime meridian
    createCircle( + 0,      Longitude, Equirectangular );

    for ( int i = 0; i <= lonNum; ++i ) {
        double cutOff = PIHALF / (double)(latNum);
        createCircle( i * M_PI / lonNum,          Longitude, Equirectangular, cutOff );
        createCircle( i * M_PI / lonNum + M_PI, Longitude, Equirectangular, cutOff );	
    }
}

void GridMap::createCircle( double val, SphereDim dim, Projection currentProjection, double cutOff)
{
    switch( currentProjection ) {
        case Spherical:
            sphericalCreateCircle( val, dim, cutOff );
            break;
        case Equirectangular:
            rectangularCreateCircle( val, dim, cutOff );
            break;
    }
}

void GridMap::sphericalCreateCircle( double val, SphereDim dim, double cutOff )
{
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

            double lat = ( dim == Latitude )  ? val : dimVal;
            double lon = ( dim == Longitude ) ? val : dimVal;

            GeoPoint    geoit( lon, -lat );
            Quaternion  qpos = geoit.quaternion();
            qpos.rotateAroundAxis(m_planetAxisMatrix);

            m_currentPoint = QPointF( (double)(m_imageWidth / 2 + m_radius * qpos.v[Q_X]) + 1,
                                      (double)(m_imageHeight / 2 + m_radius * qpos.v[Q_Y]) + 1 );
            //qDebug() << "Radius: " << m_radius
            //         << "QPointF(" << (double)(m_imageWidth / 2 + m_radius*qpos.v[Q_X])+1
            //        << ", " << (double)(m_imageHeight / 2 + m_radius*qpos.v[Q_Y])+1 << ")";

            // Take care of horizon crossings if horizon is visible.
            m_lastVisible = m_currentlyVisible;
            m_currentlyVisible = (qpos.v[Q_Z] >= 0) ? true : false;

            // Initialize crossing of the horizon.
            if ( j == 0 ) {

                m_lastVisible = m_currentlyVisible;

                // Initially m_lastPoint MUST NOT equal m_currentPoint
                m_lastPoint = QPointF( m_currentPoint.x() + 1.0, 
                                       m_currentPoint.y() + 1.0 );
            }

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
}

void GridMap::rectangularCreateCircle( double val, SphereDim dim, double cutOff )
{
    float const  centerLat =  m_planetAxis.pitch();
    float const  centerLon = -m_planetAxis.yaw();
    double       xyFactor  = (float)( 2 * m_radius ) / M_PI;
    m_polygon.clear();

    if ( dim == Latitude ) {
        QPointF beginPoint( 0.0f, m_imageHeight / 2 + ( centerLat + val ) * xyFactor );
        QPointF endPoint( m_imageWidth, m_imageHeight / 2 + ( centerLat + val ) * xyFactor );
        m_polygon << beginPoint << endPoint;
        append( m_polygon );
    }
    else {
        float beginY = m_imageHeight / 2 - m_radius + centerLat * xyFactor;
        float endY   = beginY + 2 * m_radius;
        if ( beginY < 0 ) 
            beginY = 0;
        if ( endY > m_imageHeight )
            endY = m_imageHeight ;

        float x = m_imageWidth / 2 + ( centerLon + val ) * xyFactor;
        while ( x > 4 * m_radius ) 
            x -= 4 * m_radius;
        while ( x < m_imageWidth ) {
            QPointF beginPoint( x , beginY );
            QPointF endPoint( x , endY );
            m_polygon<<beginPoint<<endPoint;
            x+=4*m_radius;
            append(m_polygon);
            m_polygon.clear();
        }
    }
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

    if (antialiasing)
        painter->setRenderHint( QPainter::Antialiasing, false );
}


const QPointF GridMap::horizonPoint()
{
    // qDebug("Interpolating");
    double  xa = 0;
    double  ya = 0;

    xa = m_currentPoint.x() - ( m_imageWidth / 2 + 1 ) ;

    // Move the m_currentPoint along the y-axis to match the horizon.
    double  radicant = (double)(m_radius +1) * (double)( m_radius +1) - xa*xa;
    if ( radicant > 0 )
        ya = sqrt( radicant );

    if ( ( m_currentPoint.y() - ( m_imageHeight / 2 + 1 ) ) < 0 )
        ya = -ya; 

    return QPointF( (double)m_imageWidth / 2  + xa + 1,
                    (double)m_imageHeight / 2 + ya + 1 );
}



void GridMap::resizeMap( int width, int height )
{
    m_imageWidth  = width;
    m_imageHeight = height;
    m_imageRadius     = ( m_imageWidth * m_imageWidth / 4
                          + m_imageHeight * m_imageHeight / 4 );
}
