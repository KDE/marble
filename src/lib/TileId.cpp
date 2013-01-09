//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008, 2010 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

// Own
#include "TileId.h"
#include "GeoDataCoordinates.h"

#include <QtCore/QDebug>
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

TileId TileId::fromCoordinates(const GeoDataCoordinates &coords, int zoomLevel)
{
    if ( zoomLevel < 0 ) {
        return TileId();
    }
    const int maxLat = 90000000;
    const int maxLon = 180000000;
    int lat = GeoDataCoordinates::normalizeLat( coords.latitude(), GeoDataCoordinates::Degree ) * 1000000;
    int lon = GeoDataCoordinates::normalizeLon( coords.longitude(), GeoDataCoordinates::Degree ) * 1000000;
    int x = 0;
    int y = 0;
    for( int i=0; i<zoomLevel; ++i ) {
        const int deltaLat = maxLat >> i;
        if( lat <= ( maxLat - deltaLat )) {
            y += 1<<(zoomLevel-i-1);
            lat += deltaLat;
        }
        const int deltaLon = maxLon >> i;
        if( lon >= ( maxLon - deltaLon )) {
            x += 1<<(zoomLevel-i-1);
        } else {
            lon += deltaLon;
        }
    }
    return TileId(0, zoomLevel, x, y);
}

}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<( QDebug dbg, const Marble::TileId &id )
{
    return dbg << QString( "Marble::TileId(%1, %2, %3, %4)" ).arg( id.mapThemeIdHash() )
                                                             .arg( id.zoomLevel() )
                                                             .arg( id.x() )
                                                             .arg( id.y() );
}
#endif
