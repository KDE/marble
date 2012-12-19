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

#include "WebPopupFloatItem.h"
#include "MarbleWidget.h"
#include "PopupItem.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QAction>

namespace Marble
{

WebPopupFloatItem::WebPopupFloatItem( const MarbleModel *marbleModel ) :
    RenderPlugin( marbleModel ),
    m_popupItem( 0 )
{
    setVisible( true );
    setEnabled( true );
}

WebPopupFloatItem::~WebPopupFloatItem()
{
}

QStringList WebPopupFloatItem::backendTypes() const
{
    return QStringList( "Web Popup" );
}

QString WebPopupFloatItem::name() const
{
    return tr( "Web Popup" );
}

QString WebPopupFloatItem::guiString() const
{
    return tr( "&Web Popup" );
}

QString WebPopupFloatItem::nameId() const
{
    return QString( "Web Popup" );
}

QString WebPopupFloatItem::version() const
{
    return "1.0";
}

QString WebPopupFloatItem::description() const
{
    return tr( "This is a float item that shows a popup dialog with information." );
}

QString WebPopupFloatItem::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> WebPopupFloatItem::pluginAuthors() const
{
    return QList<PluginAuthor>()
           << PluginAuthor( "Mohammed Nafees", "nafees.technocool@gmail.com" )
           << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "<earthwings@gentoo.org>" );
}

QIcon WebPopupFloatItem::icon() const
{
    return QIcon();
}

void WebPopupFloatItem::initialize()
{
    m_popupItem = new PopupItem;
    connect( m_popupItem, SIGNAL(dirty()), this, SIGNAL(repaintNeeded()) );
    connect( m_popupItem, SIGNAL(hide()), this, SLOT(hidePopupItem()) );
}

bool WebPopupFloatItem::isInitialized() const
{
    return m_popupItem;
}

QStringList WebPopupFloatItem::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString WebPopupFloatItem::renderPolicy() const
{
    return "ALWAYS";
}

bool WebPopupFloatItem::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString&, GeoSceneLayer* )
{
    if ( m_popupItem && visible() && m_popupItem->visible() ) {
        m_popupItem->paintEvent( painter, viewport );
    }
    return true;
}

bool WebPopupFloatItem::eventFilter( QObject *object, QEvent *e )
{
    return m_popupItem && visible() && m_popupItem->eventFilter( object, e );
}

qreal WebPopupFloatItem::zValue() const
{
    return 4711.23;
}

void WebPopupFloatItem::setCoordinates(const GeoDataCoordinates &coordinates , Qt::Alignment alignment)
{
    /** @todo respect alignment */
    Q_UNUSED( alignment );

    if ( m_popupItem ) {
        m_popupItem->setCoordinate( coordinates );
    }
}

void WebPopupFloatItem::setUrl( const QUrl &url )
{
    if ( m_popupItem ) {
        m_popupItem->setUrl( url );
    }
}

void WebPopupFloatItem::setContent( const QString &html )
{
    if ( m_popupItem ) {
        m_popupItem->setContent( html );
    }
}

void WebPopupFloatItem::setSize( const QSizeF &size )
{
    if ( m_popupItem ) {
        m_popupItem->setSize( size );
    }
}

void WebPopupFloatItem::setPosition( const QPointF &position )
{
    /** @todo Implement */
    Q_UNUSED( position );
}

void WebPopupFloatItem::hidePopupItem()
{
    setVisible( false );
}

}

Q_EXPORT_PLUGIN2( WebPopupFloatItem, Marble::WebPopupFloatItem )

#include "WebPopupFloatItem.moc"
