//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoDataAccuracy.h"

namespace Marble {

GeoDataAccuracy::GeoDataAccuracy( Level level, qreal horizontal, qreal _vertical )
    : level( level ),
      horizontal( horizontal ),
      vertical( _vertical )
{
       // nothing to do
}

bool GeoDataAccuracy::operator==( const GeoDataAccuracy &other ) const
{
    return ( level == other.level ) && ( horizontal == other.horizontal ) && ( vertical == other.vertical );
}

}
