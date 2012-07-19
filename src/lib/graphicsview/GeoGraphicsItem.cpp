//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "GeoGraphicsItem.h"
#include "GeoGraphicsItem_p.h"

//Marble
#include "GeoDataCoordinates.h"

// Qt
#include "MarbleDebug.h"

using namespace Marble;

GeoGraphicsItem::GeoGraphicsItem()
    : MarbleGraphicsItem( new GeoGraphicsItemPrivate( this ) )
{
    setFlag( ItemIsVisible, true );
}

GeoGraphicsItem::GeoGraphicsItem( GeoGraphicsItemPrivate *d_ptr )
    : MarbleGraphicsItem( d_ptr )
{
    setFlag( ItemIsVisible, true );
}

void GeoGraphicsItem::setProjection(ViewportParams *viewport, GeoPainter *painter)
{
    p()->setProjection( viewport, painter );
}

GeoGraphicsItem::~GeoGraphicsItem()
{
}

GeoDataCoordinates GeoGraphicsItem::coordinate() const
{
    return p()->m_coordinate;
}

void GeoGraphicsItem::setCoordinate( const GeoDataCoordinates &point )
{
    p()->m_coordinate = point;
}

GeoGraphicsItem::GeoGraphicsItemFlags GeoGraphicsItem::flags() const
{
    return p()->m_flags;
}

void GeoGraphicsItem::setFlag( GeoGraphicsItemFlag flag, bool enabled )
{
    if( enabled ) {
        p()->m_flags = p()->m_flags | flag;
    } else {
        p()->m_flags = p()->m_flags & ~flag;
    }
}

void GeoGraphicsItem::setFlags( GeoGraphicsItemFlags flags )
{
    p()->m_flags = flags;
}

int GeoGraphicsItem::minLodPixels() const
{
    return p()->m_minLodPixels;
}

void GeoGraphicsItem::setMinLodPixels( int pixels )
{
    p()->m_minLodPixels = pixels;
}

int GeoGraphicsItem::maxLodPixels() const
{
    return p()->m_maxLodPixels;
}

void GeoGraphicsItem::setMaxLodPixels( int pixels )
{
    p()->m_maxLodPixels = pixels;
}

GeoDataLatLonAltBox& GeoGraphicsItem::latLonAltBox() const
{
    return p()->m_latLonAltBox;
}

void GeoGraphicsItem::setLatLonAltBox( const GeoDataLatLonAltBox& latLonAltBox )
{
    p()->m_latLonAltBox = latLonAltBox;
}

void GeoGraphicsItem::setStyle( const GeoDataStyle* style )
{
    p()->m_style = style;
}

const GeoDataStyle* GeoGraphicsItem::style() const
{
    return p()->m_style;
}

qreal GeoGraphicsItem::zValue() const
{
    return p()->m_zValue;
}

void GeoGraphicsItem::setZValue( qreal z )
{
    p()->m_zValue = z;
    update();
}

QList<QPointF> GeoGraphicsItem::positions() const
{
    return p()->positions();
}

GeoGraphicsItemPrivate *GeoGraphicsItem::p() const
{
    return reinterpret_cast<GeoGraphicsItemPrivate *>( d );
}

int GeoGraphicsItem::minZoomLevel() const
{
    return p()->m_minZoomLevel;
}

void GeoGraphicsItem::setMinZoomLevel(int zoomLevel)
{
    p()->m_minZoomLevel = zoomLevel;
}

