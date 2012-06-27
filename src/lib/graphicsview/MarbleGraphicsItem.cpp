//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009-2010 Bastian Holst <bastianholst@gmx.de>
//

#include "MarbleGraphicsItem.h"

#include "MarbleGraphicsItem_p.h"

// Marble
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "ViewportParams.h"

// Qt
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>
#include <QtGui/QMouseEvent>

using namespace Marble;

MarbleGraphicsItem::MarbleGraphicsItem( MarbleGraphicsItem *parent )
    : d( new MarbleGraphicsItemPrivate( this, parent ) )
{
}

MarbleGraphicsItem::MarbleGraphicsItem( MarbleGraphicsItemPrivate *d_ptr )
    : d( d_ptr )
{
}
    
MarbleGraphicsItem::~MarbleGraphicsItem()
{
    delete d;
}

bool MarbleGraphicsItem::paintEvent( GeoPainter *painter, ViewportParams *viewport, 
                                     const QString& renderPos, GeoSceneLayer *layer )
{
    if ( !p()->m_visibility ) {
        return true;
    }

    p()->updateLabelPositions();

    p()->setProjection( viewport, painter );
    
    // Remove the pixmap if it has been requested. This prevents QPixmapCache from being used
    // outside the ui thread.
    if ( p()->m_removeCachedPixmap ) {
        p()->m_removeCachedPixmap = false;
        QPixmapCache::remove( p()->m_cacheKey );
    }
    
    if ( p()->positions().size() == 0 ) {
        return true;
    }

    // At the moment, as GraphicsItems can't be zoomed or rotated ItemCoordinateCache
    // and DeviceCoordianteCache is exactly the same
    if ( ItemCoordinateCache == cacheMode()
         || DeviceCoordinateCache == cacheMode() )
    {
        p()->ensureValidCacheKey();
        QPixmap cachePixmap;
#if QT_VERSION < 0x040600
        bool pixmapAvailable = QPixmapCache::find( p()->m_cacheKey, cachePixmap );
#else
        bool pixmapAvailable = QPixmapCache::find( p()->m_cacheKey, &cachePixmap );
#endif
        if ( !pixmapAvailable ) {
            QSize neededPixmapSize = size().toSize() + QSize( 1, 1 ); // adding a pixel for rounding errors
        
            if ( cachePixmap.size() != neededPixmapSize ) {
                if ( size().isValid() && !size().isNull() ) {
                    cachePixmap = QPixmap( neededPixmapSize ).copy();
                }
                else {
                    mDebug() << "Warning: Invalid pixmap size suggested: " << d->m_size;
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
            foreach ( MarbleGraphicsItem *item, p()->m_children ) {
                item->paintEvent( &pixmapPainter, viewport, renderPos, layer );
            }
            // Update the pixmap in cache
#if QT_VERSION < 0x040600
            QPixmapCache::insert( p()->m_cacheKey, cachePixmap );
#else
            p()->m_cacheKey = QPixmapCache::insert( cachePixmap );
#endif
        }
        
        foreach( const QPointF& position, p()->positions() ) {
            painter->drawPixmap( position, cachePixmap );
        }
    }
    else {
        foreach( const QPointF& position, p()->positions() ) {
            painter->save();

            painter->translate( position );
            paint( painter, viewport, renderPos, layer );

            // Paint children
            foreach ( MarbleGraphicsItem *item, p()->m_children ) {
                item->paintEvent( painter, viewport, renderPos, layer );
            }

            painter->restore();
        }
    }
    
    return true;
}

bool MarbleGraphicsItem::contains( const QPointF& point ) const
{
    foreach( const QRectF& rect, d->boundingRects() ) {
        if( rect.contains( point ) )
            return true;
    }
    return false;
}

QRectF MarbleGraphicsItem::containsRect( const QPointF& point ) const
{
    foreach( const QRectF& rect, d->boundingRects() ) {
        if( rect.contains( point ) )
            return rect;
    }
    return QRectF();
}

QList<QRectF> MarbleGraphicsItem::boundingRects() const
{
    return p()->boundingRects();
}

QSizeF MarbleGraphicsItem::size() const
{
    return p()->m_size;
}

AbstractMarbleGraphicsLayout *MarbleGraphicsItem::layout() const
{
    return p()->m_layout;
}

void MarbleGraphicsItem::setLayout( AbstractMarbleGraphicsLayout *layout )
{
    // Deleting the old layout
    delete p()->m_layout;
    p()->m_layout = layout;
    update();
}

MarbleGraphicsItem::CacheMode MarbleGraphicsItem::cacheMode() const
{
    return p()->m_cacheMode;
}

void MarbleGraphicsItem::setCacheMode( CacheMode mode )
{
    p()->m_cacheMode = mode;
    if ( p()->m_cacheMode == NoCache ) {
        p()->m_removeCachedPixmap = true;
    }
}

void MarbleGraphicsItem::update()
{
    p()->m_removeCachedPixmap = true;

    // Update the parent.
    if ( p()->m_parent ) {
        p()->m_parent->update();
    }
}

bool MarbleGraphicsItem::visible() const
{
    return p()->m_visibility;
}

void MarbleGraphicsItem::setVisible( bool visible )
{
    p()->m_visibility = visible;
}

void MarbleGraphicsItem::hide()
{
    setVisible( false );
}

void MarbleGraphicsItem::show()
{
    setVisible( true );
}

void MarbleGraphicsItem::setSize( const QSizeF& size )
{
    p()->m_size = size;
    update();

    foreach ( MarbleGraphicsItem *item, p()->m_children ) {
        item->p()->setParentSize( size );
    }
}

QSizeF MarbleGraphicsItem::contentSize() const
{
    return size();
}

void MarbleGraphicsItem::setContentSize( const QSizeF& size )
{
    setSize( size );
}

QRectF MarbleGraphicsItem::contentRect() const
{
    return QRectF( QPointF( 0, 0 ), contentSize() );
}

QString MarbleGraphicsItem::toolTip() const
{
    return p()->m_toolTip;
}

void MarbleGraphicsItem::setToolTip( const QString& toolTip )
{
    p()->m_toolTip = toolTip;
}

void MarbleGraphicsItem::paint( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );
    Q_UNUSED( painter );
}

bool MarbleGraphicsItem::eventFilter( QObject *object, QEvent *e )
{
    if ( ! ( e->type() == QEvent::MouseButtonDblClick
             || e->type() == QEvent::MouseMove
             || e->type() == QEvent::MouseButtonPress
             || e->type() == QEvent::MouseButtonRelease ) )
    {
        return false;
    }
    
    QMouseEvent *event = static_cast<QMouseEvent*> (e);
    
    if( !p()->m_children.isEmpty() ) {
        QList<QPointF> absolutePositions = p()->absolutePositions();
        
        foreach( const QPointF& absolutePosition, absolutePositions ) {
            QPoint shiftedPos = event->pos() - absolutePosition.toPoint();
            
            if ( QRect( QPoint( 0, 0 ), size().toSize() ).contains( shiftedPos ) ) {
                foreach( MarbleGraphicsItem *child, p()->m_children ) {
                    QList<QRectF> childRects = child->boundingRects();
                    
                    foreach( const QRectF& childRect, childRects ) {
                        if( childRect.toRect().contains( shiftedPos ) ) {
                            if( child->eventFilter( object, e ) ) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

MarbleGraphicsItemPrivate *MarbleGraphicsItem::p() const
{
    return d;
}
