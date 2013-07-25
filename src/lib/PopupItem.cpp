//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Torsten Rahn      <tackat@kde.org>
// Copyright 2013   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Dennis Nienhüser  <earthwings@gentoo.org>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "PopupItem.h"
#include "MarbleWidget.h"
#include "MarbleWebView.h"

#include <QPointer>
#include <QWebView>
#include <QWebHistory>
#include <QPrinter>
#include <QPrintDialog>
#include <QMouseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmapCache>
#include <QMenu>
#include <QKeyEvent>
#include <QClipboard>
#include <qdrawutil.h>

namespace Marble
{

PopupItem::PopupItem( QObject* parent ) :
    QObject( parent ),
    BillboardGraphicsItem(),
    m_widget( new QWidget ),
    m_textColor( QColor(Qt::black) ),
    m_backColor( QColor(Qt::white) ),
    m_needMouseRelease(false)
{
    setCacheMode( ItemCoordinateCache );
    setVisible( false );
    setSize( QSizeF( 240.0, 320.0 ) );

    m_ui.setupUi( m_widget );

    m_ui.goBackButton->setVisible( false );
    connect( m_ui.goBackButton, SIGNAL(clicked()), this, SLOT(goBack()) );

#ifdef QT_NO_PRINTER
    m_ui.printButton->setVisible( false );
#else
    m_ui.printButton->setVisible( true );
    connect( m_ui.printButton, SIGNAL(clicked()), this, SLOT(printContent()) );
#endif

    m_widget->setAttribute( Qt::WA_NoSystemBackground, true );
    QPalette palette = m_ui.webView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    m_ui.webView->setPalette(palette);
    m_ui.webView->page()->setPalette(palette);
    m_ui.webView->setAttribute(Qt::WA_OpaquePaintEvent, false);
    m_ui.webView->setUrl( QUrl( "about:blank" ) );

    connect( m_ui.webView, SIGNAL(titleChanged(QString)), m_ui.titleText, SLOT(setText(QString)) );
    connect( m_ui.webView, SIGNAL(urlChanged(QUrl)), this, SLOT(updateBackButton()) );
    connect( m_ui.hideButton, SIGNAL(clicked()), this, SIGNAL(hide()) );

    // Update the popupitem on changes while loading the webpage
    connect( m_ui.webView->page(), SIGNAL(repaintRequested(QRect)), this, SLOT(requestUpdate()) );
}

PopupItem::~PopupItem()
{
    delete m_widget;
}

bool PopupItem::isPrintButtonVisible() const
{
    return m_ui.printButton->isVisible();
}

void PopupItem::setPrintButtonVisible( bool display )
{
    m_ui.printButton->setVisible( display );
}

void PopupItem::setUrl( const QUrl &url )
{
    m_ui.webView->setUrl( url );
    setVisible( true );

    QPalette palette = m_ui.webView->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    m_ui.webView->setPalette(palette);
    m_ui.webView->page()->setPalette(palette);
    m_ui.webView->setAttribute(Qt::WA_OpaquePaintEvent, false);

    requestUpdate();
}

void PopupItem::setContent( const QString &html )
{
    m_content = html;
    m_ui.webView->setHtml( html );

    requestUpdate();
}

void PopupItem::setTextColor(const QColor &color)
{
    if(color.isValid() && m_ui.titleText != 0) {
        m_textColor = color;
        QPalette palette(m_ui.titleText->palette());
        palette.setColor(QPalette::WindowText, m_textColor);
        m_ui.titleText->setPalette(palette);
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

void PopupItem::colorize( QImage &img, const QColor &col ) const
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

    if ( e->type() == QEvent::ContextMenu) {
        QApplication::sendEvent( m_ui.webView, e );
        return BillboardGraphicsItem::eventFilter( object, e );
    }

    if ( e->type() == QEvent::KeyPress ) {
        QApplication::sendEvent( m_ui.webView, e );
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
                child = m_ui.webView;
            }
            QMouseEvent shiftedEvent = QMouseEvent( e->type(), shiftedPos,
                                                    event->globalPos(), event->button(), event->buttons(),
                                                    event->modifiers() );
            if ( QApplication::sendEvent( child, &shiftedEvent ) ) {
                widget->setCursor( child->cursor() );
                emit repaintNeeded();
                return true;
            }
        }
    } else if ( e->type() == QEvent::Wheel ) {
        // Wheel events are forwarded to the underlying widget
        QWheelEvent *event = static_cast<QWheelEvent*> ( e );
        QPoint shiftedPos = event->pos();
        QWidget* child = transform( shiftedPos );
        if ( child ) {
            QWheelEvent shiftedEvent = QWheelEvent( shiftedPos,
                                                    event->globalPos(), event->delta(), event->buttons(),
                                                    event->modifiers() );
            if ( QApplication::sendEvent( child, &shiftedEvent ) ) {
                widget->setCursor( child->cursor() );
                emit repaintNeeded();
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

void PopupItem::clearHistory()
{
    m_content.clear();
    m_ui.webView->setUrl( QUrl( "about:blank" ) );
    m_ui.webView->history()->clear();
}

void PopupItem::requestUpdate()
{
    update();
    emit repaintNeeded();
}

void PopupItem::printContent()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    QPointer<QPrintDialog> dialog = new QPrintDialog(&printer);
    if (dialog->exec() == QPrintDialog::Accepted) {
        m_ui.webView->print(&printer);
    }
    delete dialog;
#endif
}

void PopupItem::updateBackButton()
{
    bool const hasHistory = m_ui.webView->history()->count() > 1;
    bool const previousIsHtml = !m_content.isEmpty() && m_ui.webView->history()->currentItemIndex() == 1;
    bool const atStart = m_ui.webView->history()->currentItemIndex() <= 1;
    bool const currentIsHtml = m_ui.webView->url() == QUrl( "about:blank" );
    m_ui.goBackButton->setVisible( hasHistory && !currentIsHtml && ( previousIsHtml || !atStart ) );
}

void PopupItem::goBack()
{
    if ( m_ui.webView->history()->currentItemIndex() == 1 && !m_content.isEmpty() ) {
        m_ui.webView->setHtml( m_content );
    } else {
        m_ui.webView->back();
    }
    updateBackButton();
}

QPixmap PopupItem::pixmap( const QString &imageId ) const
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
