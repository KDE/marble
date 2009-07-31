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
    setPadding( 4 );
}

void LabelGraphicsItem::setText( const QString& text )
{
    d->m_text = text;
    QFontMetrics metrics( d->font() );
    setContentSize( metrics.boundingRect( text ).size() );
}

void LabelGraphicsItem::clear()
{
    d->m_text.clear();
    setSize( QSizeF( 0.0, 0.0 ) );
}

void LabelGraphicsItem::paintContent( GeoPainter *painter, ViewportParams *viewport,
                              const QString& renderPos, GeoSceneLayer * layer = 0 )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();
    painter->setFont( d->font() );
    painter->drawText( contentRect().toRect(),
                       Qt::AlignVCenter | Qt::AlignLeft,
                       d->m_text );
    painter->restore();
}
