//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>"


#include "GeoPainter.h"

#include <QtCore/QDebug>
#include <QtGui/QPainterPath>

#include "AbstractProjection.h"
#include "GeoDataPoint.h"
#include "global.h"
#include "ViewportParams.h"

// #define MARBLE_DEBUG
using namespace Marble;


class GeoPainterPrivate
{
 public:
    GeoPainterPrivate( ViewportParams *viewport, MapQuality mapQuality )
        : m_viewport( viewport ),
	  m_mapQuality( mapQuality )

    {
        m_x = new int[100];
    }

    ~GeoPainterPrivate()
    {
        delete[] m_x;
    }

//  TODO: Additionally consider dateline
//  TODO: Add Interpolation of points in case of globeHidesPoint points
//  TODO: Implement isGeoProjected = true case using SLERP

    void createPolygonsFromPoints( const GeoDataPoint *points, int pointCount, 
				   QVector<QPolygon *> &polygons, 
				   bool isGeoProjected = false )
    {
        int x, y;
        bool previousGlobeHidesPoint;
        AbstractProjection *projection = m_viewport->currentProjection();

        if ( isGeoProjected == false ) {

            QPolygon* polygon;
	    //QVector<QPolygon *> polygons;

            GeoDataPoint *itPoint = const_cast<GeoDataPoint *>( points );
            while( itPoint < points + pointCount ) {
                bool globeHidesPoint;
                //bool isVisible = projection->screenCoordinates( *itPoint, m_viewport, x, y, globeHidesPoint );
                if ( itPoint == points ){
                    polygon = new QPolygon;
                    previousGlobeHidesPoint = globeHidesPoint;
                }

                if ( globeHidesPoint && !previousGlobeHidesPoint ) {
                    polygons.append( polygon );
                    polygon = new QPolygon;
                }

                if ( !globeHidesPoint ) {
                    polygon->append( QPoint( x, y ) );
                }

                previousGlobeHidesPoint = globeHidesPoint;
                ++itPoint;
            }

            if ( polygon->size() > 1 ){
                polygons.append( polygon );
            }
        }
    }

    void createAnnotationLayout (  int x, int y, QSize bubbleSize, int bubbleOffsetX, int bubbleOffsetY, int xRnd, int yRnd, QPainterPath& path, QRectF& rect )
    {
        // TODO: MOVE this into an own Annotation class
        double arrowPosition = 0.3;
        double arrowWidth = 12.0;

        double width =  (double)( bubbleSize.width() );
        double height = (double)( bubbleSize.height() );

        double dx = ( bubbleOffsetX > 0 ) ? 1.0 : -1.0; // x-Mirror
        double dy = ( bubbleOffsetY < 0 ) ? 1.0 : -1.0; // y-Mirror

        double x0 = (double) ( x + bubbleOffsetX ) - dx * ( 1.0 - arrowPosition ) * ( width - 2.0 * xRnd ) - xRnd *dx;
        double x1 = (double) ( x + bubbleOffsetX ) - dx * ( 1.0 - arrowPosition ) * ( width - 2.0 * xRnd );
        double x2 = (double) ( x + bubbleOffsetX ) - dx * ( 1.0 - arrowPosition ) * ( width - 2.0 * xRnd ) + xRnd * dx;
        double x3 = (double) ( x + bubbleOffsetX ) - dx * arrowWidth / 2.0;
        double x4 = (double) ( x + bubbleOffsetX ) + dx * arrowWidth / 2.0;
        double x5 = (double) ( x + bubbleOffsetX ) + dx * arrowPosition * ( width - 2.0 * xRnd )- xRnd * dx;
        double x6 = (double) ( x + bubbleOffsetX ) + dx * arrowPosition * ( width - 2.0 * xRnd );
        double x7 = (double) ( x + bubbleOffsetX ) + dx * arrowPosition * ( width - 2.0 * xRnd ) + xRnd * dx;

        double y0 = (double) ( y + bubbleOffsetY );
        double y1 = (double) ( y + bubbleOffsetY ) - dy * yRnd;
        double y2 = (double) ( y + bubbleOffsetY ) - dy * 2 * yRnd;
        double y5 = (double) ( y + bubbleOffsetY ) - dy * ( height - 2 * yRnd );
        double y6 = (double) ( y + bubbleOffsetY ) - dy * ( height - yRnd );
        double y7 = (double) ( y + bubbleOffsetY ) - dy * height;

        QPointF p1 ( x, y ); // pointing point
        QPointF p2 ( x4, y0 );
        QPointF p3 ( x6, y0 );
        QPointF p4 ( x7, y1 );
        QPointF p5 ( x7, y6 );
        QPointF p6 ( x6, y7 );
        QPointF p7 ( x1, y7 );
        QPointF p8 ( x0, y6 );
        QPointF p9 ( x0, y1 );
        QPointF p10( x1, y0 );
        QPointF p11( x3, y0 );

        QRectF bubbleBoundingBox(  QPointF( x0, y7 ), QPointF( x7, y0 ) );

        path.moveTo( p1 );
        path.lineTo( p2 );

        path.lineTo( p3 );
        QRectF bottomRight( QPointF( x5, y2 ), QPointF( x7, y0 ) );
        path.arcTo( bottomRight, 270.0, 90.0 );

        path.lineTo( p5 );
        QRectF topRight( QPointF( x5, y7 ), QPointF( x7, y5 ) );
        path.arcTo( topRight, 0.0, 90.0 );

        path.lineTo( p7 );
        QRectF topLeft( QPointF( x0, y7 ), QPointF( x2, y5 ) );
        path.arcTo( topLeft, 90.0, 90.0 );

        path.lineTo( p9 );
        QRectF bottomLeft( QPointF( x0, y2 ), QPointF( x2, y0 ) );
        path.arcTo( bottomLeft, 180.0, 90.0 );

        path.lineTo( p10 );
        path.lineTo( p11 );
        path.lineTo( p1 );

        double left   = ( dx > 0 ) ? x1 : x6;
        double right  = ( dx > 0 ) ? x6 : x1;
        double top    = ( dy > 0 ) ? y6 : y1;
        double bottom = ( dy > 0 ) ? y1 : y6;

        rect.setTopLeft( QPointF( left, top ) );
        rect.setBottomRight( QPointF( right, bottom ) );
    }

    ViewportParams  *m_viewport;
    MapQuality       m_mapQuality;

    int             *m_x;
};

GeoPainter::GeoPainter( QPaintDevice* pd, ViewportParams *viewport,
			MapQuality mapQuality, bool clip )
    : ClipPainter( pd, clip ),
      d( new GeoPainterPrivate( viewport, mapQuality ) )
{
}

GeoPainter::~GeoPainter()
{
    delete d;
}

void GeoPainter::autoMapQuality ()
{
    bool antialiased = false;

    if (   d->m_mapQuality == Marble::High
        || d->m_mapQuality == Marble::Print ) {
            antialiased = true;
    }

    setRenderHint( QPainter::Antialiasing, antialiased );
}

void GeoPainter::drawAnnotation (  const GeoDataPoint & position, const QString & text, QSize bubbleSize, int bubbleOffsetX, int bubbleOffsetY, int xRnd, int yRnd )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    bool visible = projection->screenCoordinates( position, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

    if ( visible ) {
        // Draw all the x-repeat-instances of the point on the screen
        for( int it = 0; it < pointRepeatNum; ++it ) {
            QPainterPath path;
            QRectF rect;
            d->createAnnotationLayout( d->m_x[it], y, bubbleSize, bubbleOffsetX, bubbleOffsetY,  xRnd, yRnd, path, rect );
            QPainter::drawPath( path );
            QPainter::drawText( rect, Qt::TextWordWrap, text, &rect );
        }
    }
}

void GeoPainter::drawPoint (  const GeoDataPoint & position )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    bool visible = projection->screenCoordinates( position, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

    if ( visible ) {
        // Draw all the x-repeat-instances of the point on the screen
        for( int it = 0; it < pointRepeatNum; ++it ) {
            QPainter::drawPoint( d->m_x[it], y );
        }
    }
}

void GeoPainter::drawPoints (  const GeoDataPoint * points, int pointCount )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    GeoDataPoint * itPoint = const_cast<GeoDataPoint *>( points );
    while( itPoint < points + pointCount ) {
        bool visible = projection->screenCoordinates( *itPoint, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );
    
        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawPoint( d->m_x[it], y );
            }
        }
    }
}

void GeoPainter::drawText ( const GeoDataPoint & position, const QString & text )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    bool visible = projection->screenCoordinates( position, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

    if ( visible ) {
        // Draw all the x-repeat-instances of the point on the screen
        for( int it = 0; it < pointRepeatNum; ++it ) {
            QPainter::drawText( d->m_x[it], y, text );
        }
    }
}

void GeoPainter::drawEllipse ( const GeoDataPoint & centerPoint, int width, int height, bool isGeoProjected )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( centerPoint, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawEllipse(  d->m_x[it] - width / 2, y - height / 2, width, height  );
            }
        }
    }
}

void GeoPainter::drawImage ( const GeoDataPoint & centerPoint, const QImage & image, bool isGeoProjected )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( centerPoint, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawImage( d->m_x[it] - ( image.width() / 2 ), y - ( image.height() / 2 ), image );
            }
        }
    }
}

void GeoPainter::drawPixmap ( const GeoDataPoint & centerPoint, const QPixmap & pixmap, bool isGeoProjected )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( centerPoint, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawPixmap( d->m_x[it] - ( pixmap.width() / 2 ), y - ( pixmap.height() / 2 ), pixmap );
            }
        }
    }
}

void GeoPainter::drawLine (  const GeoDataPoint & p1,  const GeoDataPoint & p2, bool isGeoProjected )
{
    int x1, y1, x2, y2;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible1 = projection->screenCoordinates( p1, d->m_viewport, x1, y1 );
        bool visible2 = projection->screenCoordinates( p2, d->m_viewport, x2, y2 );

        if ( ( !visible1 && p2.altitude() > 0 ) || ( !visible2 && p1.altitude() >0 ) )
            return;

        QPolygonF polygon;
        polygon << QPointF( x1, y1 ) << QPointF( x2, y2 );

        ClipPainter::drawPolyline( polygon );
    }
}

void GeoPainter::drawPolyline ( const GeoDataPoint * points, int pointCount, bool isGeoProjected )
{
    QVector<QPolygon*> polygons;
    d->createPolygonsFromPoints( points, pointCount, polygons, isGeoProjected );

    foreach( QPolygon* itPolygon, polygons ) {
        // Using QPainter instead of ClipPainter until some bugs are fixed.
        QPainter::drawPolyline( *itPolygon );
    }

    qDeleteAll( polygons );
}

void GeoPainter::drawPolygon ( const GeoDataPoint * points, int pointCount, 
			       Qt::FillRule fillRule, 
			       bool isGeoProjected )
{
    QVector<QPolygon*> polygons;
    d->createPolygonsFromPoints( points, pointCount, polygons, isGeoProjected );

    foreach( QPolygon* itPolygon, polygons ) {
        // Using QPainter instead of ClipPainter until some bugs are fixed.
        QPainter::drawPolygon( *itPolygon );
    }

    qDeleteAll( polygons );
}

void GeoPainter::drawRect ( const GeoDataPoint & centerPoint, int width, int height, bool isGeoProjected )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( centerPoint, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawRect( d->m_x[it] - ( width / 2 ), y - ( height / 2 ), width, height );
            }
        }
    }
}

void GeoPainter::drawRoundRect ( const GeoDataPoint &centerPoint, int width, int height, int xRnd, int yRnd, bool isGeoProjected )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( centerPoint, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawRoundRect( d->m_x[it] - ( width / 2 ), y - ( height / 2 ), width, height, xRnd, yRnd );
            }
        }
    }
}


    // Reenabling QPainter methods.

void GeoPainter::drawLine ( int x1, int y1, int x2, int y2 )
{
    QPainter::drawLine( x1, y1, x2, y2 );
}

void GeoPainter::drawText ( int x, int y, const QString &text )
{
    QPainter::drawText( x, y, text );
}

void GeoPainter::drawEllipse ( int x, int y, int width, int height )
{
    QPainter::drawEllipse( x, y, width, height);
}

void GeoPainter::drawEllipse ( const QRectF & rectangle )
{
    QPainter::drawEllipse( rectangle );
}

void GeoPainter::drawImage ( const QRect &target, const QImage &image, const QRect &source, Qt::ImageConversionFlags flags )
{
    QPainter::drawImage( target, image, source, flags );
}

void GeoPainter::drawPixmap( int x, int y, const QPixmap &pixmap )
{
    QPainter::drawPixmap( x, y, pixmap );
}

void GeoPainter::drawPixmap( const QPointF &point, const QPixmap & pixmap )
{
    QPainter::drawPixmap( point, pixmap );
}

void GeoPainter::drawPixmap( const QPoint &point, const QPixmap & pixmap )
{
    QPainter::drawPixmap( point, pixmap );
}

void GeoPainter::drawPoint ( int x, int y )
{
    QPainter::drawPoint( x, y );
}

void GeoPainter::drawPolyline( const QPolygonF &polyline )
{
    ClipPainter::drawPolyline( polyline );
}

void GeoPainter::drawPolygon( const QPolygonF &polygon )
{
    ClipPainter::drawPolygon( polygon );
}

void GeoPainter::drawRect ( const QRectF &rectangle )
{
    QPainter::drawRect( rectangle );
}

void GeoPainter::drawRect ( const QRect &rectangle )
{
    QPainter::drawRect( rectangle );
}

void GeoPainter::drawRect ( int x, int y, int width, int height )
{
    QPainter::drawRect( x, y, width, height);
}

void GeoPainter::drawText ( const QPoint & position, const QString & text )
{
    QPainter::drawText( position, text );
}
