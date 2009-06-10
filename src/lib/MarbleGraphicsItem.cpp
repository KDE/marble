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
    if ( p()->positions().size() == 0 ) {
        return true;
    }
    
    bool successful = true;
    
    // At the moment, as GraphicsItems can't be zoomed or rotated ItemCoordinateCache
    // and DeviceCoordianteCache is exactly the same
    if ( ItemCoordinateCache == cacheMode()
         || DeviceCoordinateCache == cacheMode() )
    {
        if ( needsUpdate() ) {
            p()->m_needsUpdate = false;
            QSize neededPixmapSize = size() + QSize( 1, 1 ); // adding a pixel for rounding errors
        
            if ( p()->m_cachePixmap.size() != neededPixmapSize ) {
                if ( size().isValid() && !size().isNull() ) {
                    p()->m_cachePixmap = QPixmap( neededPixmapSize ).copy();
                }
                else {
                    qDebug() << "Warning: Invalid pixmap size suggested: " << d->m_size;
                }
            }
        
            p()->m_cachePixmap.fill( Qt::transparent );
            GeoPainter pixmapPainter( &( p()->m_cachePixmap ), viewport, Normal );
            // We paint in best quality here, as we only have to paint once.
            pixmapPainter.setRenderHint( QPainter::Antialiasing, true );
            // The cache image will get a 0.5 pixel bounding to save antialiasing effects.
            pixmapPainter.translate( 0.5, 0.5 );
            paint( &pixmapPainter, viewport, renderPos, layer );
        }
        
        foreach( QPoint position, p()->positions() ) {
            painter->save();
            
            painter->drawPixmap( position, d->m_cachePixmap );
            
            painter->restore();
        }
    }
    else {
        foreach( QPoint position, p()->positions() ) {
            painter->save();
        
            painter->translate( position );
            paint( painter, viewport, renderPos, layer );
        
            painter->restore();
        }
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

void MarbleGraphicsItem::update() {
    p()->m_needsUpdate = true;
}

bool MarbleGraphicsItem::needsUpdate() {
    return p()->m_needsUpdate;
}

void MarbleGraphicsItem::setSize( const QSize& size ) {
    p()->m_size = size;
    update();
}

bool MarbleGraphicsItem::eventFilter( QObject *object, QEvent *e ) {
    return false;
}

MarbleGraphicsItemPrivate *MarbleGraphicsItem::p() const {
    return d;
}
