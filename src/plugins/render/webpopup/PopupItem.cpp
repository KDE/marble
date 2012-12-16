//
// This file is part of the Marble Virtual Globe.
//
// This program is free software WebPopupFloatItemd under the GNU LGPL. You can
// find a copy of this WebPopupFloatItem in WebPopupFloatItem.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Dennis Nienhüser  <earthwings@gentoo.org>
//

#include "PopupItem.h"
#include "MarbleWidget.h"

#include <QtWebKit/QWebView>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>

namespace Marble
{

PopupItem::PopupItem( QObject* parent ) :
    QObject( parent ),
    BillboardGraphicsItem(),
    m_webView( new QWebView ),
    m_needMouseRelease(false)
{
    m_webView->setMaximumSize( 600, 800 );
    setSize( QSizeF( 600, 800 ) );
    setVisible( false );
}

PopupItem::~PopupItem()
{
    delete m_webView;
}

void PopupItem::setUrl( const QUrl &url )
{
    if ( m_webView ) {
        m_webView->setUrl( url );
        setVisible( true );
        emit dirty();
    }
}

void PopupItem::setContent( const QString &html )
{
    /** @todo Pass to web view */
    m_content = html;
}

void PopupItem::paint( QPainter *painter )
{
    m_webView->setMaximumSize( size().toSize() );
    m_webView->render( painter, QPoint( 0, 0 ), QRegion(),
                       QWidget::RenderFlag( QWidget::DrawChildren ) );
}

bool PopupItem::eventFilter( QObject *object, QEvent *e )
{
    MarbleWidget *widget = dynamic_cast<MarbleWidget*> ( object );
    if ( !widget ) {
        return BillboardGraphicsItem::eventFilter( object, e );
    }

    if ( e->type() == QEvent::MouseButtonDblClick
            || e->type() == QEvent::MouseMove
            || e->type() == QEvent::MouseButtonPress
            || e->type() == QEvent::MouseButtonRelease )
    {
        // Mouse events are forwarded to the underlying widget
        QMouseEvent *event = static_cast<QMouseEvent*> ( e );
        QPoint const shiftedPos = transform( event->pos() );
        bool const forcedMouseRelease = m_needMouseRelease && e->type() == QEvent::MouseButtonRelease;
        if ( !shiftedPos.isNull() || forcedMouseRelease ) {
            if ( !m_needMouseRelease && e->type() == QEvent::MouseButtonPress ) {
                m_needMouseRelease = true;
            } else if ( forcedMouseRelease ) {
                m_needMouseRelease = false;
            }
            widget->setCursor( Qt::ArrowCursor );
            // transform to children's coordinates
            QMouseEvent shiftedEvent = QMouseEvent( e->type(), shiftedPos,
                                                    event->globalPos(), event->button(), event->buttons(),
                                                    event->modifiers() );
            if ( QApplication::sendEvent( m_webView, &shiftedEvent ) ) {
                widget->setCursor( m_webView->cursor() );
                emit dirty();
                return true;
            }
        }
    } else if ( e->type() == QEvent::Wheel ) {
        // Wheel events are forwarded to the underlying widget
        QWheelEvent *event = static_cast<QWheelEvent*> ( e );
        QPoint const shiftedPos = transform( event->pos() );
        if ( !shiftedPos.isNull() ) {
            widget->setCursor( Qt::ArrowCursor );
            QWheelEvent shiftedEvent = QWheelEvent( shiftedPos,
                                                    event->globalPos(), event->delta(), event->buttons(),
                                                    event->modifiers() );
            if ( QApplication::sendEvent( m_webView, &shiftedEvent ) ) {
                widget->setCursor( m_webView->cursor() );
                emit dirty();
                return true;
            }
        }
    }

    return BillboardGraphicsItem::eventFilter( object, e );
}

QPoint PopupItem::transform( const QPoint &point ) const
{
    QList<QPointF> widgetPositions = positions();
    QList<QPointF>::const_iterator it = widgetPositions.begin();
    for( ; it != widgetPositions.end(); ++it ) {
        if ( QRectF( *it, size() ).contains( point ) ) {
            return point - it->toPoint();
        }
    }
    return QPoint();
}

}

#include "PopupItem.moc"
