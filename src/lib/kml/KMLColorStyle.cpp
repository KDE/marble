//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLColorStyle.h"

KMLColorStyle::KMLColorStyle() :
    m_color( Qt::black )
{
}

void KMLColorStyle::setColor( QColor value )
{
    m_color = value;
}

QColor KMLColorStyle::color() const
{
    return m_color;
}

void KMLColorStyle::pack( QDataStream& stream ) const
{
    KMLObject::pack( stream );

    stream << m_color;
}

void KMLColorStyle::unpack( QDataStream& stream )
{
    KMLObject::unpack( stream );

    stream >> m_color;
}
