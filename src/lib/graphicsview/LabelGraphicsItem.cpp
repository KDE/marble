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
#include "LabelGraphicsItem.h"
#include "LabelGraphicsItem_p.h"

// Marble
#include "GeoPainter.h"

// Qt
#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtGui/QFont>
#include <QtCore/QString>

using namespace Marble;

LabelGraphicsItemPrivate::LabelGraphicsItemPrivate()
    : m_text()
{
}

QFont LabelGraphicsItemPrivate::font() const
{
    return QApplication::font();
}

// ----------------------------------------------------------------

LabelGraphicsItem::LabelGraphicsItem( MarbleGraphicsItem *parent )
    : FrameGraphicsItem( parent ),
      d( new LabelGraphicsItemPrivate() )
{
}

void LabelGraphicsItem::setText( const QString& text, int minWidth, int minHeight )
{
    clear();
    d->m_text = text;
    QFontMetrics metrics( d->font() );
    QSizeF size = metrics.boundingRect( text ).size() + QSizeF( 14, 2 );
    if ( size.width() < minWidth )
        size.setWidth( minWidth );
    if ( size.height() < minHeight )
        size.setHeight( minHeight );
    setContentSize( size );

    update();
}

void LabelGraphicsItem::setImage( const QImage& image, const QSize& size )
{
    clear();
    d->m_image = image;
    if ( size.isEmpty() ) {
        setContentSize( image.size() );
    }
    else {
        setContentSize( size );
    }

    update();
}

void LabelGraphicsItem::setIcon( const QIcon& icon, const QSize& size )
{
    clear();
    d->m_icon = icon;
    setContentSize( size );

    update();
}

void LabelGraphicsItem::clear()
{
    d->m_text.clear();
    d->m_image = QImage();
    d->m_icon = QIcon();
    setSize( QSizeF( 0.0, 0.0 ) );

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
        painter->drawText( contentRect().toRect(),
                           Qt::AlignVCenter | Qt::AlignLeft,
                           d->m_text );
    }
    else if ( !d->m_image.isNull() ) {
        painter->drawImage( contentRect(),
                            d->m_image );
    }
    else if ( !d->m_icon.isNull() ) {
        d->m_icon.paint( painter, contentRect().toRect(), Qt::AlignCenter );
    }

    painter->restore();
}
