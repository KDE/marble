//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataIconStyle.h"

GeoDataIconStyle::GeoDataIconStyle()
{
}

void GeoDataIconStyle::setIcon( const QPixmap &value )
{
    m_pixmap = value;
}

QPixmap GeoDataIconStyle::icon() const
{
    return m_pixmap;
}

void GeoDataIconStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << m_pixmap;
}

void GeoDataIconStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );

    stream >> m_pixmap;
}
