//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "LabelGraphicsItem.h"
#include "LabelGraphicsItem_p.h"

// Marble
#include "GeoPainter.h"

// Qt
#include <QtGui/QApplication>
#include "MarbleDebug.h"
#include <QtGui/QFont>
#include <QtCore/QString>

using namespace Marble;

LabelGraphicsItemPrivate::LabelGraphicsItemPrivate( LabelGraphicsItem *parent )
    : m_text(),
      m_parent( parent )
{
}

QFont LabelGraphicsItemPrivate::font() const
{
    return QApplication::font();
}

void LabelGraphicsItemPrivate::updateSize()
{
    QSizeF updatedSize = m_calculatedSize;
    if ( updatedSize.isEmpty() ) {
        updatedSize.setHeight( 0 );
        updatedSize.setWidth( 0 );
    }
    else {
        if ( m_minimumSize.width() > updatedSize.width() ) {
            updatedSize.setWidth( m_minimumSize.width() );
        }
        if ( m_minimumSize.height() > updatedSize.height() ) {
            updatedSize.setHeight( m_minimumSize.height() );
        }
    }

    m_parent->setContentSize( updatedSize );
}

// ----------------------------------------------------------------

LabelGraphicsItem::LabelGraphicsItem( MarbleGraphicsItem *parent )
    : FrameGraphicsItem( parent ),
      d( new LabelGraphicsItemPrivate( this ) )
{
}

LabelGraphicsItem::~LabelGraphicsItem()
{
    delete d;
}

QString LabelGraphicsItem::text() const
{
    return d->m_text;
}

void LabelGraphicsItem::setText( const QString& text )
{
    clear();
    d->m_text = text;
    QFontMetrics metrics( d->font() );
    QSizeF size = metrics.boundingRect( text ).size() + QSizeF( 14, 2 );
    d->m_calculatedSize = size;
    d->updateSize();

    update();
}

QImage LabelGraphicsItem::image() const
{
    return d->m_image;
}

void LabelGraphicsItem::setImage( const QImage& image, const QSize& size )
{
    clear();
    d->m_image = image;
    if ( size.isEmpty() ) {
        d->m_calculatedSize = image.size();
    }
    else {
        d->m_calculatedSize = size;
    }
    d->updateSize();

    update();
}

QIcon LabelGraphicsItem::icon() const
{
    return d->m_icon;
}

void LabelGraphicsItem::setIcon( const QIcon& icon, const QSize& size )
{
    clear();
    d->m_icon = icon;
    d->m_calculatedSize = size;
    d->updateSize();

    update();
}

QSizeF LabelGraphicsItem::minimumSize() const
{
    return d->m_minimumSize;
}

void LabelGraphicsItem::setMinimumSize( const QSizeF& size )
{
    d->m_minimumSize = size;
    d->updateSize();

    update();
}

void LabelGraphicsItem::clear()
{
    d->m_text.clear();
    d->m_image = QImage();
    d->m_icon = QIcon();
    d->m_calculatedSize = QSizeF( 0.0, 0.0 );
    d->updateSize();

    update();
}

void LabelGraphicsItem::paintContent( GeoPainter *painter, ViewportParams *viewport,
                              const QString& renderPos, GeoSceneLayer * layer = 0 )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();

    if ( !d->m_text.isNull() ) {
        painter->setFont( d->font() );
        painter->setPen( QColor( Qt::black ) );
        painter->drawText( QRect( QPoint( 0, 0 ), contentSize().toSize() ),
                           Qt::AlignVCenter | Qt::AlignLeft,
                           d->m_text );
    }
    else if ( !d->m_image.isNull() ) {
        painter->drawImage( QRectF( QPointF( 0, 0 ), contentSize() ),
                            d->m_image );
    }
    else if ( !d->m_icon.isNull() ) {
        d->m_icon.paint( painter,
                         QRect( QPoint( 0, 0 ), contentSize().toSize() ),
                         Qt::AlignCenter );
    }

    painter->restore();
}
