//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008, 2010 Jens-Michael Hoffmann <jensmh@gmx.de>
//

// Own
#include "TileId.h"

#include <QtCore/QStringList>

namespace Marble
{

TileId::TileId( QString const & mapThemeId, int zoomLevel, int tileX, int tileY )
    : m_mapThemeIdHash( qHash( mapThemeId )), m_zoomLevel( zoomLevel ), m_tileX( tileX ), m_tileY( tileY )
{
}

TileId::TileId( uint mapThemeIdHash, int zoomLevel, int tileX, int tileY )
    : m_mapThemeIdHash( mapThemeIdHash ), m_zoomLevel( zoomLevel ), m_tileX( tileX ), m_tileY( tileY )
{
}

TileId::TileId()
    : m_mapThemeIdHash( 0 ), m_zoomLevel( 0 ), m_tileX( 0 ), m_tileY( 0 )
{
}

TileId TileId::fromString( QString const& idStr )
{
    QStringList const components = idStr.split( ':', QString::SkipEmptyParts );
    Q_ASSERT( components.size() == 4 );

    uint const mapThemeIdHash = components[ 0 ].toUInt();
    int const zoomLevel = components[ 1 ].toInt();
    int const tileX = components[ 2 ].toInt();
    int const tileY = components[ 3 ].toInt();
    return TileId( mapThemeIdHash, zoomLevel, tileX, tileY );
}

}
