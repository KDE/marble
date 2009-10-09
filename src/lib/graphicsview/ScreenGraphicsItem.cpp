//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "ScreenGraphicsItem.h"
#include "ScreenGraphicsItem_p.h"

#include <QtGui/QMouseEvent>

// Marble
#include "MarbleMap.h"
#include "MarbleWidget.h"

using namespace Marble;

ScreenGraphicsItem::ScreenGraphicsItem( MarbleGraphicsItem *parent )
    : MarbleGraphicsItem( new ScreenGraphicsItemPrivate( this, parent ) )
{
}

ScreenGraphicsItem::ScreenGraphicsItem( const QPointF& position, const QSizeF& size,
                                        MarbleGraphicsItem *parent )
    : MarbleGraphicsItem( new ScreenGraphicsItemPrivate( this, parent ) )
{
    setPosition( position );
    setSize( size );
}

ScreenGraphicsItem::ScreenGraphicsItem( ScreenGraphicsItemPrivate *d_ptr )
    : MarbleGraphicsItem( d_ptr )
{
}

ScreenGraphicsItem::ScreenGraphicsItem( const QPointF& position, const QSizeF& size,
                                        ScreenGraphicsItemPrivate *d_ptr )
    : MarbleGraphicsItem( d_ptr )
{
    setPosition( position );
    setSize( size );
}

ScreenGraphicsItem::~ScreenGraphicsItem()
{
}

QPointF ScreenGraphicsItem::position() const
{
    return p()->m_position;
}

void ScreenGraphicsItem::setPosition( const QPointF& position )
{
    p()->m_position = position;
}

QPointF ScreenGraphicsItem::positivePosition() const
{
    return p()->positivePosition();
}

ScreenGraphicsItem::GraphicsItemFlags ScreenGraphicsItem::flags() const
{
    return p()->m_flags;
}

void ScreenGraphicsItem::setFlags( ScreenGraphicsItem::GraphicsItemFlags flags )
{
    p()->m_flags = flags;
}

void ScreenGraphicsItem::changeViewport( ViewportParams *viewport )
{
    Q_UNUSED( viewport );
}

bool ScreenGraphicsItem::eventFilter( QObject *object, QEvent *e )
{
    if ( !visible() || !p()->isMovable() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*>(object);
    if ( !widget ) {
        return false;
    }

    if ( e->type() == QEvent::MouseMove && !p()->m_floatItemMoving ) {
        return false;
    }

    // Move float items
    bool cursorAboveFloatItem = false;
    if ( e->type() == QEvent::MouseMove
         || e->type() == QEvent::MouseButtonPress
         || e->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        QRectF floatItemRect = QRectF( positivePosition(), size() + QSize( 1, 1 ) );

        // Click and move above a float item triggers moving the float item
        if ( contains( event->pos() ) ) {
            cursorAboveFloatItem = true;

            if ( e->type() == QEvent::MouseButtonPress && event->button() == Qt::LeftButton ) {
                p()->m_floatItemMoveStartPos = event->pos();
                p()->m_floatItemMoving = true;
                return true;
            }
        }

        if ( e->type() == QEvent::MouseMove && event->buttons() & Qt::LeftButton
            && ( cursorAboveFloatItem || p()->m_floatItemMoving ) )
        {
            p()->m_floatItemMoving = true;
            const QPoint &point = event->pos();
            QPointF position = positivePosition();
            qreal newX = position.x()+point.x()-p()->m_floatItemMoveStartPos.x();
            qreal newY = position.y()+point.y()-p()->m_floatItemMoveStartPos.y();
            if ( newX >= 0 && newY >= 0 ) {
                // docking behavior
                const qreal dockArea = 60.0; // Alignment area width/height
                const qreal dockJump = 30.0; // Alignment indicator jump size
                if ( widget->width()-size().width()-newX < dockArea ) {
                    newX = qMin(qreal(-1.0), size().width()+newX-widget->width());
                    if (p()->m_floatItemMoveStartPos.x()<event->pos().x()) {
                        // Indicate change to right alignment with a short jump
                        newX = qMax( newX, -(dockArea-dockJump) );
                    }
                }
                if ( widget->height()-size().height()-newY < dockArea ) {
                    newY = qMin(qreal(-1.0),size().height()+newY-widget->height());
                    if (p()->m_floatItemMoveStartPos.y()<event->pos().y()) {
                       // Indicate change to bottom alignment with a short jump
                       newY = qMax( newY, -(dockArea-dockJump) );
                    }
                }

                setPosition( QPointF( newX,newY ) );
                QRect newFloatItemRect = QRectF( positivePosition(), size() + QSize( 1, 1 ) ).toRect();
                p()->m_floatItemMoveStartPos = event->pos();
                QRegion dirtyRegion( floatItemRect.toRect() );
                dirtyRegion = dirtyRegion.united( newFloatItemRect );

                widget->setAttribute( Qt::WA_NoSystemBackground,  false );
                widget->repaint(dirtyRegion);
                widget->setAttribute( Qt::WA_NoSystemBackground,  widget->map()->mapCoversViewport() );
                return true;
            }
        }

        if ( e->type() == QEvent::MouseButtonRelease ) {
            p()->m_floatItemMoving = false;
        }

        // Adjusting Cursor shape
        if ( cursorAboveFloatItem || p()->m_floatItemMoving ) {
            widget->setCursor(QCursor(Qt::SizeAllCursor));
            return true;
        }
    }

    return false;
}

ScreenGraphicsItemPrivate *ScreenGraphicsItem::p() const
{
    return reinterpret_cast<ScreenGraphicsItemPrivate *>( d );
}
