//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataStyle.h"

GeoDataStyle::GeoDataStyle()
  : m_iconStyle( 0 ),
    m_labelStyle( 0 )
{
}

GeoDataStyle::GeoDataStyle( const QPixmap& icon, 
                            const QFont &font, const QColor &color  )
  : m_iconStyle( new GeoDataIconStyle( icon ) ),
    m_labelStyle( new GeoDataLabelStyle( font, color ) )
{
}

GeoDataStyle::~GeoDataStyle()
{
    delete m_labelStyle;
    delete m_iconStyle;
}

GeoDataIconStyle* GeoDataStyle::iconStyle()
{
    return m_iconStyle;
}

GeoDataLabelStyle* GeoDataStyle::labelStyle()
{
    return m_labelStyle;
}

void GeoDataStyle::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );

    m_iconStyle->pack( stream );
    m_labelStyle->pack( stream );
}

void GeoDataStyle::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );

    m_iconStyle->unpack( stream );
    m_labelStyle->unpack( stream );
}
