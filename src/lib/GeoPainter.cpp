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

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"

#include "global.h"
#include "ViewportParams.h"

// #define MARBLE_DEBUG

namespace Marble
{

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

    void createPolygonsFromLineString( const GeoDataLineString & lineString, 
                   QVector<QPolygonF *> &polygons )
    {
        AbstractProjection *projection = m_viewport->currentProjection();

        projection->screenCoordinates( lineString, m_viewport, polygons );
    }


    void createPolygonsFromLinearRing( const GeoDataLinearRing & linearRing, 
                   QVector<QPolygonF *> &polygons )
    {
        AbstractProjection *projection = m_viewport->currentProjection();

        projection->screenCoordinates( linearRing, m_viewport, polygons );
    }


    void createAnnotationLayout (  int x, int y, QSize bubbleSize, int bubbleOffsetX, int bubbleOffsetY, int xRnd, int yRnd, QPainterPath& path, QRectF& rect )
    {
        // TODO: MOVE this into an own Annotation class
        qreal arrowPosition = 0.3;
        qreal arrowWidth = 12.0;

        qreal width =  (qreal)( bubbleSize.width() );
        qreal height = (qreal)( bubbleSize.height() );

        qreal dx = ( bubbleOffsetX > 0 ) ? 1.0 : -1.0; // x-Mirror
        qreal dy = ( bubbleOffsetY < 0 ) ? 1.0 : -1.0; // y-Mirror

        qreal x0 = (qreal) ( x + bubbleOffsetX ) - dx * ( 1.0 - arrowPosition ) * ( width - 2.0 * xRnd ) - xRnd *dx;
        qreal x1 = (qreal) ( x + bubbleOffsetX ) - dx * ( 1.0 - arrowPosition ) * ( width - 2.0 * xRnd );
        qreal x2 = (qreal) ( x + bubbleOffsetX ) - dx * ( 1.0 - arrowPosition ) * ( width - 2.0 * xRnd ) + xRnd * dx;
        qreal x3 = (qreal) ( x + bubbleOffsetX ) - dx * arrowWidth / 2.0;
        qreal x4 = (qreal) ( x + bubbleOffsetX ) + dx * arrowWidth / 2.0;
        qreal x5 = (qreal) ( x + bubbleOffsetX ) + dx * arrowPosition * ( width - 2.0 * xRnd )- xRnd * dx;
        qreal x6 = (qreal) ( x + bubbleOffsetX ) + dx * arrowPosition * ( width - 2.0 * xRnd );
        qreal x7 = (qreal) ( x + bubbleOffsetX ) + dx * arrowPosition * ( width - 2.0 * xRnd ) + xRnd * dx;

        qreal y0 = (qreal) ( y + bubbleOffsetY );
        qreal y1 = (qreal) ( y + bubbleOffsetY ) - dy * yRnd;
        qreal y2 = (qreal) ( y + bubbleOffsetY ) - dy * 2 * yRnd;
        qreal y5 = (qreal) ( y + bubbleOffsetY ) - dy * ( height - 2 * yRnd );
        qreal y6 = (qreal) ( y + bubbleOffsetY ) - dy * ( height - yRnd );
        qreal y7 = (qreal) ( y + bubbleOffsetY ) - dy * height;

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

        qreal left   = ( dx > 0 ) ? x1 : x6;
        qreal right  = ( dx > 0 ) ? x6 : x1;
        qreal top    = ( dy > 0 ) ? y6 : y1;
        qreal bottom = ( dy > 0 ) ? y1 : y6;

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

void GeoPainter::drawAnnotation (  const GeoDataCoordinates & position, const QString & text, QSize bubbleSize, int bubbleOffsetX, int bubbleOffsetY, int xRnd, int yRnd )
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

void GeoPainter::drawPoint (  const GeoDataCoordinates & position )
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

void GeoPainter::drawPoints (  const GeoDataCoordinates * points, int pointCount )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    const GeoDataCoordinates * itPoint = points;
    while( itPoint < points + pointCount ) {
        bool visible = projection->screenCoordinates( *itPoint, d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );
    
        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawPoint( d->m_x[it], y );
            }
        }

        ++itPoint;
    }
}

void GeoPainter::drawText ( const GeoDataCoordinates & position, const QString & text )
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

void GeoPainter::drawEllipse ( const GeoDataCoordinates & centerPoint, qreal width, qreal height, bool isGeoProjected )
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
                QPainter::drawEllipse(  d->m_x[it] - width / 2.0, y - height / 2.0, width, height  );
            }
        }
    }
    else {
        qreal centerLon = 0.0;
        qreal centerLat = 0.0;
        qreal altitude = centerPoint.altitude();
        centerPoint.geoCoordinates( centerLon, centerLat, GeoDataCoordinates::Degree );

        GeoDataLinearRing ellipse;

        qreal lon = 0.0;
        qreal lat = 0.0;

        // TODO: Optimize the precision by determining the size which the 
        //       ellipse covers on the screen.
        int precision = 100;

        for ( int i = 0; i <= precision; ++i ) {
            qreal t = 1.0 - 2.0 * (qreal)(i) / (qreal)(precision);
            lat = centerLat + 0.5 * height * sqrt( 1.0 - t * t );
            lon = centerLon + 0.5 * width * t;
            ellipse << new GeoDataCoordinates( lon, lat, altitude, GeoDataCoordinates::Degree );
        }
        for ( int i = 0; i <= precision; ++i ) {
            qreal t = 2.0 * (qreal)(i) / (qreal)(precision) -  1.0;
            lat = centerLat - 0.5 * height * sqrt( 1.0 - t * t );
            lon = centerLon + 0.5 * width * t;
            ellipse << new GeoDataCoordinates( lon, lat, altitude, GeoDataCoordinates::Degree );
        }

        drawPolygon( ellipse );
    }

}

void GeoPainter::drawImage ( const GeoDataCoordinates & centerPoint, const QImage & image, bool isGeoProjected )
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

void GeoPainter::drawPixmap ( const GeoDataCoordinates & centerPoint, const QPixmap & pixmap, bool isGeoProjected )
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

void GeoPainter::drawLine (  const GeoDataCoordinates & p1,  const GeoDataCoordinates & p2, bool isGeoProjected )
{
    GeoDataLineString line;
    line.setTessellate( isGeoProjected );

    GeoDataCoordinates c1 ( p1 );
    GeoDataCoordinates c2 ( p2 );

    line << &c1 << &c2;

    drawPolyline( line ); 
}

void GeoPainter::drawPolyline ( const GeoDataLineString & lineString )
{
    // If the object is not visible in the viewport return 
    // FIXME: For better testing we are using "contains" instead of "intersects" right now
    if ( ! d->m_viewport->viewLatLonAltBox().contains( lineString.latLonAltBox() ) )
    {
        qDebug() << "LineString doesn't get displayed on the viewport";
        return;
    }
    qDebug() << "Drawing LineString";

    QVector<QPolygonF*> polygons;
    d->createPolygonsFromLineString( lineString, polygons );

    foreach( QPolygonF* itPolygon, polygons ) {
        // Using QPainter instead of ClipPainter until some bugs are fixed.
        QPainter::drawPolyline( *itPolygon );
    }

    qDeleteAll( polygons );
}

void GeoPainter::drawPolygon ( const GeoDataLinearRing & linearRing, Qt::FillRule fillRule )
{
    // If the object is not visible in the viewport return 
    // FIXME: For better testing we are using "contains" instead of "intersects" right now
    if ( ! d->m_viewport->viewLatLonAltBox().contains( linearRing.latLonAltBox() ) )
    {
        qDebug() << "Polygon doesn't get displayed on the viewport";
        return;
    }

    qDebug() << "Drawing Polygon";

    QVector<QPolygonF*> polygons;
    d->createPolygonsFromLinearRing( linearRing, polygons );

    foreach( QPolygonF* itPolygon, polygons ) {
        // Using QPainter instead of ClipPainter until some bugs are fixed.
        QPainter::drawPolygon( *itPolygon );
    }

    qDeleteAll( polygons );
}

void GeoPainter::drawPolygon ( const GeoDataPolygon & polygon, Qt::FillRule fillRule )
{
    // If the object is not visible in the viewport return 
    // FIXME: For better testing we are using "contains" instead of "intersects" right now
    if ( ! d->m_viewport->viewLatLonAltBox().contains( polygon.outerBoundary().latLonAltBox() ) )
    {
        qDebug() << "Polygon doesn't get displayed on the viewport";
        return;
    }
    qDebug() << "Drawing Polygon";

    // Creating the outer screen polygons first
    QVector<QPolygonF*> outerPolygons;
    d->createPolygonsFromLinearRing( polygon.outerBoundary(), outerPolygons );

    // Now creating the "holes" by cutting away the inner boundaries:

    // In QPathClipper We Trust ...
    // ... and in the speed of a threesome of nested foreachs!

    QVector<GeoDataLinearRing *> innerBoundaries = polygon.innerBoundaries(); 
    foreach( GeoDataLinearRing *itInnerBoundary, innerBoundaries ) {
        QVector<QPolygonF*> innerPolygons;
        d->createPolygonsFromLinearRing( *itInnerBoundary, innerPolygons );
        
        foreach( QPolygonF* itOuterPolygon, outerPolygons ) {
            foreach( QPolygonF* itInnerPolygon, innerPolygons ) {
                *itOuterPolygon = itOuterPolygon->subtracted( *itInnerPolygon );
            }
        }
        qDeleteAll( innerPolygons );    
    }

    foreach( QPolygonF* itOuterPolygon, outerPolygons ) {
        // Using QPainter instead of ClipPainter until some bugs are fixed.
        QPainter::drawPolygon( *itOuterPolygon );
    }

    qDeleteAll( outerPolygons );    
}

void GeoPainter::drawRect ( const GeoDataCoordinates & centerCoordinates, qreal width, qreal height, bool isGeoProjected )
{
    int pointRepeatNum;
    int y;
    bool globeHidesPoint;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( centerCoordinates, 
                       d->m_viewport, d->m_x, y, pointRepeatNum, globeHidesPoint );

        if ( visible ) {
            // Draw all the x-repeat-instances of the point on the screen
            for( int it = 0; it < pointRepeatNum; ++it ) {
                QPainter::drawRect( d->m_x[it] - ( width / 2.0 ), y - ( height / 2.0 ), width, height );
            }
        }
    }
    else {
        qreal lon = 0.0;
        qreal lat = 0.0;
        qreal altitude = centerCoordinates.altitude();
        centerCoordinates.geoCoordinates( lon, lat, GeoDataCoordinates::Degree );

        if ( width > 180.0 ) width = 180.0;
        qreal lonLeft = lon - width * 0.5;
        qreal lonRight =  lon + width * 0.5;

        qreal latTop = lat + height * 0.5;
        if ( latTop > +90.0 ) latTop = 90.0;
        if ( latTop < -90.0 ) latTop = -90.0;

        qreal latBottom = lat - height * 0.5;
        if ( latBottom > +90.0 ) latBottom = 90.0;
        if ( latBottom < -90.0 ) latBottom = -90.0;

        GeoDataCoordinates bottomLeft( lonLeft, latBottom,
                                    altitude, GeoDataCoordinates::Degree );;
        GeoDataCoordinates bottomRight( lonRight, latBottom,
                                    altitude, GeoDataCoordinates::Degree );
        GeoDataCoordinates topRight( lonRight, latTop,
                                    altitude, GeoDataCoordinates::Degree );
        GeoDataCoordinates topLeft( lonLeft, latTop,
                                    altitude, GeoDataCoordinates::Degree );

        GeoDataLinearRing rectangle( 0, Tessellate | RespectLatitudeCircle );

        rectangle << &bottomLeft << &bottomRight << &topRight << &topLeft;

        drawPolygon( rectangle, Qt::OddEvenFill ); 
    }
}

void GeoPainter::drawRoundRect ( const GeoDataCoordinates &centerPoint, int width, int height, int xRnd, int yRnd, bool isGeoProjected )
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

}
