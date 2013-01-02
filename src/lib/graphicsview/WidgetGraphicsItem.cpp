//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WidgetGraphicsItem.h"
#include "WidgetGraphicsItem_p.h"

// Marble
#include "MarbleWidget.h"
#include "MarbleDebug.h"

// Qt
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QWidget>

using namespace Marble;

WidgetGraphicsItemPrivate::WidgetGraphicsItemPrivate() :
    m_widget(0), m_marbleWidget(0), m_activeWidget( 0 )
{
    // nothing to do
}

WidgetGraphicsItemPrivate::~WidgetGraphicsItemPrivate()
{
    delete m_widget;
}

WidgetGraphicsItem::WidgetGraphicsItem( MarbleGraphicsItem *parent )
    : ScreenGraphicsItem( parent ),
      d( new WidgetGraphicsItemPrivate() )
{
}
    
WidgetGraphicsItem::~WidgetGraphicsItem() {
    delete d;
}
    
void WidgetGraphicsItem::setWidget( QWidget *widget ) {
    d->m_widget = widget;
    
    QSize size = widget->sizeHint().expandedTo( widget->size() );
    size = size.expandedTo( widget->minimumSize() );
    size = size.boundedTo( widget->maximumSize() );
    setSize( size );
    widget->resize( size );
}

QWidget *WidgetGraphicsItem::widget() const {
    return d->m_widget;
}

void WidgetGraphicsItem::paint( QPainter *painter )
{
    if( d->m_widget == 0 )
        return;

    // Paint widget without a background
    d->m_widget->render( painter, 
                         QPoint( 0, 0 ),
                         QRegion(),
                         QWidget::RenderFlags(QWidget::DrawChildren) );
}

bool WidgetGraphicsItem::eventFilter( QObject *object, QEvent *e )
{
    if ( !visible() || d->m_widget == 0 ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*> (object);
    if ( !widget ) {
        return ScreenGraphicsItem::eventFilter(object, e);
    }

    if ( d->m_marbleWidget != widget ) {
        // Delayed initialization
        d->m_marbleWidget = widget;
    }

    Q_ASSERT(d->m_marbleWidget);

    if ( e->type() == QEvent::MouseButtonDblClick
         || e->type() == QEvent::MouseMove
         || e->type() == QEvent::MouseButtonPress
         || e->type() == QEvent::MouseButtonRelease )
    {
        // Mouse events are forwarded to the underlying widget
        QMouseEvent *event = static_cast<QMouseEvent*> ( e );

        QList<QPointF> widgetPositions = absolutePositions();
        QRectF widgetItemRect;
        QPoint shiftedPos;
        QList<QPointF>::iterator it = widgetPositions.begin();
        bool foundRightPosition = false;
        for(; !foundRightPosition && it != widgetPositions.end(); ++it ) {
            widgetItemRect = QRectF( *it, size() );

            if ( widgetItemRect.contains( event->pos() ) ) {
                foundRightPosition = true;
                shiftedPos = event->pos() - widgetItemRect.topLeft().toPoint();
            }
        }
        
        if ( foundRightPosition ) {
            QWidget *child = d->m_widget->childAt( shiftedPos );

            if ( d->m_activeWidget && d->m_activeWidget != child ) {
                QEvent leaveEvent( QEvent::Leave );
                QApplication::sendEvent( d->m_activeWidget, &leaveEvent );
            }

            if ( child && d->m_activeWidget != child ) {
                QEvent enterEvent( QEvent::Enter );
                QApplication::sendEvent( child, &enterEvent );
            }
            d->m_activeWidget = child;

            if ( child ) {
                shiftedPos -= child->pos(); // transform to children's coordinates
                QMouseEvent shiftedEvent = QMouseEvent( e->type(), shiftedPos,
                        event->globalPos(), event->button(), event->buttons(),
                        event->modifiers() );
                if ( QApplication::sendEvent( child, &shiftedEvent ) ) {
                    d->m_marbleWidget->setCursor( d->m_widget->cursor() );
                    return true;
                }
            }
        } else {
            if ( d->m_activeWidget ) {
                QEvent leaveEvent( QEvent::Leave );
                QApplication::sendEvent( d->m_activeWidget, &leaveEvent );
                d->m_activeWidget = 0;
            }
        }
    }

    return ScreenGraphicsItem::eventFilter(object, e);
}
