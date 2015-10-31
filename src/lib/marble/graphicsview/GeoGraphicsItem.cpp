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
    qDeleteAll< QList<GeoGraphicsItem*> >(d->m_decorations);
    delete d;
}

bool GeoGraphicsItem::visible() const
{
    return d->m_flags & ItemIsVisible;
}

void GeoGraphicsItem::setVisible( bool visible )
{
    setFlag( ItemIsVisible, visible );
    foreach( GeoGraphicsItem* decoration, d->m_decorations ) {
        decoration->setVisible( visible );
    }
}

GeoGraphicsItem::GeoGraphicsItemFlags GeoGraphicsItem::flags() const
{
    return d->m_flags;
}

void GeoGraphicsItem::setFlag( GeoGraphicsItemFlag flag, bool enabled )
{
    if( enabled ) {
        d->m_flags = d->m_flags | flag;
    } else {
        d->m_flags = d->m_flags & ~flag;
    }
}

void GeoGraphicsItem::setFlags( GeoGraphicsItemFlags flags )
{
    d->m_flags = flags;
}

const GeoDataFeature* GeoGraphicsItem::feature() const
{
    return d->m_feature;
}

const GeoDataLatLonAltBox& GeoGraphicsItem::latLonAltBox() const
{
    return d->m_latLonAltBox;
}

void GeoGraphicsItem::setLatLonAltBox( const GeoDataLatLonAltBox& latLonAltBox )
{
    d->m_latLonAltBox = latLonAltBox;
    foreach( GeoGraphicsItem* decoration, d->m_decorations ) {
        decoration->setLatLonAltBox( latLonAltBox );
    }
}

void GeoGraphicsItem::setStyle( const GeoDataStyle::ConstPtr &style )
{
    d->m_style = style;
    foreach( GeoGraphicsItem* decoration, d->m_decorations ) {
        decoration->setStyle( style );
    }
}

void GeoGraphicsItem::setHighlightStyle( const GeoDataStyle::ConstPtr &highlightStyle)
{
    /**
     * Delete any previously set style
     * and assign the new style @highlightStyle
     */
    d->m_highlightStyle = highlightStyle;
    foreach( GeoGraphicsItem* decoration, d->m_decorations ) {
        decoration->setHighlightStyle( highlightStyle );
    }
}

GeoDataStyle::ConstPtr GeoGraphicsItem::style() const
{
    /**
     * m_isHighlight is set true when the item is
     * supposed to be colored highlighted
     */
    if ( d->m_highlighted && d->m_highlightStyle ) {
        return d->m_highlightStyle;
    }
    return d->m_style;
}

qreal GeoGraphicsItem::zValue() const
{
    return d->m_zValue;
}

void GeoGraphicsItem::setZValue( qreal z )
{
    d->m_zValue = z;
}

void GeoGraphicsItem::setHighlighted( bool highlight )
{
    d->m_highlighted = highlight;
    foreach( GeoGraphicsItem* decoration, d->m_decorations ) {
        decoration->setHighlighted( highlight );
    }
}

bool GeoGraphicsItem::isHighlighted() const
{
    return d->m_highlighted;
}

int GeoGraphicsItem::minZoomLevel() const
{
    return d->m_minZoomLevel;
}

void GeoGraphicsItem::setMinZoomLevel(int zoomLevel)
{
    d->m_minZoomLevel = zoomLevel;
    foreach( GeoGraphicsItem* decoration, d->m_decorations ) {
        decoration->setMinZoomLevel( zoomLevel );
    }
}

const QList<GeoGraphicsItem*>& GeoGraphicsItem::decorations()
{
    if ( d->m_decorations.isEmpty() ) {
        createDecorations();
    }

    return d->m_decorations;
}

void GeoGraphicsItem::addDecoration(GeoGraphicsItem* decoration)
{
    if (decoration != nullptr) {
        decoration->d->m_isDecoration = true;

        decoration->setLatLonAltBox(this->latLonAltBox());
        decoration->setFlags(this->flags());
        decoration->setHighlighted(this->isHighlighted());
        decoration->setStyle(this->style());
        decoration->setMinZoomLevel(this->minZoomLevel());
        decoration->setVisible(this->visible());

        d->m_decorations.append(decoration);
    }
}

bool GeoGraphicsItem::isDecoration() const
{
    return d->m_isDecoration;
}

bool GeoGraphicsItem::zValueLessThan(GeoGraphicsItem *one, GeoGraphicsItem *two)
{
    return one->d->m_zValue < two->d->m_zValue;
}

void GeoGraphicsItem::createDecorations()
{
    return;
}
