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
#include "MarbleDebug.h"
#include "ViewportParams.h"

// Qt
#include <QList>
#include <QSet>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>

using namespace Marble;

MarbleGraphicsItem::MarbleGraphicsItem( MarbleGraphicsItemPrivate *d_ptr )
    : d( d_ptr )
{
}
    
MarbleGraphicsItem::~MarbleGraphicsItem()
{
    delete d;
}

bool MarbleGraphicsItem::paintEvent( QPainter *painter, const ViewportParams *viewport )
{
    if ( !p()->m_visibility ) {
        return true;
    }

    if ( p()->m_repaintNeeded ) {
        p()->updateChildPositions();
        p()->m_pixmap = QPixmap();
        p()->m_repaintNeeded = false;
    }

    setProjection( viewport );

    if ( p()->positions().size() == 0 ) {
        return true;
    }

    // At the moment, as GraphicsItems can't be zoomed or rotated ItemCoordinateCache
    // and DeviceCoordianteCache is exactly the same
    if ( ItemCoordinateCache == cacheMode()
         || DeviceCoordinateCache == cacheMode() )
    {
        const QSize neededPixmapSize = size().toSize() + QSize( 1, 1 ); // adding a pixel for rounding errors
        if ( p()->m_pixmap.size() != neededPixmapSize ) {

            if ( p()->m_pixmap.size() != neededPixmapSize ) {
                if ( size().isValid() && !size().isNull() ) {
                    p()->m_pixmap = QPixmap( neededPixmapSize );
                }
                else {
                    mDebug() << "Warning: Invalid pixmap size suggested: " << d->m_size;
                }
            }

            p()->m_pixmap.fill( Qt::transparent );
            QPainter pixmapPainter( &p()->m_pixmap );
            // We paint in best quality here, as we only have to paint once.
            pixmapPainter.setRenderHint( QPainter::Antialiasing, true );
            // The cache image will get a 0.5 pixel bounding to save antialiasing effects.
            pixmapPainter.translate( 0.5, 0.5 );
            paint( &pixmapPainter );

            // Paint children
            foreach ( MarbleGraphicsItem *item, p()->m_children ) {
                item->paintEvent( &pixmapPainter, viewport );
            }
        }

        foreach( const QPointF& position, p()->positions() ) {
            painter->drawPixmap( position, p()->m_pixmap );
        }
    }
    else {
        foreach( const QPointF& position, p()->positions() ) {
            painter->save();

            painter->translate( position );
            paint( painter );

            // Paint children
            foreach ( MarbleGraphicsItem *item, p()->m_children ) {
                item->paintEvent( painter, viewport );
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

QList<QRectF> MarbleGraphicsItemPrivate::boundingRects() const
{
    QList<QRectF> list;

    foreach( const QPointF &point, positions() ) {
        QRectF rect( point, m_size );
        if( rect.x() < 0 )
            rect.setLeft( 0 );
        if( rect.y() < 0 )
            rect.setTop( 0 );

        list.append( rect );
    }

    return list;
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
        p()->m_repaintNeeded = true;
    }
}

void MarbleGraphicsItem::update()
{
    p()->m_repaintNeeded = true;

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
    if ( p()->m_size != size ) {
        p()->m_size = size;
        update();
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

void MarbleGraphicsItem::paint( QPainter *painter )
{
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
                    QList<QRectF> childRects = child->d->boundingRects();
                    
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

void MarbleGraphicsItem::setProjection( const ViewportParams *viewport )
{
    p()->setProjection( viewport );
}
