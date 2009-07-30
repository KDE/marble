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
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>

using namespace Marble;

MarbleGraphicsItem::MarbleGraphicsItem( MarbleGraphicsItem *parent )
    : d( new MarbleGraphicsItemPrivate( this, parent ) )
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
    if ( !p()->m_visibility ) {
        return true;
    }

    p()->setProjection( viewport->currentProjection(), viewport );
    
    // Remove the pixmap if it has been requested. This prevents QPixmapCache from being used
    // outside the ui thread.
    if ( p()->m_removeCachedPixmap ) {
        p()->m_removeCachedPixmap = false;
        QPixmapCache::remove( p()->m_cacheKey );
    }
    
    if ( p()->positions().size() == 0 ) {
        return true;
    }
    
    bool successful = true;
    
    // At the moment, as GraphicsItems can't be zoomed or rotated ItemCoordinateCache
    // and DeviceCoordianteCache is exactly the same
    if ( ItemCoordinateCache == cacheMode()
         || DeviceCoordinateCache == cacheMode() )
    {
        p()->ensureValidCacheKey();
        QPixmap cachePixmap;
        bool pixmapAvailable = QPixmapCache::find( p()->m_cacheKey, cachePixmap );
        if ( !pixmapAvailable ) {
            QSize neededPixmapSize = size().toSize() + QSize( 1, 1 ); // adding a pixel for rounding errors
        
            if ( cachePixmap.size() != neededPixmapSize ) {
                if ( size().isValid() && !size().isNull() ) {
                    cachePixmap = QPixmap( neededPixmapSize ).copy();
                }
                else {
                    qDebug() << "Warning: Invalid pixmap size suggested: " << d->m_size;
                }
            }
        
            cachePixmap.fill( Qt::transparent );
            GeoPainter pixmapPainter( &( cachePixmap ), viewport, NormalQuality );
            // We paint in best quality here, as we only have to paint once.
            pixmapPainter.setRenderHint( QPainter::Antialiasing, true );
            // The cache image will get a 0.5 pixel bounding to save antialiasing effects.
            pixmapPainter.translate( 0.5, 0.5 );
            paint( &pixmapPainter, viewport, renderPos, layer );
            // Paint children
            if ( p()->m_children ) {
                foreach ( MarbleGraphicsItem *item, *p()->m_children ) {
                    item->paintEvent( &pixmapPainter, viewport, renderPos, layer );
                }
            }
            // Update the pixmap in cache
            QPixmapCache::insert( p()->m_cacheKey, cachePixmap );
        }
        
        foreach( const QPointF& position, p()->positions() ) {
            painter->save();
            
            painter->drawPixmap( position, cachePixmap );
            
            painter->restore();
        }
    }
    else {
        foreach( const QPointF& position, p()->positions() ) {
            painter->save();

            painter->translate( position );
            paint( painter, viewport, renderPos, layer );
            // Paint children
            if ( p()->m_children ) {
                foreach ( MarbleGraphicsItem *item, *p()->m_children ) {
                    item->paintEvent( painter, viewport, renderPos, layer );
                }
            }

            painter->restore();
        }
    }
    
    return successful;
}

bool MarbleGraphicsItem::contains( const QPointF& point ) const {
    foreach( const QRectF& rect, d->boundingRects() ) {
        if( rect.contains( point ) )
            return true;
    }
    return false;
}

QSizeF MarbleGraphicsItem::size() const {
    return p()->m_size;
}

MarbleGraphicsItem::CacheMode MarbleGraphicsItem::cacheMode() const {
    return p()->m_cacheMode;
}

void MarbleGraphicsItem::setCacheMode( CacheMode mode, const QSize & logicalCacheSize ) {
    p()->m_cacheMode = mode;
    p()->m_logicalCacheSize = logicalCacheSize;
    if ( p()->m_cacheMode == NoCache ) {
        p()->m_removeCachedPixmap = true;
    }
}

void MarbleGraphicsItem::update() {
    p()->m_removeCachedPixmap = true;
    // Update the parent.
    if ( p()->m_parent ) {
        p()->m_parent->update();
    }
}

bool MarbleGraphicsItem::visible() const {
    return p()->m_visibility;
}

void MarbleGraphicsItem::setVisible( bool visible ) {
    p()->m_visibility = visible;
}

void MarbleGraphicsItem::hide() {
    setVisible( false );
}

void MarbleGraphicsItem::show() {
    setVisible( true );
}

void MarbleGraphicsItem::setSize( const QSizeF& size ) {
    p()->m_size = size;
    update();
}

bool MarbleGraphicsItem::eventFilter( QObject *object, QEvent *e ) {
    Q_UNUSED( object );
    Q_UNUSED( e );
    return false;
}

MarbleGraphicsItemPrivate *MarbleGraphicsItem::p() const {
    return d;
}
