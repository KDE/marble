//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLLabelStyle.h"

KMLLabelStyle::KMLLabelStyle()
{
}

void KMLLabelStyle::pack( QDataStream& stream ) const
{
    KMLColorStyle::pack( stream );
}

void KMLLabelStyle::unpack( QDataStream& stream )
{
    KMLColorStyle::unpack( stream );
}
