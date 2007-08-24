//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLStyle.h"

KMLStyle::KMLStyle()
{
}

KMLIconStyle& KMLStyle::getIconStyle()
{
    return m_iconStyle;
}

void KMLStyle::pack( QDataStream& stream ) const
{
    KMLStyleSelector::pack( stream );

    m_iconStyle.pack( stream );
    m_labelStyle.pack( stream );
}

void KMLStyle::unpack( QDataStream& stream )
{
    KMLStyleSelector::unpack( stream );

    m_iconStyle.unpack( stream );
    m_labelStyle.unpack( stream );
}
