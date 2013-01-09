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

// Qt
#include "MarbleDebug.h"

using namespace Marble;

GeoGraphicsItem::GeoGraphicsItem()
    : d( new GeoGraphicsItemPrivate )
{
    setFlag( ItemIsVisible, true );
}

GeoGraphicsItem::~GeoGraphicsItem()
{
    delete d;
}

bool GeoGraphicsItem::visible() const
{
    return d->m_flags & ItemIsVisible;
}

void GeoGraphicsItem::setVisible( bool visible )
{
    setFlag( ItemIsVisible, visible );
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

const GeoDataLatLonAltBox& GeoGraphicsItem::latLonAltBox() const
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

