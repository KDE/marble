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
    qDeleteAll< QList<GeoGraphicsItem*> >(p()->m_decorations);
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
    foreach( GeoGraphicsItem* decoration, p()->m_decorations ) {
        decoration->setVisible( visible );
    }
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
    foreach( GeoGraphicsItem* decoration, p()->m_decorations ) {
        decoration->setLatLonAltBox( latLonAltBox );
    }
}

void GeoGraphicsItem::setStyle( const GeoDataStyle* style )
{
    p()->m_style = style;
    foreach( GeoGraphicsItem* decoration, p()->m_decorations ) {
        decoration->setStyle( style );
    }
}

void GeoGraphicsItem::setHighlightStyle( GeoDataStyle* highlightStyle)
{
    /**
     * Delete any previously set style
     * and assign the new style @highlightStyle
     */
    delete p()->m_highlightStyle;
    p()->m_highlightStyle = highlightStyle;
    foreach( GeoGraphicsItem* decoration, p()->m_decorations ) {
        decoration->setHighlightStyle( highlightStyle );
    }
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
    foreach( GeoGraphicsItem* decoration, p()->m_decorations ) {
        decoration->setHighlighted( highlight );
    }
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
    foreach( GeoGraphicsItem* decoration, p()->m_decorations ) {
        decoration->setMinZoomLevel( zoomLevel );
    }
}

const QList<GeoGraphicsItem*>& GeoGraphicsItem::decorations()
{
    if ( p()->m_decorations.isEmpty() ) {
        createDecorations();
    }

    return p()->m_decorations;
}

void GeoGraphicsItem::addDecoration(GeoGraphicsItem* decoration)
{
    if (decoration != nullptr) {
        decoration->p()->m_isDecoration = true;

        decoration->setLatLonAltBox(this->latLonAltBox());
        decoration->setFlags(this->flags());
        decoration->setHighlighted(this->isHighlighted());
        decoration->setStyle(this->style());
        decoration->setMinZoomLevel(this->minZoomLevel());
        decoration->setVisible(this->visible());

        p()->m_decorations.append(decoration);
    }
}

bool GeoGraphicsItem::isDecoration() const
{
    return p()->m_isDecoration;
}

void GeoGraphicsItem::createDecorations()
{
    return;
}
