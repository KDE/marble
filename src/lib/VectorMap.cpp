//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//


#include "VectorMap.h"

#include <cmath>
#include <cstdlib>

#include <QtCore/QVector>
#include <QtGui/QColor>

#include "MarbleDebug.h"
#include "global.h"
#include "AbstractProjection.h"
#include "GeoPainter.h"
#include "GeoPolygon.h"
#include "ViewportParams.h"
#include "MathHelper.h"

// #define VECMAP_DEBUG 

using namespace Marble;

VectorMap::VectorMap()
    : m_zBoundingBoxLimit( 0.0 ),
      m_zPointLimit( 0.0 )
      // m_debugNodeCount( 0 )
{
}

VectorMap::~VectorMap()
{
}


void VectorMap::createFromPntMap( const PntMap* pntmap, 
                                  const ViewportParams* viewport )
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
                                           const ViewportParams* viewport )
{
    m_polygons.clear();

    // We must use qreal or int64 for the calculations because we
    // square radius sometimes below, and it may cause an overflow. We
    // choose qreal because of some sqrt() calculations.
    qreal   radius     = viewport->radius();
    qreal   imgradius2 = ( viewport->width()  / 2 ) * ( viewport->width()  / 2 ) + ( viewport->height() / 2 ) * ( viewport->height() / 2 );

    // zlimit: describes the lowest z value of the sphere that is
    //         visible on the screen.  This should happen in the
    //         corners.
    qreal zlimit = ( ( imgradius2 < radius * radius )
                      ? sqrt( 1 - imgradius2 / ( radius * radius ) )
                      : 0.0 );
    // mDebug() << "zlimit: " << zlimit;

    m_zBoundingBoxLimit = ( ( m_zBoundingBoxLimit >= 0.0
                              && zlimit < m_zBoundingBoxLimit )
                            || m_zBoundingBoxLimit < 0.0 )
                     ? zlimit : m_zBoundingBoxLimit;
    m_zPointLimit = ( ( m_zPointLimit >= 0.0 && zlimit < m_zPointLimit )
                      || m_zPointLimit < 0.0 )
                     ? zlimit : m_zPointLimit;

    viewport->planetAxis().inverse().toMatrix( m_rotMatrix );
    GeoPolygon::PtrVector::ConstIterator  itPolyLine = pntmap->constBegin();
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = pntmap->constEnd();

    //	const int detail = 0;
    const int  detail = getDetailLevel( viewport->radius() );

    for (; itPolyLine != itEndPolyLine; ++itPolyLine )
    {
        // This sorts out polygons by bounding box which aren't visible at all.
        GeoDataCoordinates::PtrVector boundary = (*itPolyLine)->getBoundary();
        // rather paint an invalid line then crashing here if the boundaries are not loaded yet
        if(boundary.size() < 5) continue;

        for ( int i = 0; i < 5; ++i ) {
            Quaternion qbound = boundary[i]->quaternion();

            qbound.rotateAroundAxis(m_rotMatrix); 
            if ( qbound.v[Q_Z] > m_zBoundingBoxLimit ) {
                // if (qbound.v[Q_Z] > 0){
                m_polygon.clear();
                m_polygon.reserve( (*itPolyLine)->size() );
                m_polygon.setClosed( (*itPolyLine)->getClosed() );

                // mDebug() << i << " Visible: YES";
                sphericalCreatePolyLine( (*itPolyLine)->constBegin(),
                                         (*itPolyLine)->constEnd(), detail, viewport );

                break; // abort foreach test of current boundary
            } 
            // else
            //     mDebug() << i << " Visible: NOT";
        }
    }
}

void VectorMap::rectangularCreateFromPntMap( const PntMap* pntmap, 
                                             const ViewportParams* viewport )
{
    m_polygons.clear();
    int  radius = viewport->radius();

    // Calculate translation of center point
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    qreal rad2Pixel = (float)( 2 * radius ) / M_PI;
    qreal lon, lat;
    qreal x, y;

    viewport->planetAxis().inverse().toMatrix( m_rotMatrix );
    GeoPolygon::PtrVector::ConstIterator  itPolyLine = pntmap->constBegin();
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = pntmap->constEnd();

    ScreenPolygon  boundingPolygon;
    QRectF         visibleArea ( 0, 0, viewport->width(), viewport->height() );
    const int      detail = getDetailLevel( radius );

    for (; itPolyLine != itEndPolyLine; ++itPolyLine )
    {
        // This sorts out polygons by bounding box which aren't visible at all.
        int offset = 0;

        GeoDataCoordinates::PtrVector  boundary = (*itPolyLine)->getBoundary();
        boundingPolygon.clear();

        // Let's just use the top left and the bottom right bounding
        // box point for this projection.

        // rather paint an invalid line then crashing here if the boundaries are not loaded yet
        if(boundary.size() < 3) continue;

        for ( int i = 1; i < 3; ++i ) {
            boundary[i]->geoCoordinates(lon, lat);
            x = (qreal)(viewport->width())  / 2.0 - rad2Pixel * (centerLon - lon);
            y = (qreal)(viewport->height()) / 2.0 + rad2Pixel * (centerLat - lat);
            boundingPolygon << QPointF( x, y );
        }

        if ( boundingPolygon.at(0).x() < 0 || boundingPolygon.at(1).x() < 0 ) {
            boundingPolygon.translate( 4 * radius, 0 );
            offset += 4 * radius;
        }

        do {
            offset -= 4 * radius;
            boundingPolygon.translate( -4 * radius, 0 );
	    // FIXME: Get rid of this really fugly code once we have a
	    //        proper LatLonBox check implemented and in place.
        } while( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		   && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		 || ( (*itPolyLine)->getDateLine() == GeoPolygon::Even
		      && ( visibleArea.intersects( QRectF( boundingPolygon.at(1),
                                                           QPointF( (qreal)(viewport->width()) / 2.0
                                                                    - rad2Pixel * ( centerLon - M_PI )
                                                                    + offset,
                                                                    boundingPolygon.at(0).y() ) ) )
                           || visibleArea.intersects( QRectF( QPointF( (qreal)(viewport->width()) / 2.0
                                                                       - rad2Pixel * ( centerLon
                                                                                       + M_PI )
                                                                       + offset,
                                                                       boundingPolygon.at(1).y() ),
                                                              boundingPolygon.at(0) ) ) ) ) );
        offset += 4 * radius;
        boundingPolygon.translate( 4 * radius, 0 );

	// FIXME: Get rid of this really fugly code once we will have
	//        a proper LatLonBox check implemented and in place.
        while ( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		  && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		|| ( (*itPolyLine)->getDateLine() == GeoPolygon::Even 
		     && ( visibleArea.intersects(
			    QRectF( boundingPolygon.at(1),
				    QPointF( (qreal)(viewport->width()) / 2.0
					     - rad2Pixel * ( centerLon - M_PI )
                                             + offset,
					     boundingPolygon.at(0).y() ) ) ) 
			  || visibleArea.intersects(
			         QRectF( QPointF( (qreal)(viewport->width()) / 2.0
						  - rad2Pixel * ( centerLon + M_PI )
                                                  + offset,
						  boundingPolygon.at(1).y() ),
					 boundingPolygon.at(0) ) ) ) )
		) 
	{

            m_polygon.clear();
            m_polygon.reserve( (*itPolyLine)->size() );
            m_polygon.setClosed( (*itPolyLine)->getClosed() );

            rectangularCreatePolyLine( (*itPolyLine)->constBegin(),
                                       (*itPolyLine)->constEnd(), detail, viewport, offset );

            offset += 4 * radius;
            boundingPolygon.translate( 4 * radius, 0 );
        }
    }
}

void VectorMap::mercatorCreateFromPntMap( const PntMap* pntmap,
                                          const ViewportParams* viewport )
{
    m_polygons.clear();
    int  radius = viewport->radius();

    // Calculate translation of center point
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    qreal rad2Pixel = (float)( 2 * radius ) / M_PI;
    qreal lon, lat;
    qreal x, y;

    viewport->planetAxis().inverse().toMatrix( m_rotMatrix );
    GeoPolygon::PtrVector::ConstIterator  itPolyLine = pntmap->constBegin();
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = pntmap->constEnd();

    ScreenPolygon  boundingPolygon;
    QRectF         visibleArea ( 0, 0, viewport->width(), viewport->height() );
    const int      detail = getDetailLevel( radius );

    for (; itPolyLine != itEndPolyLine; ++itPolyLine )
    {
        // This sorts out polygons by bounding box which aren't visible at all.
        int offset = 0;

        GeoDataCoordinates::PtrVector boundary = (*itPolyLine)->getBoundary();
        boundingPolygon.clear();

        // Let's just use the top left and the bottom right bounding box point for 
        // this projection

        // rather paint an invalid line then crashing here if the boundaries are not loaded yet
        if(boundary.size() < 3) continue;

        for ( int i = 1; i < 3; ++i ) {
            boundary[i]->geoCoordinates(lon, lat);
            x = (qreal)(viewport->width())  / 2.0 + rad2Pixel * (lon - centerLon);
            y = (qreal)(viewport->height()) / 2.0 - rad2Pixel * ( atanh( sin( lat ) )
                                                           - atanh( sin( centerLat ) ) );

            boundingPolygon << QPointF( x, y );
        }

        if ( boundingPolygon.at(0).x() < 0 || boundingPolygon.at(1).x() < 0 ) {
            boundingPolygon.translate( 4 * radius, 0 );
            offset += 4 * radius;
        }

        do {
            offset -= 4 * radius;
            boundingPolygon.translate( -4 * radius, 0 );
	    // FIXME: Get rid of this really fugly code once we have a
	    //        proper LatLonBox check implemented and in place.
        } while( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		   && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		 || ( (*itPolyLine)->getDateLine() == GeoPolygon::Even
		      && ( visibleArea.intersects( QRectF( boundingPolygon.at(1),
                                                           QPointF( (qreal)(viewport->width()) / 2.0
                                                                    - rad2Pixel * ( centerLon
                                                                                    - M_PI )
                                                                    + offset,
                                                                    boundingPolygon.at(0).y() ) ) )
                           || visibleArea.intersects( QRectF( QPointF( (qreal)(viewport->width()) / 2.0
                                                                       - rad2Pixel * ( centerLon
                                                                                       + M_PI )
                                                                       + offset,
                                                                       boundingPolygon.at(1).y() ),
                                                              boundingPolygon.at(0) ) ) ) ) );
        offset += 4 * radius;
        boundingPolygon.translate( 4 * radius, 0 );

	// FIXME: Get rid of this really fugly code once we will have
	//        a proper LatLonBox check implemented and in place.
        while ( ( (*itPolyLine)->getDateLine() != GeoPolygon::Even 
		  && visibleArea.intersects( (QRectF)( boundingPolygon.boundingRect() ) ) )
		|| ( (*itPolyLine)->getDateLine() == GeoPolygon::Even 
		     && ( visibleArea.intersects(
			    QRectF( boundingPolygon.at(1),
				    QPointF( (qreal)(viewport->width()) / 2.0
					     - rad2Pixel * ( centerLon - M_PI )
                                             + offset,
					     boundingPolygon.at(0).y() ) ) ) 
			  || visibleArea.intersects(
			         QRectF( QPointF( (qreal)(viewport->width()) / 2.0
						  - rad2Pixel * ( centerLon + M_PI )
                                                  + offset,
						  boundingPolygon.at(1).y() ),
					 boundingPolygon.at(0) ) ) ) )
		)
	{
            m_polygon.clear();
            m_polygon.reserve( (*itPolyLine)->size() );
            m_polygon.setClosed( (*itPolyLine)->getClosed() );

            mercatorCreatePolyLine( (*itPolyLine)->constBegin(),
                                    (*itPolyLine)->constEnd(), detail, viewport, offset );

            offset += 4 * radius;
            boundingPolygon.translate( 4 * radius, 0 );
        }
    }
}

void VectorMap::sphericalCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator const & itStartPoint,
                                         GeoDataCoordinates::Vector::ConstIterator const & itEndPoint,
                                         const int detail, const ViewportParams *viewport )
{
    const int radius = viewport->radius();

    const int rLimit = (int)( ( radius * radius )
                      * (1.0 - m_zPointLimit * m_zPointLimit ) );

    QPointF lastPoint;
    bool firsthorizon = false;
    bool currentlyvisible = false;
    bool horizonpair = false;
    QPointF firstHorizonPoint;
    QPointF horizona;

    GeoDataCoordinates::Vector::const_iterator itPoint = itStartPoint;
    for (; itPoint != itEndPoint; ++itPoint ) {
        if ( itPoint->detail() < detail )
            continue;

	// Calculate polygon nodes
#ifdef VECMAP_DEBUG
	++m_debugNodeCount;
#endif
        Quaternion qpos = itPoint->quaternion();
        qpos.rotateAroundAxis( m_rotMatrix );
        const QPointF currentPoint( ( viewport->width()  / 2 ) + radius * qpos.v[Q_X] + 1.0,
                                    ( viewport->height() / 2 ) - radius * qpos.v[Q_Y] + 1.0 );

	// Take care of horizon crossings if horizon is visible
        bool lastvisible = currentlyvisible;

	// Less accurate:
	// currentlyvisible = (qpos.v[Q_Z] >= m_zPointLimit) ? true : false;
        currentlyvisible = ( qpos.v[Q_Z] >= 0 );
	if ( itPoint == itStartPoint ) {
	    // qDebug("Initializing scheduled new PolyLine");
            lastvisible  = currentlyvisible;
            lastPoint    = QPointF( currentPoint.x() + 1.0,
                                      currentPoint.y() + 1.0 );
        }

        if ( currentlyvisible != lastvisible ) {
            // qDebug("Crossing horizon line");
            // if (!currentlyvisible) qDebug("Leaving visible hemisphere");
            // else qDebug("Entering visible hemisphere");

            if ( !horizonpair ) {
                // qDebug("Point A");

                if ( !currentlyvisible ) {
                    horizona    = horizonPoint(viewport, currentPoint, rLimit);
                    horizonpair = true;
                }
                else {
                    // qDebug("Orphaned");
                    firstHorizonPoint = horizonPoint(viewport, currentPoint, rLimit);
                    firsthorizon      = true;
                }
            }
            else {
                // qDebug("Point B");
                const QPointF horizonb = horizonPoint(viewport, currentPoint, rLimit);

                createArc(viewport, horizona, horizonb, rLimit);
                horizonpair = false;
            }
        }

	// Take care of screencrossing crossings if horizon is visible.
	// Filter Points which aren't on the visible Hemisphere.
        if ( currentlyvisible && currentPoint != lastPoint ) {
	    // most recent addition: currentPoint != lastPoint
            m_polygon << currentPoint;
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

        lastPoint = currentPoint;
    }

    // In case of horizon crossings, make sure that we always get a
    // polygon closed correctly.
    if ( firsthorizon ) {
        const QPointF horizonb = firstHorizonPoint;
        if (m_polygon.closed())
            createArc(viewport, horizona, horizonb, rLimit);

        firsthorizon = false;
    }

    // Avoid polygons degenerated to Points.
    if ( m_polygon.size() >= 2 ) {
        m_polygons.append(m_polygon);
    }
}

void VectorMap::rectangularCreatePolyLine(
    GeoDataCoordinates::Vector::ConstIterator const & itStartPoint,
    GeoDataCoordinates::Vector::ConstIterator const & itEndPoint,
    const int detail, const ViewportParams *viewport, int offset )
{
    // Calculate translation of center point
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    // Other convenience variables
    const qreal  rad2Pixel = (float)( 2 * viewport->radius() ) / M_PI;

    ScreenPolygon otherPolygon;
    otherPolygon.setClosed ( m_polygon.closed() );
    bool CrossedDateline = false;
    bool firstPoint = true;
    int lastSign = 0;
    qreal lastLon, lastLat;

    GeoDataCoordinates::Vector::const_iterator itPoint = itStartPoint;
    for (; itPoint != itEndPoint; ++itPoint ) {
        // remain -= step;
        if ( itPoint->detail() < detail )
	    continue;

	// Calculate polygon nodes
#ifdef VECMAP_DEBUG
	++m_debugNodeCount;
#endif

        qreal lon, lat;
        itPoint->geoCoordinates( lon, lat);
        const qreal x = (qreal)(viewport->width())  / 2.0 - rad2Pixel * (centerLon - lon) + offset;
        const qreal y = (qreal)(viewport->height()) / 2.0 + rad2Pixel * (centerLat - lat);
        int currentSign = ( lon > 0.0 ) ? 1 : -1 ;
	if ( firstPoint ) {
	    firstPoint = false;
            lastSign = currentSign;
	}

        const QPointF currentPoint = QPointF( x, y );

        // Correction of the Dateline
        if ( lastSign != currentSign && fabs(lastLon) + fabs(lon) > M_PI ) {

	    // X coordinate on the screen for the points on the
	    // dateline on both sides of the flat map.
	    qreal lastXAtDateLine = (qreal)(viewport->width()) / 2.0
                + rad2Pixel * ( lastSign * M_PI - centerLon ) + offset;
	    qreal xAtDateLine = (qreal)(viewport->width()) / 2.0
                + rad2Pixel * ( -lastSign * M_PI - centerLon ) + offset;
	    qreal lastYAtDateLine = (qreal)(viewport->height()) / 2.0
                - ( lastLat - centerLat ) * rad2Pixel;
	    qreal yAtSouthPole = (qreal)(viewport->height()) / 2.0
                - ( -viewport->currentProjection()->maxLat() - centerLat ) * rad2Pixel;

	    //If the "jump" occurs in the Anctartica's latitudes

	    if ( lat < - M_PI / 3 ) {
		// FIXME: This should actually need to get investigated
		//        in GeoPainter.  For now though we just help
		//        GeoPainter to get the clipping right.
		if ( lastXAtDateLine > (qreal)(viewport->width()) - 1.0 )
		    lastXAtDateLine = (qreal)(viewport->width()) - 1.0;
		if ( lastXAtDateLine < 0.0 )
		    lastXAtDateLine = 0.0; 
		if ( xAtDateLine > (qreal)(viewport->width()) - 1.0 )
		    xAtDateLine = (qreal)(viewport->width()) - 1.0;
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
            otherPolygon << currentPoint;
	else
            m_polygon << currentPoint;

        lastLon  = lon;
        lastLat  = lat;
        lastSign = currentSign;
    }

    // Avoid polygons degenerated to Points.
    if ( m_polygon.size() >= 2 ) {
        m_polygons.append(m_polygon);
    }

    if ( otherPolygon.size() >= 2 ) {
        m_polygons.append( otherPolygon );
    }
}

void VectorMap::mercatorCreatePolyLine(
        GeoDataCoordinates::Vector::ConstIterator const & itStartPoint,
        GeoDataCoordinates::Vector::ConstIterator const & itEndPoint,
        const int detail,
        const ViewportParams *viewport,
        int offset )
{
    // Calculate translation of center point
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    // Other convenience variables
    const qreal  rad2Pixel = (qreal)( 2 * viewport->radius() ) / M_PI;

    ScreenPolygon  otherPolygon;
    otherPolygon.setClosed ( m_polygon.closed() );

    bool    CrossedDateline = false;
    bool    firstPoint      = true;
    int lastSign = 0;
    qreal lastLon, lastLat;

    GeoDataCoordinates::Vector::const_iterator itPoint = itStartPoint;
    for (; itPoint != itEndPoint; ++itPoint ) {
        // remain -= step;
        if ( itPoint->detail() < detail )
	    continue;

	// Calculate polygon nodes
#ifdef VECMAP_DEBUG
	++m_debugNodeCount;
#endif

        // FIXME: Call the projection.  Unfortunately there is no
        //        screenCoordinates taking qreals.
        qreal lon, lat;
        itPoint->geoCoordinates( lon, lat );

    // Removing all points beyond +/- 85 deg for Mercator:
    if ( fabs( lat ) > viewport->currentProjection()->maxLat() )
        continue;

        const qreal x = (qreal)(viewport->width())  / 2.0 + rad2Pixel * (lon - centerLon) + offset;
        const qreal y = (qreal)(viewport->height()) / 2.0
            - rad2Pixel * ( atanh( sin( lat ) ) - atanh( sin( centerLat ) ) );
        int currentSign = ( lon > 0.0 ) ? 1 : -1 ;
	if ( firstPoint ) {
	    firstPoint = false;
            lastSign = currentSign;
	}

        const QPointF currentPoint = QPointF( x, y );

	//correction of the Dateline

        if ( lastSign != currentSign && fabs(lastLon) + fabs(lon) > M_PI ) {

	    // x coordinate on the screen for the points on the dateline on both
	    // sides of the flat map.
	    // FIXME: mercator projection here too.
	    qreal lastXAtDateLine = (qreal)(viewport->width()) / 2.0
                + rad2Pixel * ( lastSign * M_PI - centerLon ) + offset;
	    qreal xAtDateLine = (qreal)(viewport->width()) / 2.0
                + rad2Pixel * ( -lastSign * M_PI - centerLon ) + offset;
        qreal lastYAtDateLine = (qreal)( viewport->height() / 2 - rad2Pixel
                                         * ( atanh( sin( lastLat ) )
                                             - atanh( sin( centerLat ) ) ) );
        qreal yAtSouthPole = (qreal)( viewport->height() / 2
                                      - rad2Pixel * ( atanh( sin( -viewport->currentProjection()->
                                                                  maxLat() ) )
                                                      - atanh( sin( centerLat ) ) ) );

	    //If the "jump" occurs in the Anctartica's latitudes

	    if ( lat < - M_PI / 3 ) {
		// FIXME: This should actually need to get investigated
		//        in GeoPainter.  For now though we just help
		//        GeoPainter to get the clipping right.
		if ( lastXAtDateLine > (qreal)(viewport->width()) - 1.0 )
		    lastXAtDateLine = (qreal)(viewport->width()) - 1.0;
		if ( lastXAtDateLine < 0.0 )
		    lastXAtDateLine = 0.0; 
		if ( xAtDateLine > (qreal)(viewport->width()) - 1.0 )
		    xAtDateLine = (qreal)(viewport->width()) - 1.0;
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
            otherPolygon << currentPoint;
	else
            m_polygon << currentPoint;

        lastLon  = lon;
        lastLat  = lat;
        lastSign = currentSign;
    }

    // Avoid polygons degenerated to Points.
    if ( m_polygon.size() >= 2 ) {
        m_polygons.append(m_polygon);
    }

    if ( otherPolygon.size() >= 2 ) {
        m_polygons.append( otherPolygon );
    }
}


void VectorMap::drawMap( GeoPainter *painter )
{
    ScreenPolygon::Vector::const_iterator  itEndPolygon = m_polygons.constEnd();
    for ( ScreenPolygon::Vector::const_iterator itPolygon = m_polygons.constBegin();
          itPolygon != itEndPolygon; 
          ++itPolygon )
    {
        if ( itPolygon->closed() )  
            painter->drawPolygon( *itPolygon );
        else
            painter->drawPolyline( *itPolygon );
    }
}


// Paint the prepared vectors in screen coordinates.

void VectorMap::paintMap(GeoPainter * painter)
{
    ScreenPolygon::Vector::const_iterator  itEndPolygon = m_polygons.constEnd();

    for ( ScreenPolygon::Vector::const_iterator itPolygon = m_polygons.constBegin();
          itPolygon != itEndPolygon;
          ++itPolygon )
    {
        if ( itPolygon->closed() )
            painter->drawPolygon( *itPolygon );
        else
            painter->drawPolyline( *itPolygon );
    }
}


QPointF VectorMap::horizonPoint( const ViewportParams *viewport, const QPointF &currentPoint, int rLimit ) const
{
    // qDebug("Interpolating");
    const qreal xa = currentPoint.x() - ( ( viewport->width()  / 2 ) + 1 );

    // Move the currentPoint along the y-axis to match the horizon.
    // ya = sqrt( ((qreal)m_radius + 1) * ( (qreal)m_radius + 1) - xa*xa);
    qreal ya = ( rLimit > xa * xa )
        ? sqrt( (qreal)(rLimit) - (qreal)( xa * xa ) ) : 0;
    // mDebug() << " m_rlimit" << m_rlimit << " xa*xa" << xa*xa << " ya: " << ya;
    if ( ( currentPoint.y() - ( ( viewport->height() / 2 ) + 1 ) ) < 0 )
        ya = -ya; 

    return QPointF( ( viewport->width()  / 2 ) + xa + 1, ( viewport->height() / 2 ) + ya + 1 );
}


void VectorMap::createArc( const ViewportParams *viewport, const QPointF &horizona, const QPointF &horizonb, int rLimit )
{

    qreal  beta  = (qreal)( RAD2DEG 
                              * atan2( horizonb.y() - ( viewport->height() / 2 ) - 1,
                                       horizonb.x() - ( viewport->width()  / 2 ) - 1 ) );
    qreal  alpha = (qreal)( RAD2DEG
                              * atan2( horizona.y() - ( viewport->height() / 2 ) - 1,
                                       horizona.x() - ( viewport->width()  / 2 ) - 1 ) );

    qreal diff = beta - alpha;

    if ( diff != 0.0 && diff != 180.0 && diff != -180.0 ) {

        m_polygon.append( horizona );

        qreal sgndiff = diff / fabs(diff);

        if (fabs(diff) > 180.0)
            diff = - sgndiff * (360.0 - fabs(diff));

        // Reassigning sgndiff this way seems dull
        sgndiff = diff / fabs(diff);
        // mDebug() << "SGN: " << sgndiff;

        // qDebug () << " beta: " << beta << " alpha " << alpha << " diff: " << diff;
	
        int  itx;
        int  ity;
        // mDebug() << "r: " << (m_radius+1) << "rn: " << sqrt((qreal)(m_rlimit));
        qreal  arcradius = sqrt( (qreal)( rLimit ) );

        for ( int it = 1; it < fabs(diff); ++it ) {
            qreal angle = DEG2RAD * (qreal)( alpha + (sgndiff * it) );
            itx = (int)( ( viewport->width()  / 2 ) +  arcradius * cos( angle ) + 1 );
            ity = (int)( ( viewport->height() / 2 ) +  arcradius * sin( angle ) + 1 );
            // mDebug() << " ity: " << ity;
            m_polygon.append( QPoint( itx, ity ) );		
        }

        m_polygon.append( horizonb );
    }
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
