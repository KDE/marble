//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>"
//


#include "VectorMap.h"

#include <cmath>
#include <stdlib.h>

#include <QtCore/QVector>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtGui/QColor>

#include "global.h"
#include "AbstractProjection.h"
#include "GeoPainter.h"
#include "GeoPolygon.h"
#include "ViewportParams.h"
#include "AbstractProjectionHelper.h"

#ifdef Q_CC_MSVC
static double msvc_atanh(double x)
{
  return ( 0.5 * log( ( 1.0 + x ) / ( 1.0 - x ) ) );
}
#define atanh msvc_atanh
#endif

// #define VECMAP_DEBUG 


VectorMap::VectorMap()
{

    m_zlimit = 0.0; 
    m_plimit = 0.0;
    m_zBoundingBoxLimit = 0.0; 
    m_zPointLimit = 0.0;

    m_imgrx = 0; 
    m_imgry = 0; 
    m_imgwidth  = 0; 
    m_imgheight = 0;

    // Initialising booleans for horizoncrossing
    m_horizonpair = false;
    m_lastvisible = false;
    m_currentlyvisible = false;
    m_firsthorizon = false;

    m_rlimit = 0;

    m_brush = QBrush( QColor( 0, 0, 0 ) );

    //	m_debugNodeCount = 0;
}

VectorMap::~VectorMap()
{
}


void VectorMap::createFromPntMap( const PntMap* pntmap, 
				  ViewportParams* viewport )
{
    switch( viewport->projection() ) {
        case Spherical:
            sphericalCreateFromPntMap( pntmap, viewport );
            break;
        case Equirectangular:
            rectangularCreateFromPntMap( pntmap, viewport );
            break;
        case Mercator:
            mercatorCreateFromPntMap( pntmap, viewport );
            break;
    }
}

void VectorMap::sphericalCreateFromPntMap( const PntMap* pntmap, 
					   ViewportParams* viewport )
{
    clear();

    // We must use double or int64 for the calculations because we
    // square radius sometimes below, and it may cause an overflow. We
    // choose double because of some sqrt() calculations.
    double   radius     = viewport->radius();
    double   imgradius2 = m_imgrx * m_imgrx + m_imgry * m_imgry;

    // zlimit: describes the lowest z value of the sphere that is
    //         visible on the screen.  This should happen in the
    //         corners.
    double zlimit = ( ( imgradius2 < radius * radius )
                      ? sqrt( 1 - imgradius2 / ( radius * radius ) )
                      : 0.0 );
    // qDebug() << "zlimit: " << zlimit;

    m_zBoundingBoxLimit = ( ( m_zBoundingBoxLimit >= 0.0
                              && zlimit < m_zBoundingBoxLimit )
                            || m_zBoundingBoxLimit < 0.0 )
                     ? zlimit : m_zBoundingBoxLimit;
    m_zPointLimit = ( ( m_zPointLimit >= 0.0 && zlimit < m_zPointLimit )
                      || m_zPointLimit < 0.0 )
                     ? zlimit : m_zPointLimit;

    m_rlimit = (int)( ( radius * radius )
                      * (1.0 - m_zPointLimit * m_zPointLimit ) );

    Quaternion  qbound;

    viewport->planetAxis().inverse().toMatrix( m_rotMatrix );
    GeoPolygon::PtrVector::Iterator       itPolyLine;
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = pntmap->constEnd();

    //	const int detail = 0;
    const int  detail = getDetailLevel( viewport->radius() );

    for ( itPolyLine = const_cast<PntMap *>(pntmap)->begin();
          itPolyLine < itEndPolyLine;
          ++itPolyLine )
    {
        // This sorts out polygons by bounding box which aren't visible at all.
        GeoDataCoordinates::PtrVector boundary = (*itPolyLine)->getBoundary();

        for ( int i = 0; i < 5; ++i ) {
            qbound = boundary[i]->quaternion();

            qbound.rotateAroundAxis(m_rotMatrix); 
            if ( qbound.v[Q_Z] > m_zBoundingBoxLimit ) {
                // if (qbound.v[Q_Z] > 0){
                m_polygon.clear();
                m_polygon.reserve( (*itPolyLine)->size() );
                m_polygon.setClosed( (*itPolyLine)->getClosed() );

                // qDebug() << i << " Visible: YES";
                createPolyLine( (*itPolyLine)->constBegin(),
                                (*itPolyLine)->constEnd(), detail, viewport );

                break; // abort foreach test of current boundary
            } 
            // else
            //     qDebug() << i << " Visible: NOT";
        }
    }
}

void VectorMap::rectangularCreateFromPntMap( const PntMap* pntmap, 
					     ViewportParams* viewport )
{
    clear();
    int  radius = viewport->radius();

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    double rad2Pixel = (float)( 2 * radius ) / M_PI;
    double lon, lat;
    double x, y;

    viewport->planetAxis().inverse().toMatrix( m_rotMatrix );
    GeoPolygon::PtrVector::Iterator       itPolyLine;
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = pntmap->constEnd();

    ScreenPolygon  boundingPolygon;
    QRectF         visibleArea ( 0, 0, m_imgwidth, m_imgheight );
    const int      detail = getDetailLevel( radius );

    for ( itPolyLine = const_cast<PntMap *>(pntmap)->begin();
          itPolyLine < itEndPolyLine;
          ++itPolyLine )
    {
        // This sorts out polygons by bounding box which aren't visible at all.
        m_offset = 0;

        GeoDataCoordinates::PtrVector  boundary = (*itPolyLine)->getBoundary();
        boundingPolygon.clear();

        // Let's just use the top left and the bottom right bounding
        // box point for this projection.
        for ( int i = 1; i < 3; ++i ) {
            boundary[i]->geoCoordinates(lon, lat);
            x = (double)(m_imgwidth)  / 2.0 - rad2Pixel * (centerLon - lon);
            y = (double)(m_imgheight) / 2.0 + rad2Pixel * (centerLat - lat);
            boundingPolygon << QPointF( x, y );
        }

        if ( boundingPolygon.at(0).x() < 0 || boundingPolygon.at(1).x() < 0 ) {
            boundingPolygon.translate( 4 * radius, 0 );
            m_offset += 4 * radius;
        }

        do {
            m_offset -= 4 * radius;
            boundingPolygon.translate( -4 * radius, 0 );
	    // FIXME: Get rid of this really fugly code once we have a
	    //        proper LatLonBox check implemented and in place.
        } while( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		   && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		 || ( (*itPolyLine)->getDateLine() == GeoPolygon::Even
		      && ( visibleArea.intersects( QRectF( boundingPolygon.at(1), QPointF( (double)(m_imgwidth)  / 2.0 - rad2Pixel * ( centerLon - M_PI ) + m_offset, boundingPolygon.at(0).y() ) ) ) 
                || visibleArea.intersects( QRectF( QPointF( (double)(m_imgwidth)  / 2.0 - rad2Pixel * ( centerLon + M_PI )  + m_offset, boundingPolygon.at(1).y() ), boundingPolygon.at(0) ) ) ) ) 
                );
        m_offset += 4 * radius;
        boundingPolygon.translate( 4 * radius, 0 );

	// FIXME: Get rid of this really fugly code once we will have
	//        a proper LatLonBox check implemented and in place.
        while ( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		  && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		|| ( (*itPolyLine)->getDateLine() == GeoPolygon::Even 
		     && ( visibleArea.intersects(
			    QRectF( boundingPolygon.at(1),
				    QPointF( (double)(m_imgwidth) / 2.0
					     - rad2Pixel * ( centerLon - M_PI )
					     + m_offset, 
					     boundingPolygon.at(0).y() ) ) ) 
			  || visibleArea.intersects(
			         QRectF( QPointF( (double)(m_imgwidth) / 2.0
						  - rad2Pixel * ( centerLon + M_PI )
						  + m_offset,
						  boundingPolygon.at(1).y() ),
					 boundingPolygon.at(0) ) ) ) )
		) 
	{

            m_polygon.clear();
            m_polygon.reserve( (*itPolyLine)->size() );
            m_polygon.setClosed( (*itPolyLine)->getClosed() );

            createPolyLine( (*itPolyLine)->constBegin(),
                            (*itPolyLine)->constEnd(), detail, viewport );

            m_offset += 4 * radius;
            boundingPolygon.translate( 4 * radius, 0 );
        }
    }
}

void VectorMap::mercatorCreateFromPntMap( const PntMap* pntmap,
                                          ViewportParams* viewport )
{
    clear();
    int  radius = viewport->radius();

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    double rad2Pixel = (float)( 2 * radius ) / M_PI;
    double lon, lat;
    double x, y;

    viewport->planetAxis().inverse().toMatrix( m_rotMatrix );
    GeoPolygon::PtrVector::Iterator       itPolyLine;
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = pntmap->constEnd();

    ScreenPolygon  boundingPolygon;
    QRectF         visibleArea ( 0, 0, m_imgwidth, m_imgheight );
    const int      detail = getDetailLevel( radius );

    for ( itPolyLine = const_cast<PntMap *>(pntmap)->begin();
          itPolyLine < itEndPolyLine;
          ++itPolyLine )
    {
        // This sorts out polygons by bounding box which aren't visible at all.
        m_offset = 0;

        GeoDataCoordinates::PtrVector boundary = (*itPolyLine)->getBoundary();
        boundingPolygon.clear();

        // Let's just use the top left and the bottom right bounding box point for 
        // this projection
        for ( int i = 1; i < 3; ++i ) {
            boundary[i]->geoCoordinates(lon, lat);
            x = (double)(m_imgwidth)  / 2.0 + rad2Pixel * (lon - centerLon);
            y = (double)(m_imgheight) / 2.0 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) );

            boundingPolygon << QPointF( x, y );
        }

        if ( boundingPolygon.at(0).x() < 0 || boundingPolygon.at(1).x() < 0 ) {
            boundingPolygon.translate( 4 * radius, 0 );
            m_offset += 4 * radius;
        }

        do {
            m_offset -= 4 * radius;
            boundingPolygon.translate( -4 * radius, 0 );
	    // FIXME: Get rid of this really fugly code once we have a
	    //        proper LatLonBox check implemented and in place.
        } while( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		   && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		 || ( (*itPolyLine)->getDateLine() == GeoPolygon::Even
		      && ( visibleArea.intersects( QRectF( boundingPolygon.at(1), QPointF( (double)(m_imgwidth)  / 2.0 - rad2Pixel * ( centerLon - M_PI ) + m_offset, boundingPolygon.at(0).y() ) ) ) 
                || visibleArea.intersects( QRectF( QPointF( (double)(m_imgwidth)  / 2.0 - rad2Pixel * ( centerLon + M_PI )  + m_offset, boundingPolygon.at(1).y() ), boundingPolygon.at(0) ) ) ) ) 
                );
        m_offset += 4 * radius;
        boundingPolygon.translate( 4 * radius, 0 );

	// FIXME: Get rid of this really fugly code once we will have
	//        a proper LatLonBox check implemented and in place.
        while ( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		  && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		|| ( (*itPolyLine)->getDateLine() == GeoPolygon::Even 
		     && ( visibleArea.intersects(
			    QRectF( boundingPolygon.at(1),
				    QPointF( (double)(m_imgwidth) / 2.0
					     - rad2Pixel * ( centerLon - M_PI )
					     + m_offset, 
					     boundingPolygon.at(0).y() ) ) ) 
			  || visibleArea.intersects(
			         QRectF( QPointF( (double)(m_imgwidth) / 2.0
						  - rad2Pixel * ( centerLon + M_PI )
						  + m_offset,
						  boundingPolygon.at(1).y() ),
					 boundingPolygon.at(0) ) ) ) )
		)
	{
            m_polygon.clear();
            m_polygon.reserve( (*itPolyLine)->size() );
            m_polygon.setClosed( (*itPolyLine)->getClosed() );

            createPolyLine( (*itPolyLine)->constBegin(),
                            (*itPolyLine)->constEnd(), detail, viewport );

            m_offset += 4 * radius;
            boundingPolygon.translate( 4 * radius, 0 );
        }
    }
}

void VectorMap::createPolyLine( GeoDataCoordinates::Vector::ConstIterator  itStartPoint, 
                                GeoDataCoordinates::Vector::ConstIterator  itEndPoint,
                                const int detail, ViewportParams *viewport )
{
    switch( viewport->projection() ) {
       case Spherical:
	   sphericalCreatePolyLine( itStartPoint, itEndPoint,
				    detail, viewport );
            break;
        case Equirectangular:
            rectangularCreatePolyLine( itStartPoint, itEndPoint, 
				       detail, viewport );
            break;
        case Mercator:
            mercatorCreatePolyLine( itStartPoint, itEndPoint, 
				    detail, viewport );
            break;
    }
}

void VectorMap::sphericalCreatePolyLine(
GeoDataCoordinates::Vector::ConstIterator  itStartPoint,
GeoDataCoordinates::Vector::ConstIterator  itEndPoint,
const int detail, ViewportParams *viewport )
{
    GeoDataCoordinates::Vector::const_iterator  itPoint;

    int  radius = viewport->radius();

    // Quaternion qpos = ( FastMath::haveSSE() == true ) ? QuaternionSSE() : Quaternion();
    Quaternion qpos;
    //	int step = 1;
    //	int remain = size();
    for ( itPoint = itStartPoint; itPoint != itEndPoint; ++itPoint ) {
        // remain -= step;
        if ( itPoint->detail() < detail )
            continue;

	// Calculate polygon nodes
#ifdef VECMAP_DEBUG
	++m_debugNodeCount;
#endif
	qpos = itPoint->quaternion();
	qpos.rotateAroundAxis( m_rotMatrix );
	m_currentPoint = QPointF( m_imgrx + radius * qpos.v[Q_X] + 1.0,
				  m_imgry - radius * qpos.v[Q_Y] + 1.0 );
			
	// Take care of horizon crossings if horizon is visible
	m_lastvisible = m_currentlyvisible;			

	// Less accurate:
	// currentlyvisible = (qpos.v[Q_Z] >= m_zPointLimit) ? true : false;
	m_currentlyvisible = ( qpos.v[Q_Z] >= 0 );
	if ( itPoint == itStartPoint ) {
	    // qDebug("Initializing scheduled new PolyLine");
	    m_lastvisible  = m_currentlyvisible;
	    m_lastPoint    = QPointF( m_currentPoint.x() + 1.0, 
				      m_currentPoint.y() + 1.0 );
	    m_horizonpair  = false;
	    m_firsthorizon = false;
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

    // In case of horizon crossings, make sure that we always get a
    // polygon closed correctly.
    if ( m_firsthorizon ) {
        m_horizonb = m_firstHorizonPoint;
        if (m_polygon.closed())
            createArc();

        m_firsthorizon = false;
    }

    // Avoid polygons degenerated to Points.
    if ( m_polygon.size() >= 2 ) {
        append(m_polygon);
    }
}

void VectorMap::rectangularCreatePolyLine(
    GeoDataCoordinates::Vector::ConstIterator  itStartPoint, 
    GeoDataCoordinates::Vector::ConstIterator  itEndPoint,
    const int detail, ViewportParams *viewport )
{
    GeoDataCoordinates::Vector::const_iterator  itPoint;

    Quaternion qpos;

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Other convenience variables
    double  rad2Pixel = (float)( 2 * viewport->radius() ) / M_PI;

    ScreenPolygon otherPolygon;
    otherPolygon.setClosed ( m_polygon.closed() );
    bool CrossedDateline = false;
    bool firstPoint = true;
    double lon, lat;

    for ( itPoint = itStartPoint; itPoint != itEndPoint; ++itPoint ) {
        // remain -= step;
        if ( itPoint->detail() < detail )
	    continue;

	// Calculate polygon nodes
#ifdef VECMAP_DEBUG
	++m_debugNodeCount;
#endif

	itPoint->geoCoordinates( lon, lat);
	double x = (double)(m_imgwidth)  / 2.0 - rad2Pixel * (centerLon - lon) + m_offset;
	double y = (double)(m_imgheight) / 2.0 + rad2Pixel * (centerLat - lat);
	int currentSign = ( lon > 0.0 ) ? 1 : -1 ;
	if ( firstPoint ) {
	    firstPoint = false;
	    m_lastSign = currentSign;
	}

	m_currentPoint = QPointF( x, y );

	// Correction of the Dateline
	if ( m_lastSign != currentSign && fabs(m_lastLon) + fabs(lon) > M_PI ) {

	    // X coordinate on the screen for the points on the
	    // dateline on both sides of the flat map.
	    double lastXAtDateLine = (double)(m_imgwidth) / 2.0 + rad2Pixel * ( m_lastSign * M_PI - centerLon ) + m_offset;
	    double xAtDateLine = (double)(m_imgwidth) / 2.0 + rad2Pixel * ( -m_lastSign * M_PI - centerLon ) + m_offset;
	    double lastYAtDateLine = (double)(m_imgheight) / 2.0 - ( m_lastLat - centerLat ) * rad2Pixel;
	    double yAtSouthPole = (double)(m_imgheight) / 2.0 + rad2Pixel * ( M_PI / 2.0 + centerLat );

	    //If the "jump" occurs in the Anctartica's latitudes

	    if ( lat < - M_PI / 3 ) {
		// FIXME: This should actually need to get investigated
		//        in GeoPainter.  For now though we just help
		//        GeoPainter to get the clipping right.
		if ( lastXAtDateLine > (double)(m_imgwidth) - 1.0 )
		    lastXAtDateLine = (double)(m_imgwidth) - 1.0;
		if ( lastXAtDateLine < 0.0 )
		    lastXAtDateLine = 0.0; 
		if ( xAtDateLine > (double)(m_imgwidth) - 1.0 )
		    xAtDateLine = (double)(m_imgwidth) - 1.0;
		if ( xAtDateLine < 0.0 )
		    xAtDateLine = 0.0; 

		m_polygon << QPointF( lastXAtDateLine, y ); 
		m_polygon << QPointF( lastXAtDateLine, yAtSouthPole );
		m_polygon << QPointF( xAtDateLine,     yAtSouthPole );
		m_polygon << QPointF( xAtDateLine,     y );
	    }
	    else {

		if ( CrossedDateline ) {
		    m_polygon    << QPointF( xAtDateLine,     y );
		    otherPolygon << QPointF( lastXAtDateLine, lastYAtDateLine);
		}
		else {
		    m_polygon << QPointF( lastXAtDateLine, lastYAtDateLine );
		    otherPolygon << QPointF( xAtDateLine,  y );
		}
		CrossedDateline = !CrossedDateline;
	    }
	}

	if ( CrossedDateline )
	    otherPolygon << m_currentPoint;
	else
	    m_polygon << m_currentPoint;

	m_lastLon  = lon;
	m_lastLat  = lat;
	m_lastSign = currentSign;
    }

    // Avoid polygons degenerated to Points.
    if ( m_polygon.size() >= 2 ) {
        append(m_polygon);
    }

    if ( otherPolygon.size() >= 2 ) {
        append( otherPolygon );
    }
}

void VectorMap::mercatorCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator  itStartPoint, 
					GeoDataCoordinates::Vector::ConstIterator  itEndPoint,
					const int detail,
					ViewportParams *viewport )
{
    GeoDataCoordinates::Vector::const_iterator  itPoint;

    Quaternion qpos;

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // Other convenience variables
    double  rad2Pixel = (double)( 2 * viewport->radius() ) / M_PI;

    ScreenPolygon  otherPolygon;
    otherPolygon.setClosed ( m_polygon.closed() );

    bool    CrossedDateline = false;
    bool    firstPoint      = true;
    double  lon;
    double  lat;

    for ( itPoint = itStartPoint; itPoint != itEndPoint; ++itPoint ) {
        // remain -= step;
        if ( itPoint->detail() < detail )
	    continue;

	// Calculate polygon nodes
#ifdef VECMAP_DEBUG
	++m_debugNodeCount;
#endif

	// FIXME: Call the projection.  Unfortunately there is no
	//        screenCoordinates taking doubles.
	itPoint->geoCoordinates( lon, lat);
	double x = (double)(m_imgwidth)  / 2.0 + rad2Pixel * (lon - centerLon) + m_offset;
	double y = (double)(m_imgheight) / 2.0 - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) );
	int currentSign = ( lon > 0.0 ) ? 1 : -1 ;
	if ( firstPoint ) {
	    firstPoint = false;
	    m_lastSign = currentSign;
	}

	m_currentPoint = QPointF( x, y );

	//correction of the Dateline

	if ( m_lastSign != currentSign && fabs(m_lastLon) + fabs(lon) > M_PI ) {

	    // x coordinate on the screen for the points on the dateline on both
	    // sides of the flat map.
	    // FIXME: mercator projection here too.
	    double lastXAtDateLine = (double)(m_imgwidth) / 2.0 + rad2Pixel * ( m_lastSign * M_PI - centerLon ) + m_offset;
	    double xAtDateLine = (double)(m_imgwidth) / 2.0 + rad2Pixel * ( -m_lastSign * M_PI - centerLon ) + m_offset;
	    double lastYAtDateLine = (double)(m_imgheight) / 2.0 - ( m_lastLat - centerLat ) * rad2Pixel;
	    double yAtSouthPole = (double)(m_imgheight) / 2.0 + rad2Pixel * ( M_PI / 2.0 + centerLat );

	    //If the "jump" occurs in the Anctartica's latitudes

	    if ( lat < - M_PI / 3 ) {
		// FIXME: This should actually need to get investigated
		//        in GeoPainter.  For now though we just help
		//        GeoPainter to get the clipping right.
		if ( lastXAtDateLine > (double)(m_imgwidth) - 1.0 )
		    lastXAtDateLine = (double)(m_imgwidth) - 1.0;
		if ( lastXAtDateLine < 0.0 )
		    lastXAtDateLine = 0.0; 
		if ( xAtDateLine > (double)(m_imgwidth) - 1.0 )
		    xAtDateLine = (double)(m_imgwidth) - 1.0;
		if ( xAtDateLine < 0.0 )
		    xAtDateLine = 0.0; 

		m_polygon << QPointF( lastXAtDateLine, y ); 
		m_polygon << QPointF( lastXAtDateLine, yAtSouthPole );
		m_polygon << QPointF( xAtDateLine,     yAtSouthPole );
		m_polygon << QPointF( xAtDateLine,     y );
	    }
	    else {

		if ( CrossedDateline ) {
		    m_polygon    << QPointF( xAtDateLine,     y );
		    otherPolygon << QPointF( lastXAtDateLine, lastYAtDateLine);
		}
		else {
		    m_polygon << QPointF( lastXAtDateLine, lastYAtDateLine );
		    otherPolygon << QPointF( xAtDateLine,  y );
		}
		CrossedDateline = !CrossedDateline;
	    }
	}

	if ( CrossedDateline )
	    otherPolygon << m_currentPoint;
	else
	    m_polygon << m_currentPoint;

	m_lastLon  = lon;
	m_lastLat  = lat;
	m_lastSign = currentSign;
    }

    // Avoid polygons degenerated to Points.
    if ( m_polygon.size() >= 2 ) {
        append(m_polygon);
    }

    if ( otherPolygon.size() >= 2 ) {
        append( otherPolygon );
    }
}


// Paint the background of the ground, i.e. the water.
//
// FIXME: This is a strange thing to have in the vector code. 
//        Move it somewhere better.
// FIXME: Btw, we shouldn't assume that the globe to paint is the earth.
//
void VectorMap::paintBase( GeoPainter * painter, ViewportParams* viewport,
			   bool antialiasing )
{
    // The paintBase function is projection dependent, and it's not
    // just a mathematical conversion, so it's located in the
    // projection helper.
    viewport->currentProjection()->helper()->paintBase( painter, viewport,
							m_pen, m_brush,
							antialiasing );
}


void VectorMap::drawMap( QPaintDevice *origimg, bool antialiasing,
			 ViewportParams *viewport, MapQuality mapQuality )
{
    bool doClip = false; //assume false
    switch( viewport->projection() ) {
        case Spherical:
            doClip = ( viewport->radius() > m_imgrx
		       || viewport->radius() > m_imgry );
            break;
        case Equirectangular:
            doClip = true; // clipping should always be enabled
            break;
        case Mercator:
            doClip = true; // clipping should always be enabled
            break;
    }

    GeoPainter  painter( origimg, viewport, mapQuality, doClip );
    painter.setRenderHint( QPainter::Antialiasing, antialiasing );
    painter.setPen( m_pen );
    painter.setBrush( m_brush );

    ScreenPolygon::Vector::const_iterator  itEndPolygon = end();
    for ( ScreenPolygon::Vector::const_iterator itPolygon = begin();
          itPolygon != itEndPolygon; 
          ++itPolygon )
    {
        if ( itPolygon->closed() )  
            painter.drawPolygon( *itPolygon );
        else
            painter.drawPolyline( *itPolygon );
    }
}


// Paint the prepared vectors in screen coordinates.

void VectorMap::paintMap(GeoPainter * painter, bool antialiasing)
{
    painter->setRenderHint( QPainter::Antialiasing, antialiasing );

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
    //	ya = sqrt( ((double)m_radius + 1) * ( (double)m_radius + 1) - xa*xa);
    ya = ( m_rlimit > xa * xa )
        ? sqrt( (double)(m_rlimit) - (double)( xa * xa ) ) : 0;
    // qDebug() << " m_rlimit" << m_rlimit << " xa*xa" << xa*xa << " ya: " << ya;
    if ( ( m_currentPoint.y() - ( m_imgry + 1 ) ) < 0 )
        ya = -ya; 

    return QPointF( m_imgrx + xa + 1, m_imgry + ya + 1 );
}


void VectorMap::createArc()
{

    double  beta  = (double)( RAD2DEG 
			      * atan2( m_horizonb.y() - m_imgry - 1,
				       m_horizonb.x() - m_imgrx - 1 ) );
    double  alpha = (double)( RAD2DEG
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
            double angle = DEG2RAD * (double)( alpha + (sgndiff * it) );
            itx = (int)( m_imgrx +  arcradius * cos( angle ) + 1 );
            ity = (int)( m_imgry +  arcradius * sin( angle ) + 1 );
            // qDebug() << " ity: " << ity;
            m_polygon.append( QPoint( itx, ity ) );		
        }

        m_polygon.append( m_horizonb );
    }
}


void VectorMap::resizeMap( int width, int height )
{
    m_imgwidth  = width;
    m_imgheight = height;

    m_imgrx = ( m_imgwidth  / 2 );
    m_imgry = ( m_imgheight / 2 );
}


int VectorMap::getDetailLevel( int radius ) const
{
    int detail = 5;
	
    if ( radius > 5000 )      detail = 0;
    else if ( radius > 2500 ) detail = 1;
    else if ( radius > 1000 ) detail = 2;
    else if ( radius >  600 ) detail = 3;
    else if ( radius >   50 ) detail = 4;

    return detail;
}
