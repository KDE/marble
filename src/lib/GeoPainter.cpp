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
#include "ViewportParams.h"

// #define MARBLE_DEBUG

class GeoPainterPrivate
{
 public:
    GeoPainterPrivate( ViewportParams * params )
        : m_params( params )
    {
    }

    ViewportParams * m_params;
};

GeoPainter::GeoPainter( QPaintDevice* pd, ViewportParams * params, bool clip )
    : ClipPainter( pd, clip ),
      d( new GeoPainterPrivate( params ) )
{
}

void GeoPainter::drawPoint (  const GeoDataPoint & position )
{
    int x, y;
    AbstractProjection *projection = d->m_params->currentProjection();

    bool visible = projection->screenCoordinates( position, d->m_params, x, y );
    if ( visible ) {
        QPainter::drawPoint( x, y );
    }
}

void GeoPainter::drawPoints (  const GeoDataPoint * points, int pointCount )
{
}

void GeoPainter::drawText ( const GeoDataPoint & position, const QString & text )
{
    int x, y;
    AbstractProjection *projection = d->m_params->currentProjection();

    bool visible = projection->screenCoordinates( position, d->m_params, x, y );
    if ( visible ) {
        QPainter::drawText( QPoint( x, y ), text );
    }
}

void GeoPainter::drawText ( int x, int y, const QString & text )
{
    QPainter::drawText( x, y, text );
}

void GeoPainter::drawText ( const QPoint & position, const QString & text )
{
    QPainter::drawText( position, text );
}


void GeoPainter::drawEllipse ( const GeoDataPoint & point, int width, int height, bool isGeoProjected )
{
    int x, y;
    AbstractProjection *projection = d->m_params->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( point, d->m_params, x, y );
        if ( visible ) {
            QPainter::drawEllipse(  x - width / 2, y - height / 2, width, height  );
        }
    }
}

void GeoPainter::drawEllipse ( int x, int y, int width, int height )
{
    QPainter::drawEllipse( x, y, width, height);
}

void GeoPainter::drawPixmap ( const GeoDataPoint & point, const QPixmap & pixmap, bool isGeoProjected )
{
    int x, y;
    AbstractProjection *projection = d->m_params->currentProjection();

    if ( isGeoProjected == false ) {
        // FIXME: Better visibility detection that takes the circle geometry into account
        bool visible = projection->screenCoordinates( point, d->m_params, x, y );
        if ( visible ) {
            QPainter::drawPixmap( x - ( pixmap.width() / 2 ), y - ( pixmap.height() / 2 ), pixmap );
        }
    }
}

void GeoPainter::drawPixmap ( int x, int y, const QPixmap & pixmap )
{
    QPainter::drawPixmap( x, y, pixmap );
}
