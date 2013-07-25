//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012   Mohammed Nafees   <nafees.technocool@gmail.com>
// Copyright 2012   Dennis Nienhüser  <earthwings@gentoo.org>
// Copyright 2012   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "PopupLayer.h"
#include "MarbleWidget.h"
#include "PopupItem.h"

#include <QMouseEvent>
#include <QApplication>
#include <QAction>

namespace Marble
{

PopupLayer::PopupLayer( MarbleWidget *marbleWidget, QObject *parent ) :
    QObject( parent ),
    m_popupItem( new PopupItem( this ) ),
    m_widget( marbleWidget ),
    m_adjustMap( false )
{
    connect( m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
    connect( m_popupItem, SIGNAL(hide()), this, SLOT(hidePopupItem()) );
}

PopupLayer::~PopupLayer()
{
}

QStringList PopupLayer::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString PopupLayer::renderPolicy() const
{
    return "ALWAYS";
}

bool PopupLayer::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString&, GeoSceneLayer* )
{
    if ( visible() ) {
        setAppropriateSize( viewport );
        if ( m_adjustMap ) {
            m_widget->centerOn( m_popupItem->coordinate(), true );
            m_adjustMap = false;
        }
        m_popupItem->paintEvent( painter, viewport );
    }

    return true;
}

bool PopupLayer::eventFilter( QObject *object, QEvent *e )
{
    return visible() && m_popupItem->eventFilter( object, e );
}

qreal PopupLayer::zValue() const
{
    return 4711.23;
}

bool PopupLayer::visible() const
{
    return m_popupItem->visible();
}

void PopupLayer::setVisible( bool visible )
{
    m_popupItem->setVisible( visible );
    if ( !visible ) {
        disconnect( m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
        m_popupItem->clearHistory();
        emit repaintNeeded();
    }
    else {
        connect( m_popupItem, SIGNAL(repaintNeeded()), this, SIGNAL(repaintNeeded()) );
    }
}

void PopupLayer::popup()
{
    m_adjustMap = true;
    setVisible( true );
}

void PopupLayer::setCoordinates(const GeoDataCoordinates &coordinates , Qt::Alignment alignment)
{
    m_popupItem->setCoordinate( coordinates );
    m_popupItem->setAlignment( alignment );
}

void PopupLayer::setUrl( const QUrl &url )
{
    m_popupItem->setUrl( url );
}

void PopupLayer::setContent( const QString &html )
{
    m_popupItem->setContent( html );
    emit repaintNeeded();
}

void PopupLayer::setBackgroundColor(const QColor &color)
{
    if(color.isValid()) {
        m_popupItem->setBackgroundColor(color);
    }
}

void PopupLayer::setTextColor(const QColor &color)
{
    if(color.isValid()) {
        m_popupItem->setTextColor(color);
    }
}

void PopupLayer::setSize( const QSizeF &size )
{
    m_requestedSize = size;
}

void PopupLayer::setAppropriateSize( const ViewportParams *viewport )
{
    qreal margin = 15.0;

    QSizeF maximumSize;
    maximumSize.setWidth( viewport->width() / 2.0 - margin );
    maximumSize.setHeight( viewport->height() - 2.0 * margin );

    QSizeF minimumSize( 240.0, 240.0 );

    m_popupItem->setSize( m_requestedSize.boundedTo( maximumSize ).expandedTo( minimumSize ) );
}

void PopupLayer::setPosition( const QPointF &position )
{
    /** @todo Implement */
    Q_UNUSED( position );
}

void PopupLayer::hidePopupItem()
{
    setVisible( false );
}

}

#include "PopupLayer.moc"
