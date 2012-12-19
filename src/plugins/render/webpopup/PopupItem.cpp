//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
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
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <qdrawutil.h>

namespace Marble
{

PopupItem::PopupItem( QObject* parent ) :
    QObject( parent ),
    BillboardGraphicsItem(),
    m_widget( new QWidget ),
    m_webView( new QWebView ( m_widget ) ),
    m_needMouseRelease(false)
{
    QGridLayout *childLayout = new QGridLayout;
    QLabel *titleText = new QLabel( m_widget );
    childLayout->addWidget( titleText, 0, 0 );
    QPushButton *hideButton = new QPushButton( m_widget );
    hideButton->setIcon( QIcon( ":/marble/webpopup/icon-remove.png" ) );
    hideButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    hideButton->setCursor( QCursor( Qt::PointingHandCursor ) );
    hideButton->setFlat( true );
    childLayout->addWidget( hideButton, 0, 1 );
    QVBoxLayout *layout = new QVBoxLayout( m_widget );
    layout->addLayout( childLayout );
    layout->addWidget( m_webView );
    m_widget->setLayout( layout );
    m_widget->setAttribute( Qt::WA_NoSystemBackground, true );
    setSize( QSizeF( 300, 350 ) );
    //setVisible( true );

    connect( m_webView, SIGNAL(titleChanged(QString)), titleText, SLOT(setText(QString)) );
    connect( hideButton, SIGNAL(clicked()), this, SIGNAL(hide()) );
}

PopupItem::~PopupItem()
{
    delete m_widget;
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
    QRect popupRect( -10, -10, size().width(), size().height() );
    qDrawBorderPixmap( painter, popupRect, QMargins( 20, 20, 20, 20 ),
                       QPixmap::fromImage( QImage( ":/marble/webpopup/drop-shadow.png" ) ) );
    QImage arrowImage( ":/marble/webpopup/placemark-indicator-left.png" );
    QImage placemarkImage = arrowImage.scaledToWidth( arrowImage.width()/2 );
    painter->drawImage( -(placemarkImage.width()-3),
                        popupRect.height()/2-placemarkImage.height()/2, placemarkImage );
    m_widget->setFixedSize(size().toSize()-QSize(20,20));
    m_widget->render( painter, QPoint( 0, 0 ), QRegion() );
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
        QPoint shiftedPos = event->pos();
        QWidget* child = transform( shiftedPos );
        bool const forcedMouseRelease = m_needMouseRelease && e->type() == QEvent::MouseButtonRelease;
        if ( child || forcedMouseRelease ) {
            if ( !m_needMouseRelease && e->type() == QEvent::MouseButtonPress ) {
                m_needMouseRelease = true;
            } else if ( forcedMouseRelease ) {
                m_needMouseRelease = false;
            }
            if ( !child ) {
                child = m_webView;
            }
            widget->setCursor( Qt::ArrowCursor );
            QMouseEvent shiftedEvent = QMouseEvent( e->type(), shiftedPos,
                                                    event->globalPos(), event->button(), event->buttons(),
                                                    event->modifiers() );
            if ( QApplication::sendEvent( child, &shiftedEvent ) ) {
                widget->setCursor( child->cursor() );
                emit dirty();
                return true;
            }
        }
    } else if ( e->type() == QEvent::Wheel ) {
        // Wheel events are forwarded to the underlying widget
        QWheelEvent *event = static_cast<QWheelEvent*> ( e );
        QPoint shiftedPos = event->pos();
        QWidget* child = transform( shiftedPos );
        if ( child ) {
            widget->setCursor( Qt::ArrowCursor );
            QWheelEvent shiftedEvent = QWheelEvent( shiftedPos,
                                                    event->globalPos(), event->delta(), event->buttons(),
                                                    event->modifiers() );
            if ( QApplication::sendEvent( child, &shiftedEvent ) ) {
                widget->setCursor( child->cursor() );
                emit dirty();
                return true;
            }
        }
    }

    return BillboardGraphicsItem::eventFilter( object, e );
}

QWidget* PopupItem::transform( QPoint &point ) const
{
    QList<QPointF> widgetPositions = positions();
    QList<QPointF>::const_iterator it = widgetPositions.constBegin();
    for( ; it != widgetPositions.constEnd(); ++it ) {
        if ( QRectF( *it, size() ).contains( point ) ) {
            point -= it->toPoint();
            QWidget* child = m_widget->childAt( point );
            if ( child ) {
                point -= child->pos();
            }
            return child;
        }
    }
    return 0;
}

}

#include "PopupItem.moc"
