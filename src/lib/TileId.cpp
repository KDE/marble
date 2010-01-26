//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Jens-Michael Hoffmann  <jensmh@gmx.de>
//

// Own
#include "TileId.h"

namespace Marble
{

TileId::TileId( int zoomLevel, int tileX, int tileY )
    : m_zoomLevel( zoomLevel ), m_tileX( tileX ), m_tileY( tileY )
{
}

TileId::TileId()
    : m_zoomLevel( 0 ), m_tileX( 0 ), m_tileY( 0 )
{
}

TileId TileId::fromString( QString const& idStr )
{
    int first = idStr.indexOf( ':' );
    int last = idStr.lastIndexOf( ':' );

    return TileId( idStr.left( first ).toInt(),
                   idStr.mid( first + 1, last - first - 1 ).toInt(),
                   idStr.mid( last + 1 ).toInt() );
}


}
