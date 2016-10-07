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
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>

using namespace Marble;

MarbleGraphicsItem::MarbleGraphicsItem(MarbleGraphicsItemPrivate *dd)
    : d_ptr(dd)
{
}

MarbleGraphicsItem::~MarbleGraphicsItem()
{
    delete d_ptr;
}

bool MarbleGraphicsItem::paintEvent( QPainter *painter, const ViewportParams *viewport )
{
    Q_D(MarbleGraphicsItem);

    if (!d->m_visibility) {
        return true;
    }

    if (d->m_repaintNeeded) {
        d->updateChildPositions();
        d->m_pixmap = QPixmap();
        d->m_repaintNeeded = false;
    }

    setProjection( viewport );

    if (d->positions().size() == 0) {
        return true;
    }

    // At the moment, as GraphicsItems can't be zoomed or rotated ItemCoordinateCache
    // and DeviceCoordianteCache is exactly the same
    if ( ItemCoordinateCache == cacheMode()
         || DeviceCoordinateCache == cacheMode() )
    {
        const qreal scale = painter->device()->devicePixelRatio();

        const QSize neededPixmapSize = scale * size().toSize() + QSize( 1, 1 ); // adding a pixel for rounding errors

        if (d->m_pixmap.size() != neededPixmapSize ||
            d->m_pixmap.devicePixelRatio() != scale) {


            if ( size().isValid() && !size().isNull() ) {
                d->m_pixmap = QPixmap(neededPixmapSize);
                d->m_pixmap.setDevicePixelRatio(scale);
            }
            else {
                mDebug() << "Warning: Invalid pixmap size suggested: " << d->m_size;
            }

            d->m_pixmap.fill(Qt::transparent);
            QPainter pixmapPainter(&d->m_pixmap);
            // We paint in best quality here, as we only have to paint once.
            pixmapPainter.setRenderHint( QPainter::Antialiasing, true );
            // The cache image will get a 0.5 pixel bounding to save antialiasing effects.
            pixmapPainter.translate( 0.5, 0.5 );
            paint( &pixmapPainter );

            // Paint children
            foreach (MarbleGraphicsItem *item, d->m_children) {
                item->paintEvent( &pixmapPainter, viewport );
            }
        }

        foreach (const QPointF& position, d->positions()) {
            painter->drawPixmap(position, d->m_pixmap);
        }
    }
    else {
        foreach (const QPointF& position, d->positions()) {
            painter->save();

            painter->translate( position );
            paint( painter );

            // Paint children
            foreach (MarbleGraphicsItem *item, d->m_children) {
                item->paintEvent( painter, viewport );
            }

            painter->restore();
        }
    }

    return true;
}

bool MarbleGraphicsItem::contains( const QPointF& point ) const
{
    Q_D(const MarbleGraphicsItem);
    foreach (const QRectF& rect, d->boundingRects()) {
        if( rect.contains( point ) )
            return true;
    }
    return false;
}

QVector<QRectF> MarbleGraphicsItemPrivate::boundingRects() const
{
    const QVector<QPointF> positions = this->positions();

    QVector<QRectF> list;
    list.reserve(positions.count());

    foreach (const QPointF &point, positions) {
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
    Q_D(const MarbleGraphicsItem);
    return d->m_size;
}

AbstractMarbleGraphicsLayout *MarbleGraphicsItem::layout() const
{
    Q_D(const MarbleGraphicsItem);
    return d->m_layout;
}

void MarbleGraphicsItem::setLayout( AbstractMarbleGraphicsLayout *layout )
{
    Q_D(MarbleGraphicsItem);
    // Deleting the old layout
    delete d->m_layout;
    d->m_layout = layout;
    update();
}

MarbleGraphicsItem::CacheMode MarbleGraphicsItem::cacheMode() const
{
    Q_D(const MarbleGraphicsItem);
    return d->m_cacheMode;
}

void MarbleGraphicsItem::setCacheMode( CacheMode mode )
{
    Q_D(MarbleGraphicsItem);
    d->m_cacheMode = mode;
    if (d->m_cacheMode == NoCache) {
        d->m_repaintNeeded = true;
    }
}

void MarbleGraphicsItem::update()
{
    Q_D(MarbleGraphicsItem);
    d->m_repaintNeeded = true;

    // Update the parent.
    if (d->m_parent) {
        d->m_parent->update();
    }
}

bool MarbleGraphicsItem::visible() const
{
    Q_D(const MarbleGraphicsItem);
    return d->m_visibility;
}

void MarbleGraphicsItem::setVisible( bool visible )
{
    Q_D(MarbleGraphicsItem);
    d->m_visibility = visible;
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
    Q_D(MarbleGraphicsItem);
    if (d->m_size != size) {
        d->m_size = size;
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

    Q_D(const MarbleGraphicsItem);
    QMouseEvent *event = static_cast<QMouseEvent*> (e);

    if (!d->m_children.isEmpty()) {
        const QVector<QPointF> absolutePositions = d->absolutePositions();

        foreach( const QPointF& absolutePosition, absolutePositions ) {
            QPoint shiftedPos = event->pos() - absolutePosition.toPoint();
            
            if ( QRect( QPoint( 0, 0 ), size().toSize() ).contains( shiftedPos ) ) {
                foreach (MarbleGraphicsItem *child, d->m_children) {
                    const QVector<QRectF> childRects = child->d_func()->boundingRects();
                    
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

void MarbleGraphicsItem::setProjection( const ViewportParams *viewport )
{
    Q_D(MarbleGraphicsItem);
    d->setProjection(viewport);
}
