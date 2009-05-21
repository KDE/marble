//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#include "MarbleGraphicsItem.h"

#include "MarbleGraphicsItem_p.h"

//Marble
#include "GeoPainter.h"
#include "ViewportParams.h"

// Qt
#include <QtCore/QDebug>

using namespace Marble;

MarbleGraphicsItem::MarbleGraphicsItem()
    : d( new MarbleGraphicsItemPrivate )
{
}

MarbleGraphicsItem::MarbleGraphicsItem( MarbleGraphicsItemPrivate *d_ptr )
    : d( d_ptr )
{
}
    
MarbleGraphicsItem::~MarbleGraphicsItem() {
    delete d;
}

bool MarbleGraphicsItem::paintEvent( GeoPainter *painter, ViewportParams *viewport, 
                 const QString& renderPos, GeoSceneLayer *layer )
{
    p()->setProjection( viewport->currentProjection(), viewport );
    bool successful = true;
    
    foreach( QPoint position, p()->positions() ) {
        painter->save();
        
        painter->translate( position );
        if( !paint( painter, viewport, renderPos, layer ) ) {
            successful = false;
        }
        
        painter->restore();
    }
    
    return successful;
}

bool MarbleGraphicsItem::contains( const QPoint& point ) const {
    foreach( QRect rect, d->boundingRects() ) {
        if( rect.contains( point ) )
            return true;
    }
    return false;
}

QSize MarbleGraphicsItem::size() const {
    return p()->m_size;
}

MarbleGraphicsItem::CacheMode MarbleGraphicsItem::cacheMode() const {
    return p()->m_cacheMode;
}

void MarbleGraphicsItem::setCacheMode( CacheMode mode, const QSize & logicalCacheSize ) {
    p()->m_cacheMode = mode;
    p()->m_logicalCacheSize = logicalCacheSize;
}

void MarbleGraphicsItem::setSize( const QSize& size ) {
    p()->m_size = size;
}

bool MarbleGraphicsItem::eventFilter( QObject *object, QEvent *e ) {
    return false;
}

MarbleGraphicsItemPrivate *MarbleGraphicsItem::p() const {
    return d;
}
