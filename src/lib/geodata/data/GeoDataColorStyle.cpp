//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataColorStyle.h"

GeoDataColorStyle::GeoDataColorStyle() :
    m_color( Qt::black )
{
}

void GeoDataColorStyle::setColor( const QColor &value )
{
    m_color = value;
}

QColor GeoDataColorStyle::color() const
{
    return m_color;
}

void GeoDataColorStyle::setColorMode( const ColorMode &colorMode )
{
    m_colorMode = colorMode;
}

GeoDataColorStyle::ColorMode GeoDataColorStyle::colorMode() const
{
    return m_colorMode;
}

void GeoDataColorStyle::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << m_color;
}

void GeoDataColorStyle::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> m_color;
}
