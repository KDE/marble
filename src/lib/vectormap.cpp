//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "vectormap.h"

#include <QtGui/QColor>
#include <QtCore/QVector>
#include <QtCore/QTime>
#include <QtCore/QDebug>

#include <cmath>
#include <stdlib.h>

#include "clippainter.h"
#include "GeoPolygon.h"


// #define VECMAP_DEBUG 


VectorMap::VectorMap()
{

    m_zlimit = 0.0; 
    m_plimit = 0.0;
    m_zBoundingBoxLimit = 0.0; 
    m_zPointLimit = 0.0;

    m_imgrx = 0; 
    m_imgry = 0; 
    m_imgradius = 0;
    m_imgwidth  = 0; 
    m_imgheight = 0;

    // Initialising booleans for horizoncrossing
    m_horizonpair = false;
    m_lastvisible = false;
    m_currentlyvisible = false;
    m_firsthorizon = false;

    m_radius = 0;
    m_rlimit = 0;

    m_brush = QBrush( QColor( 0, 0, 0 ) );

    //	m_debugNodeCount = 0;
}

VectorMap::~VectorMap()
{
}


void VectorMap::createFromPntMap(const PntMap* pntmap, const int& radius, 
                                 Quaternion& rotAxis)
{
    clear();

    m_radius = radius - 1;

    // zlimit: describes the lowest z value of the sphere that is
    //         visible as an excerpt on the screen
    double zlimit = ( ( m_imgradius < m_radius * m_radius )
                     ? sqrt(1 - (double)m_imgradius / (double)(m_radius * m_radius))
                     : 0.0 );
//    zlimit = 0.0;
    // qDebug() << "zlimit: " << zlimit;

    m_zBoundingBoxLimit = ( ( m_zBoundingBoxLimit >= 0.0
                              && zlimit < m_zBoundingBoxLimit )
                            || m_zBoundingBoxLimit < 0.0 )
                     ? zlimit : m_zBoundingBoxLimit;
    m_zPointLimit = ( ( m_zPointLimit >= 0.0 && zlimit < m_zPointLimit )
                      || m_zPointLimit < 0.0 )
                     ? zlimit : m_zPointLimit;
//    m_zPointLimit = 0.0;

    m_rlimit = (int)( (double)(m_radius * m_radius)
                      * (1.0 - m_zPointLimit * m_zPointLimit ) );

    Quaternion  qbound;

    rotAxis.inverse().toMatrix( m_rotMatrix );
#ifdef FLAT_PROJ
    m_planetAxis = rotAxis;
#endif
    GeoPolygon::PtrVector::Iterator       itPolyLine;
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = pntmap->constEnd();

    //	const int detail = 0;
    const int  detail = getDetailLevel();
    GeoPoint   corner;
#ifndef FLAT_PROJ
    for ( itPolyLine = const_cast<PntMap *>(pntmap)->begin();
          itPolyLine < itEndPolyLine;
          ++itPolyLine )
    {
        // This sorts out polygons by bounding box which aren't visible at all.
        m_boundary = (*itPolyLine)->getBoundary();

        for ( int i = 0; i < 5; ++i ) {
            qbound = m_boundary[i].quaternion();

            qbound.rotateAroundAxis(m_rotMatrix); 
            if ( qbound.v[Q_Z] > m_zBoundingBoxLimit ) {
                // if (qbound.v[Q_Z] > 0){
                m_polygon.clear();
                m_polygon.reserve( (*itPolyLine)->size() );
                m_polygon.setClosed( (*itPolyLine)->getClosed() );

                // qDebug() << i << " Visible: YES";
                createPolyLine( (*itPolyLine)->constBegin(),
                                (*itPolyLine)->constEnd(), detail );

                break; // abort foreach test of current boundary
            } 
            // else
            //     qDebug() << i << " Visible: NOT";
            // ++i;
        }
    }
#else
    float const centerLat=m_planetAxis.pitch();
    float const centerLng=-m_planetAxis.yaw();
    double xyFactor = (float)(2*radius)/M_PI;
    double degX;
    double degY;
    double x;
    double y;
    QRectF visibleArea ( 0, 0, m_imgwidth, m_imgheight );
    int offset=0;
    for ( itPolyLine = const_cast<PntMap *>(pntmap)->begin();
          itPolyLine < itEndPolyLine;
          ++itPolyLine )
    {
        offset = 0;
        // This sorts out polygons by bounding box which aren't visible at all.
        m_boundary = (*itPolyLine)->getBoundary();
        m_polygon.clear();
        for ( int i = 0; i < 4; ++i )
        {
            qbound = m_boundary[i].quaternion();
            qbound.getSpherical(degX,degY);
            x = m_imgwidth/2 + xyFactor * (degX + centerLng);
            y = m_imgheight/2 + xyFactor * (degY + centerLat);
            m_polygon<<QPointF( x, y );
        }
        qbound = m_boundary[0].quaternion();
        qbound.getSpherical(degX,degY);
        m_correctSign = degX / fabs(degX);
        while( visibleArea.intersects( (const QRectF&) m_polygon.boundingRect() ) ) {
            offset-=4*radius;
            m_polygon.translate(-4*radius,0);
        }
        offset+=4*radius;
        m_polygon.translate(4*radius,0);
        while( visibleArea.intersects( (const QRectF&) m_polygon.boundingRect() )) {
                m_polygon.clear();
                m_polygon.reserve( (*itPolyLine)->size() );
                m_polygon.setClosed( (*itPolyLine)->getClosed() );

                createPolyLine( (*itPolyLine)->constBegin(),
                                (*itPolyLine)->constEnd(), detail, offset );
                offset+=4*radius;
                m_polygon.translate(4*radius,0);
        }
    }
#endif

}

#ifndef FLAT_PROJ
void VectorMap::createPolyLine( GeoPoint::Vector::ConstIterator  itStartPoint, 
                                GeoPoint::Vector::ConstIterator  itEndPoint,
                                const int detail)
#else
void VectorMap::createPolyLine( GeoPoint::Vector::ConstIterator  itStartPoint, 
                                GeoPoint::Vector::ConstIterator  itEndPoint,
                                const int detail, const int offset )
#endif
{
    GeoPoint::Vector::const_iterator  itPoint;

    // Quaternion qpos = ( FastMath::haveSSE() == true ) ? QuaternionSSE() : Quaternion();
    Quaternion qpos;
    //	int step = 1;
    //	int remain = size();
#ifdef FLAT_PROJ
    float const centerLat=m_planetAxis.pitch();
    float const centerLng=-m_planetAxis.yaw();
    double PiRestriction = 5 * M_PI / 6;
#endif
    for ( itPoint = itStartPoint; itPoint != itEndPoint; ++itPoint ) {
        // remain -= step;
        if ( itPoint->detail() >= detail ) {

            // Calculate polygon nodes
#ifdef VECMAP_DEBUG
            ++m_debugNodeCount;
#endif
#ifndef FLAT_PROJ
            qpos = itPoint->quaternion();
            qpos.rotateAroundAxis(m_rotMatrix);
            m_currentPoint = QPointF( m_imgrx + m_radius * qpos.v[Q_X] + 1,
                                      m_imgry + m_radius * qpos.v[Q_Y] + 1 );
			
            // Take care of horizon crossings if horizon is visible
            m_lastvisible = m_currentlyvisible;			

            // Less accurate:
            // currentlyvisible = (qpos.v[Q_Z] >= m_zPointLimit) ? true : false;
            m_currentlyvisible = ( qpos.v[Q_Z] >= 0 ) ? true : false;
            if ( itPoint == itStartPoint ) {
                initCrossHorizon();
            }
            if ( m_currentlyvisible != m_lastvisible )
                manageCrossHorizon();

            // Take care of screencrossing crossings if horizon is visible.
            // Filter Points which aren't on the visible Hemisphere.
            if ( m_currentlyvisible && m_currentPoint != m_lastPoint ) {
                // most recent addition: currentPoint != lastPoint
                m_polygon << m_currentPoint;
            }
    #if 0
            else {
                // Speed burst on invisible hemisphere
                step = 1;
                if ( z < -0.2) step = 10;
                if ( z < -0.4) step = 30;
                if ( step > remain ) step = 1; 
            }
    #endif

            m_lastPoint = m_currentPoint;
        }
    }

    // In case of horizon crossings, make sure that we always get a
    // polygon closed correctly.
    if ( m_firsthorizon ) {
        m_horizonb = m_firstHorizonPoint;
        if (m_polygon.closed())
            createArc();

        m_firsthorizon = false;
    }
#else
            double xyFactor = (float)(2*m_radius)/M_PI;
            double degX;
            double degY;
            qpos = itPoint->quaternion();
            qpos.getSpherical(degX,degY);
            double x = m_imgwidth/2 + xyFactor * (degX + centerLng) + offset;
            double y = m_imgheight/2 + xyFactor * (degY + centerLat);
            m_currentPoint = QPointF( x, y );
            int sign = (int) (degX / fabs(degX) );
            if( sign != m_correctSign && fabs(degX) > PiRestriction ) {
                if(sign == -1){
                    correct+=4*( m_radius + 1 );
                }
                if(sign == 1){
                    correct-=4*( m_radius + 1 );
                }
            }
            if( x > m_imgwidth ) x = m_imgwidth;
            if( x < 0 ) x = 0;

            if( correct == 0 )
                m_polygon<<m_currentPoint;
            else {
                m_currentPoint.setX( m_currentPoint.rx() + correct );
                m_polygon<<m_currentPoint;
            }
        }
    }
#endif	
    // Avoid polygons degenerated to Points and Lines.

    if ( m_polygon.size() >= 2 ) {
        append(m_polygon);
    }
}


void VectorMap::drawMap(QPaintDevice * origimg, bool antialiasing)
{

    bool clip = (m_radius > m_imgrx || m_radius > m_imgry) ? true : false;

    ClipPainter  painter(origimg, clip);
    //	QPainter painter(origimg);
    if ( antialiasing == true )
        painter.setRenderHint( QPainter::Antialiasing );

    painter.setPen(m_pen);
    painter.setBrush(m_brush);

    ScreenPolygon::Vector::const_iterator  itEndPolygon = end();

    for ( ScreenPolygon::Vector::const_iterator itPolygon = begin();
          itPolygon != itEndPolygon; 
          ++itPolygon )
    {

        if ( itPolygon->closed() == true )  
            painter.drawPolygon( *itPolygon );
        else
            painter.drawPolyline( *itPolygon );
    }

    // painter.drawEllipse(imgrx-m_radius,imgry-m_radius,2*m_radius,2*m_radius+1);
}


void VectorMap::paintMap(ClipPainter * painter, bool antialiasing)
{
    // bool clip = (m_radius > imgrx || m_radius > imgry) ? true : false;

    // ClipPainter painter(origimg, clip);
    // QPainter painter(origimg);
    if ( antialiasing )
        painter->setRenderHint( QPainter::Antialiasing );

    painter->setPen( m_pen );
    painter->setBrush( m_brush );

    ScreenPolygon::Vector::const_iterator  itEndPolygon = end();

    for ( ScreenPolygon::Vector::const_iterator itPolygon = begin();
          itPolygon != itEndPolygon;
          ++itPolygon )
    {
        if ( itPolygon->closed() == true )  
            painter->drawPolygon( *itPolygon );
        else
            painter->drawPolyline( *itPolygon );
    }

    // painter.drawEllipse(imgrx-m_radius,imgry-m_radius,2*m_radius,2*m_radius+1);
}


void VectorMap::initCrossHorizon()
{
    // qDebug("Initializing scheduled new PolyLine");
    m_lastvisible  = m_currentlyvisible;
    m_lastPoint    = QPointF( m_currentPoint.x() + 1, m_currentPoint.y() + 1 );
    m_horizonpair  = false;
    m_firsthorizon = false;
}


void VectorMap::manageCrossHorizon()
{
    // qDebug("Crossing horizon line");
    // if (currentlyvisible == false) qDebug("Leaving visible hemisphere");
    // else qDebug("Entering visible hemisphere");

    if ( m_horizonpair == false ) {
        // qDebug("Point A");

        if ( m_currentlyvisible == false ) {
            m_horizona    = horizonPoint();
            m_horizonpair = true;
        }
        else {
            // qDebug("Orphaned");
            m_firstHorizonPoint = horizonPoint();
            m_firsthorizon      = true;
        }
    }
    else {
        // qDebug("Point B");
        m_horizonb = horizonPoint();

        createArc();
        m_horizonpair = false;
    }
}


const QPointF VectorMap::horizonPoint()
{
    // qDebug("Interpolating");
    double  xa;
    double  ya;

    xa = m_currentPoint.x() - ( m_imgrx + 1 );

    // Move the currentPoint along the y-axis to match the horizon.
    //	ya = sqrt( (m_radius +1) * ( m_radius +1) - xa*xa);
    ya = ( m_rlimit > xa * xa )
        ? sqrt( (double)(m_rlimit) - (double)( xa * xa ) ) : 0;
    // qDebug() << " m_rlimit" << m_rlimit << " xa*xa" << xa*xa << " ya: " << ya;
    if ( ( m_currentPoint.y() - ( m_imgry + 1 ) ) < 0 )
        ya = -ya; 

    return QPointF( m_imgrx + xa + 1, m_imgry + ya + 1 );
}


void VectorMap::createArc()
{

    double  beta  = (double)( 180.0 / M_PI 
			      * atan2( m_horizonb.y() - m_imgry - 1,
				       m_horizonb.x() - m_imgrx - 1 ) );
    double  alpha = (double)( 180.0 / M_PI
			      * atan2( m_horizona.y() - m_imgry - 1,
				       m_horizona.x() - m_imgrx - 1 ) );

    double diff = beta - alpha;

    if ( diff != 0.0 && diff != 180.0 && diff != -180.0 ) {

        m_polygon.append( m_horizona );

        double sgndiff = diff / fabs(diff);

        if (fabs(diff) > 180.0)
            diff = - sgndiff * (360.0 - fabs(diff));

        // Reassigning sgndiff this way seems dull
        sgndiff = diff / fabs(diff);
        // qDebug() << "SGN: " << sgndiff;

        // qDebug () << " beta: " << beta << " alpha " << alpha << " diff: " << diff;
	
        int  itx;
        int  ity;
        // qDebug() << "r: " << (m_radius+1) << "rn: " << sqrt((double)(m_rlimit));
        double  arcradius = sqrt( (double)( m_rlimit ) );

        for ( int it = 1; it < fabs(diff); ++it ) {
            double angle = M_PI/180.0 * (double)( alpha + (sgndiff * it) );
            itx = (int)( m_imgrx +  arcradius * cos( angle ) + 1 );
            ity = (int)( m_imgry +  arcradius * sin( angle ) + 1 );
            // qDebug() << " ity: " << ity;
            m_polygon.append( QPoint( itx, ity ) );		
        }

        m_polygon.append( m_horizonb );
    }
}


void VectorMap::resizeMap(const QPaintDevice * origimg)
{
    m_imgwidth  = origimg -> width();
    m_imgheight = origimg -> height();
    // qDebug() << "width:" << imgwidth;
    m_imgrx = ( m_imgwidth  >> 1);
    m_imgry = ( m_imgheight >> 1);
    m_imgradius = m_imgrx * m_imgrx + m_imgry * m_imgry;
}


int VectorMap::getDetailLevel() const
{
    int detail = 5;
	
    if ( m_radius >   50 ) detail = 4;
    if ( m_radius >  600 ) detail = 3;
    if ( m_radius > 1000 ) detail = 2;
    if ( m_radius > 2500 ) detail = 1;
    if ( m_radius > 5000 ) detail = 0;

    //	qDebug() << "Detail: " << detail << " Radius: " << m_radius ;

    return detail;
}

