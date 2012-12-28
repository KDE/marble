//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Torsten Rahn      <tackat@kde.org>
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Dennis Nienhüser  <earthwings@gentoo.org>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "PopupItem.h"
#include "MarbleWidget.h"

#include <QtWebKit/QWebView>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QPixmapCache>
#include <qdrawutil.h>

namespace Marble
{

PopupItem::PopupItem( QObject* parent ) :
    QObject( parent ),
    BillboardGraphicsItem(),
    m_widget( new QWidget ),
    m_webView( new QWebView ( m_widget ) ),
    m_textColor( QColor(Qt::black) ),
    m_backColor( QColor(Qt::white) ),
    m_needMouseRelease(false)
{
    setVisible( false );

    QGridLayout *childLayout = new QGridLayout;
    m_titleText = new QLabel( m_widget );
    childLayout->addWidget( m_titleText, 0, 0 );
    QPushButton *hideButton = new QPushButton( m_widget );
    hideButton->setIcon( QIcon( ":/marble/webpopup/icon-remove.png" ) );
    hideButton->setMaximumWidth( 24 );
    hideButton->setCursor( QCursor( Qt::PointingHandCursor ) );
    hideButton->setFlat( true );
    childLayout->addWidget( hideButton, 0, 1 );
    QVBoxLayout *layout = new QVBoxLayout( m_widget );
    layout->addLayout( childLayout );
    layout->addWidget( m_webView );
    m_widget->setLayout( layout );
    m_widget->setAttribute( Qt::WA_NoSystemBackground, true );
    QPalette palette = m_webView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    m_webView->setPalette(palette);
    m_webView->page()->setPalette(palette);
    m_webView->setAttribute(Qt::WA_OpaquePaintEvent, false);

    connect( m_webView, SIGNAL(titleChanged(QString)), m_titleText, SLOT(setText(QString)) );
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

	QPalette palette = m_webView->palette();
	palette.setBrush(QPalette::Base, Qt::transparent);
	m_webView->setPalette(palette);
	m_webView->page()->setPalette(palette);
	m_webView->setAttribute(Qt::WA_OpaquePaintEvent, false);

        emit dirty();
    }
}

void PopupItem::setContent( const QString &html )
{
    m_content = html;
    m_webView->setHtml(html);
}

void PopupItem::setTextColor(const QColor &color)
{
    if(color.isValid() && m_titleText != 0) {
        m_textColor = color;
        QPalette palette(m_titleText->palette());
        palette.setColor(QPalette::WindowText, m_textColor);
        m_titleText->setPalette(palette);
    }
}
void PopupItem::setBackgroundColor(const QColor &color)
{
    if(color.isValid()) {
        m_backColor = color;
        QPixmapCache::remove( "marble/webpopup/webpopup2" );
        QPixmapCache::remove( "marble/webpopup/arrow2_topleft" );
        QPixmapCache::remove( "marble/webpopup/arrow2_bottomleft" );
        QPixmapCache::remove( "marble/webpopup/arrow2_topright" );
        QPixmapCache::remove( "marble/webpopup/arrow2_bottomright" );
    }
}

void PopupItem::colorize( QImage &img, const QColor &col )
{
    if (img.depth() <= 8) return;
    int pixels = img.width()*img.height();
    unsigned int *data = (unsigned int *) img.bits();
    for (int i=0; i < pixels; ++i) {
        int val = qGray(data[i]);
        data[i] = qRgba(col.red()*val/255,col.green()*val/255, col.blue()*val/255, qAlpha(data[i]));
    }
}

void PopupItem::paint( QPainter *painter )
{
    QRect popupRect;
    QPixmap image = pixmap("marble/webpopup/arrow2_vertical_topright");

    if ( alignment() & Qt::AlignRight ) {
        popupRect.setRect( image.width() - 13, -10,
                           size().width() - ( image.width() - 23 ),
                           size().height() + 20 );
        qDrawBorderPixmap(painter, popupRect, QMargins( 20, 20, 20, 20 ),
                          pixmap("marble/webpopup/webpopup2"));
        if ( alignment() & Qt::AlignTop ) {
            image = pixmap("marble/webpopup/arrow2_bottomleft");
            painter->drawPixmap( 0, size().height() - image.height(), image );
        } else if ( alignment() & Qt::AlignBottom ) {
            image = pixmap("marble/webpopup/arrow2_topleft");
            painter->drawPixmap( 0, 0, image );
        } else { // for no horizontal align value and Qt::AlignVCenter
            image = pixmap("marble/webpopup/arrow2_topleft");
            painter->drawPixmap( 0, size().height() / 2, image );
        }
        m_widget->render( painter, QPoint( image.width() - 3, 0 ), QRegion() );
    } else if ( alignment() & Qt::AlignLeft ) {
        popupRect.setRect( -10, -10,
                           size().width() - ( image.width() - 23 ),
                           size().height() + 20 );
        qDrawBorderPixmap(painter, popupRect, QMargins( 20, 20, 20, 20 ),
                          pixmap("marble/webpopup/webpopup2"));
        if ( alignment() & Qt::AlignTop ) {
            image = pixmap("marble/webpopup/arrow2_bottomright");
            painter->drawPixmap( size().width() - image.width(),
                                 size().height() - image.height(), image );
        } else if ( alignment() & Qt::AlignBottom ) {
            image = pixmap("marble/webpopup/arrow2_topright");
            painter->drawPixmap( size().width() - image.width(),
                                 0, image );
        } else { // for no horizontal align value and Qt::AlignVCenter
            image = pixmap("marble/webpopup/arrow2_topright");
            painter->drawPixmap( size().width() - image.width(),
                                 size().height() / 2 - image.height() / 2 + 23, image );
        }
        m_widget->render( painter, QPoint( 5, 0 ), QRegion() );
    } else if ( alignment() & Qt::AlignHCenter )
    {
        if ( alignment() & Qt::AlignTop )
        {
            image = pixmap("marble/webpopup/arrow2_vertical_bottomright");
            popupRect.setRect( -10, -10, size().width() + 20,
                               size().height() - image.height() + 23 );
            qDrawBorderPixmap(painter, popupRect, QMargins( 20, 20, 20, 20 ),
                              pixmap("marble/webpopup/webpopup2"));
            painter->drawPixmap( size().width() / 2 - image.width(),
                                 size().height() - image.height(), image );
            m_widget->render( painter, QPoint( 0, 0 ), QRegion() );
        } else if ( alignment() & Qt::AlignBottom ) {
            image = pixmap("marble/webpopup/arrow2_vertical_topleft");
            popupRect.setRect( -10, image.height() - 13, size().width() + 20,
                               size().height() - image.height() + 23 );
            qDrawBorderPixmap(painter, popupRect, QMargins( 20, 20, 20, 20 ),
                              pixmap("marble/webpopup/webpopup2"));
            painter->drawPixmap( size().width() / 2, 0, image );
            m_widget->render( painter, QPoint( 5, image.height() - 7 ), QRegion() );
        } else { // for no horizontal align value and Qt::AlignVCenter
            popupRect.setRect( -10, -10, size().width() + 20,
                               size().height() + 20 );
            qDrawBorderPixmap(painter, popupRect, QMargins( 20, 20, 20, 20 ),
                              pixmap("marble/webpopup/webpopup2"));
            m_widget->render( painter, QPoint( 0, 0 ), QRegion() );
        }
    }
    m_widget->setFixedSize( popupRect.width() - 20,
                            popupRect.height() - 20 );
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
    /*
     * Fixes for mouse events to trigger when the web popup
     * is shifted in accordance with the horizontal alignment
     */
    if ( alignment() & Qt::AlignRight )
        point -= QPoint( 117, 0 );
    else if ( alignment() & Qt::AlignLeft )
        point -= QPoint( 5, 0 );
    else if ( alignment() & Qt::AlignHCenter )
    {
        if ( alignment() & Qt::AlignTop )
        {
            point -= QPoint( 0, 0 );
        } else if ( alignment() & Qt::AlignBottom )
        {
            point-= QPoint( 5, 57 );
        } else {
            point -= QPoint( 0, 0 );
        }
    }

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

QPixmap PopupItem::pixmap( const QString &imageId )
{
  QPixmap result;
  if ( !QPixmapCache::find( imageId, result ) ) {
    QImage bottom = QImage( QString( ":/%1_shadow.png" ).arg( imageId) );
    QImage top = QImage( QString( ":/%1.png" ).arg( imageId) );
    colorize( top, m_backColor );
    QPainter painter( &bottom );
    painter.drawImage( QPoint(0,0), top );

    result = QPixmap::fromImage( bottom );
    QPixmapCache::insert( imageId, result );
  }

  return result;
}

}

#include "PopupItem.moc"
