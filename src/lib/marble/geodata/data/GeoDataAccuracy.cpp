// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
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

bool GeoDataAccuracy::operator!=( const GeoDataAccuracy &other ) const
{
    return !this->operator==(other);
}

}
