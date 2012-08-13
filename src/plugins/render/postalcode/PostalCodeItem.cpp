//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

// Self
#include "PostalCodeItem.h"

// Marble
#include "ViewportParams.h"

// Qt
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

using namespace Marble;

const QFont PostalCodeItem::s_font = QFont( "Sans Serif", 10, QFont::Bold );
const int PostalCodeItem::s_labelOutlineWidth = 5;

PostalCodeItem::PostalCodeItem( QObject *parent )
    : AbstractDataPluginItem( parent )
{
    setSize( QSize( 0, 0 ) );
    setCacheMode( ItemCoordinateCache );
}

PostalCodeItem::~PostalCodeItem()
{
}

QString PostalCodeItem::itemType() const
{
    return "postalCodeItem";
}

bool PostalCodeItem::initialized()
{
    return !m_text.isEmpty();
}

bool PostalCodeItem::operator<( const AbstractDataPluginItem *other ) const
{
    return this->id() < other->id();
}

QString PostalCodeItem::text() const
{
    return m_text;
}

void PostalCodeItem::setText( const QString& text )
{
    QFontMetrics metrics( s_font );
    setSize( metrics.size( 0, text ) + QSize( 10, 10 ) );
    m_text = text;
}

void PostalCodeItem::paint( QPainter *painter )
{
    painter->save();

    const int fontAscent = QFontMetrics( s_font ).ascent();

    QPen outlinepen( Qt::white );
    outlinepen.setWidthF( s_labelOutlineWidth );
    QBrush  outlinebrush( Qt::black );

    const QPointF baseline( s_labelOutlineWidth / 2.0, fontAscent );

    QPainterPath outlinepath;
    outlinepath.addText( baseline, s_font, m_text );

    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setPen( outlinepen );
    painter->setBrush( outlinebrush );
    painter->drawPath( outlinepath );
    painter->setPen( Qt::NoPen );
    painter->drawPath( outlinepath );
    painter->setRenderHint( QPainter::Antialiasing, false );

    painter->restore();
}

#include "PostalCodeItem.moc"
