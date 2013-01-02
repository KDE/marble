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

#include "MapInfoDialog.h"
#include "MarbleWidget.h"
#include "PopupItem.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QAction>

namespace Marble
{

MapInfoDialog::MapInfoDialog(QObject *parent) :
    QObject( parent ),
    m_popupItem( new PopupItem )
{
    connect( m_popupItem, SIGNAL(dirty()), this, SIGNAL(repaintNeeded()) );
    connect( m_popupItem, SIGNAL(hide()), this, SLOT(hidePopupItem()) );
}

MapInfoDialog::~MapInfoDialog()
{
}

QStringList MapInfoDialog::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString MapInfoDialog::renderPolicy() const
{
    return "ALWAYS";
}

bool MapInfoDialog::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString&, GeoSceneLayer* )
{
    if ( visible() ) {
        m_popupItem->paintEvent( painter, viewport );
    }
    return true;
}

bool MapInfoDialog::eventFilter( QObject *object, QEvent *e )
{
    return m_popupItem && visible() && m_popupItem->eventFilter( object, e );
}

qreal MapInfoDialog::zValue() const
{
    return 4711.23;
}

bool MapInfoDialog::visible() const
{
    return m_popupItem->visible();
}

void MapInfoDialog::setVisible( bool visible )
{
    m_popupItem->setVisible( visible );
}

void MapInfoDialog::setCoordinates(const GeoDataCoordinates &coordinates , Qt::Alignment alignment)
{
    if ( m_popupItem ) {
        m_popupItem->setCoordinate( coordinates );
        m_popupItem->setAlignment( alignment );
    }
}

void MapInfoDialog::setUrl( const QUrl &url )
{
    if ( m_popupItem ) {
        m_popupItem->setUrl( url );
    }
}

void MapInfoDialog::setContent( const QString &html )
{
    if ( m_popupItem ) {
        m_popupItem->setContent( html );
        emit repaintNeeded();
    }
}

void MapInfoDialog::setBackgroundColor(const QColor &color)
{
    if(color.isValid()) {
        m_popupItem->setBackgroundColor(color);
    }
}

void MapInfoDialog::setTextColor(const QColor &color)
{
    if(color.isValid()) {
        m_popupItem->setTextColor(color);
    }
}

void MapInfoDialog::setSize( const QSizeF &size )
{
    if ( m_popupItem ) {
        m_popupItem->setSize( size );
    }
}

void MapInfoDialog::setPosition( const QPointF &position )
{
    /** @todo Implement */
    Q_UNUSED( position );
}

void MapInfoDialog::hidePopupItem()
{
    setVisible( false );
}

}

#include "MapInfoDialog.moc"
