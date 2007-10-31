//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataLabelStyle.h"

GeoDataLabelStyle::GeoDataLabelStyle()
{
}

GeoDataLabelStyle::GeoDataLabelStyle( const QFont &font, const QColor &color )
  : m_font( font ),
    m_scale( 1.0 )
{
    setColor( color );
}

void GeoDataLabelStyle::setScale( const float &scale )
{
    m_scale = scale;
}

float GeoDataLabelStyle::scale() const
{
    return m_scale;
}

void GeoDataLabelStyle::setFont( const QFont &font )
{
    m_font = font;
}

QFont GeoDataLabelStyle::font() const
{
    return m_font;
}


void GeoDataLabelStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );
}

void GeoDataLabelStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );
}
