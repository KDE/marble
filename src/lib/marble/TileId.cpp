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

#include "MarbleMath.h"

#include <QDebug>

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

GeoDataLatLonBox TileId::toLatLonBox( const GeoSceneTileDataset *textureLayer ) const
{

    qreal radius = ( 1 << zoomLevel() ) * textureLayer->levelZeroColumns() / 2.0;

    qreal lonLeft   = ( x() - radius ) / radius * M_PI;
    qreal lonRight  = ( x() - radius + 1 ) / radius * M_PI;

    radius = ( 1 << zoomLevel() ) * textureLayer->levelZeroRows() / 2.0;
    qreal latTop = 0;
    qreal latBottom = 0;

    switch ( textureLayer->projection() ) {
    case GeoSceneTileDataset::Equirectangular:
        latTop = ( radius - y() ) / radius *  M_PI / 2.0;
        latBottom = ( radius - y() - 1 ) / radius *  M_PI / 2.0;
        break;
    case GeoSceneTileDataset::Mercator:
        latTop = atan( sinh( ( radius - y() ) / radius * M_PI ) );
        latBottom = atan( sinh( ( radius - y() - 1 ) / radius * M_PI ) );
        break;
    }

    return GeoDataLatLonBox( latTop, latBottom, lonRight, lonLeft );
}

TileId TileId::fromCoordinates(const GeoDataCoordinates &coords, int zoomLevel)
{
    if ( zoomLevel < 0 ) {
        return TileId();
    }
    const int maxLat = 90 * 1000000;
    const int maxLon = 180 * 1000000;
    int lat = GeoDataCoordinates::normalizeLat( coords.latitude( GeoDataCoordinates::Degree ), GeoDataCoordinates::Degree ) * 1000000;
    int lon = GeoDataCoordinates::normalizeLon( coords.longitude( GeoDataCoordinates::Degree ), GeoDataCoordinates::Degree ) * 1000000;
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

unsigned int TileId::lon2tileX( qreal lon, unsigned int maxTileX )
{
    return (unsigned int)floor(0.5 * (lon / M_PI + 1.0) * maxTileX);
}

unsigned int TileId::lat2tileY( qreal latitude, unsigned int maxTileY )
{
    // We need to calculate the tile position from the latitude
    // projected using the Mercator projection. This requires the inverse Gudermannian
    // function which is only defined between -85°S and 85°N. Therefore in order to
    // prevent undefined results we need to restrict our calculation:
    qreal maxAbsLat = 85.0 * DEG2RAD;
    qreal lat = (qAbs(latitude) > maxAbsLat) ? latitude/qAbs(latitude) * maxAbsLat : latitude;
    return (unsigned int)floor(0.5 * (1.0 - gdInv(lat) / M_PI) * maxTileY);
}


qreal TileId::tileX2lon( unsigned int x, unsigned int maxTileX )
{
    return ( (2*M_PI * x) / maxTileX - M_PI );
}

qreal TileId::tileY2lat( unsigned int y, unsigned int maxTileY )
{
    return gd(M_PI * (1.0 - (2.0 * y) / maxTileY));
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
