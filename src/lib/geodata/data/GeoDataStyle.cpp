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
{
}

GeoDataIconStyle& GeoDataStyle::getIconStyle()
{
    return m_iconStyle;
}

void GeoDataStyle::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );

    m_iconStyle.pack( stream );
    m_labelStyle.pack( stream );
}

void GeoDataStyle::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );

    m_iconStyle.unpack( stream );
    m_labelStyle.unpack( stream );
}
