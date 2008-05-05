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


#include "AbstractProjection.h"
#include "global.h"
#include "ViewportParams.h"

// #define MARBLE_DEBUG
using namespace Marble;

class GeoPainterPrivate
{
 public:
    GeoPainterPrivate( ViewportParams * viewport )
        : m_viewport( viewport )
    {
    }

    ViewportParams * m_viewport;
};

GeoPainter::GeoPainter( QPaintDevice* pd, ViewportParams * viewport, bool clip )
    : ClipPainter( pd, clip ),
      d( new GeoPainterPrivate( viewport ) )
{
}

void GeoPainter::autoMapQuality () 
{
    bool antialiased = false;

    if (   d->m_viewport->mapQuality() == Marble::High
        || d->m_viewport->mapQuality() == Marble::Print ) {
            antialiased = true;
    }

    setRenderHint( QPainter::Antialiasing, antialiased );
}

void GeoPainter::drawPoint (  const GeoDataPoint & position )
{
    int x, y;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    bool visible = projection->screenCoordinates( position, d->m_viewport, x, y );
    if ( visible ) {
        QPainter::drawPoint( x, y );
    }
}

void GeoPainter::drawPoints (  const GeoDataPoint * points, int pointCount )
{
    int x, y;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    GeoDataPoint * itPoint = const_cast<GeoDataPoint *>( points );
    while( itPoint < points + pointCount ) {
        bool visible = projection->screenCoordinates( *itPoint, d->m_viewport, x, y );

        if ( visible ) {
            QPainter::drawPoint( x, y );
        }
    }
}

void GeoPainter::drawText ( const GeoDataPoint & position, const QString & text )
{
    int x, y;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    bool visible = projection->screenCoordinates( position, d->m_viewport, x, y );
    if ( visible ) {
        QPainter::drawText( QPoint( x, y ), text );
    }
}

void GeoPainter::drawText ( const QPoint & position, const QString & text )
{
    QPainter::drawText( position, text );
}


void GeoPainter::drawEllipse ( const GeoDataPoint & point, int width, int height, bool isGeoProjected )
{
    int x, y;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( point, d->m_viewport, x, y );
        if ( visible ) {
            QPainter::drawEllipse(  x - width / 2, y - height / 2, width, height  );
        }
    }
}

void GeoPainter::drawPixmap ( const GeoDataPoint & point, const QPixmap & pixmap, bool isGeoProjected )
{
    int x, y;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( point, d->m_viewport, x, y );
        if ( visible ) {
            QPainter::drawPixmap( x - ( pixmap.width() / 2 ), y - ( pixmap.height() / 2 ), pixmap );
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
    int x, y;
    bool previousOcculted;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {

        QPolygon* polygon;
        QVector<QPolygon*> polygons;

        GeoDataPoint * itPoint = const_cast<GeoDataPoint *>( points );
        while( itPoint < points + pointCount ) {
            bool occulted;
            bool visible = projection->screenCoordinates( *itPoint, d->m_viewport, x, y, occulted );
            if ( itPoint == points ){
                polygon = new QPolygon;
                previousOcculted = occulted;
            }

            if ( occulted && !previousOcculted ) {
                polygons.append( polygon );
                polygon = new QPolygon;
            }

            if ( !occulted ) {
                 polygon->append( QPoint( x, y ) );
            }

            previousOcculted = occulted;
            ++itPoint;
        }

        if ( polygon->size() > 1 ){
            polygons.append( polygon );
        }

        foreach( QPolygon* itPolygon, polygons ) {
            // Using QPainter instead of ClipPainter until some bugs are fixed.
            QPainter::drawPolyline( *itPolygon );
        }

        qDeleteAll( polygons );
    }
}

void GeoPainter::drawRect ( const GeoDataPoint & point, int width, int height, bool isGeoProjected )
{
    int x, y;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( point, d->m_viewport, x, y );
        if ( visible ) {
            QPainter::drawRect( x - ( width / 2 ), y - ( height / 2 ), width, height );
        }
    }
}

void GeoPainter::drawRoundRect ( const GeoDataPoint & point, int width, int height, int xRnd, int yRnd, bool isGeoProjected )
{
    int x, y;
    AbstractProjection *projection = d->m_viewport->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( point, d->m_viewport, x, y );
        if ( visible ) {
            QPainter::drawRoundRect(  x - ( width / 2 ), y - ( height / 2 ), width, height, xRnd, yRnd );
        }
    }
}

void GeoPainter::drawLine ( int x1, int y1, int x2, int y2 )
{
    QPainter::drawLine( x1, y1, x2, y2 );
}

void GeoPainter::drawText ( int x, int y, const QString & text )
{
    QPainter::drawText( x, y, text );
}

void GeoPainter::drawEllipse ( int x, int y, int width, int height )
{
    QPainter::drawEllipse( x, y, width, height);
}

void GeoPainter::drawPixmap ( int x, int y, const QPixmap & pixmap )
{
    QPainter::drawPixmap( x, y, pixmap );
}

void GeoPainter::drawPolyline( const QPolygonF & polyline )
{
    ClipPainter::drawPolyline( polyline );
}

void GeoPainter::drawRect ( int x, int y, int width, int height )
{
    QPainter::drawRect( x, y, width, height);
}
