//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLIconStyle.h"

KMLIconStyle::KMLIconStyle()
{
}

void KMLIconStyle::setIcon( QPixmap value )
{
    m_pixmap = value;
}

QPixmap KMLIconStyle::icon() const
{
    return m_pixmap;
}

void KMLIconStyle::pack( QDataStream& stream ) const
{
    KMLColorStyle::pack( stream );

    stream << m_pixmap;
}

void KMLIconStyle::unpack( QDataStream& stream )
{
    KMLColorStyle::unpack( stream );

    stream >> m_pixmap;
}
