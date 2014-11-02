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

#include "GeoDataPlacemark.h"

// Qt
#include "MarbleDebug.h"

#include <QColor>

using namespace Marble;

GeoGraphicsItem::GeoGraphicsItem( const GeoDataFeature *feature )
    : d( new GeoGraphicsItemPrivate( feature ) )
{
    setFlag( ItemIsVisible, true );
}

GeoGraphicsItem::~GeoGraphicsItem()
{
    delete p()->m_highlightStyle;
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

const GeoDataFeature* GeoGraphicsItem::feature() const
{
    return p()->m_feature;
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

void GeoGraphicsItem::setHighlightStyle( GeoDataStyle* highlightStyle)
{
    /**
     * Delete any previously set style
     * and assign the new style @highlightStyle
     */
    delete p()->m_highlightStyle;
    p()->m_highlightStyle = highlightStyle;
}

const GeoDataStyle* GeoGraphicsItem::style() const
{
    /**
     * m_isHighlight is set true when the item is
     * supposed to be colored highlighted
     */
    if ( p()->m_highlighted && p()->m_highlightStyle ) {
        return p()->m_highlightStyle;
    }
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

void GeoGraphicsItem::setHighlighted( bool highlight )
{
    p()->m_highlighted = highlight;
}

bool GeoGraphicsItem::isHighlighted() const
{
    return p()->m_highlighted;
}

GeoGraphicsItemPrivate *GeoGraphicsItem::p()
{
    return reinterpret_cast<GeoGraphicsItemPrivate *>( d );
}

const GeoGraphicsItemPrivate *GeoGraphicsItem::p() const
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

