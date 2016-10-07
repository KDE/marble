//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "DownloadRegion.h"

#include "MarbleModel.h"
#include "MarbleMap.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "TextureLayer.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineString.h"
#include "TileCoordsPyramid.h"

namespace Marble {

class DownloadRegionPrivate
{
public:
    MarbleModel* m_marbleModel;

    QPair<int,int> m_tileLevelRange;

    int m_visibleTileLevel;

    DownloadRegionPrivate();

    int rad2PixelX( qreal const lon, const TextureLayer *textureLayer ) const;

    int rad2PixelY( qreal const lat, const TextureLayer *textureLayer ) const;
};

DownloadRegionPrivate::DownloadRegionPrivate() : m_marbleModel( 0 ),
    m_tileLevelRange( 0, 0 ), m_visibleTileLevel( 0 )
{
    // nothing to do
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionPrivate::rad2PixelX( qreal const lon, const TextureLayer *textureLayer ) const
{
    qreal const globalWidth = textureLayer->tileSize().width()
            * textureLayer->tileColumnCount( m_visibleTileLevel );
    return static_cast<int>( globalWidth * 0.5 + lon * ( globalWidth / ( 2.0 * M_PI ) ) );
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionPrivate::rad2PixelY( qreal const lat, const TextureLayer *textureLayer ) const
{
    qreal const globalHeight = textureLayer->tileSize().height()
            * textureLayer->tileRowCount( m_visibleTileLevel );
    qreal const normGlobalHeight = globalHeight / M_PI;
    switch (textureLayer->tileProjectionType()) {
    case GeoSceneAbstractTileProjection::Equirectangular:
        return static_cast<int>( globalHeight * 0.5 - lat * normGlobalHeight );
    case GeoSceneAbstractTileProjection::Mercator:
        if ( fabs( lat ) < 1.4835 )
            return static_cast<int>( globalHeight * 0.5 - gdInv( lat ) * 0.5 * normGlobalHeight );
        if ( lat >= +1.4835 )
            return static_cast<int>( globalHeight * 0.5 - 3.1309587 * 0.5 * normGlobalHeight );
        if ( lat <= -1.4835 )
            return static_cast<int>( globalHeight * 0.5 + 3.1309587 * 0.5 * normGlobalHeight );
    }

    // Dummy value to avoid a warning.
    return 0;
}

DownloadRegion::DownloadRegion( QObject* parent ) : QObject( parent ),
    d( new DownloadRegionPrivate )
{
    // nothing to do
}

void DownloadRegion::setMarbleModel( MarbleModel* model )
{
    d->m_marbleModel = model;
}

DownloadRegion::~DownloadRegion()
{
    delete d;
}

void DownloadRegion::setTileLevelRange( const int minimumTileLevel, const int maximumTileLevel )
{
    Q_ASSERT( minimumTileLevel >= 0 );
    Q_ASSERT( maximumTileLevel >= 0 );
    Q_ASSERT( minimumTileLevel <= maximumTileLevel );
    d->m_tileLevelRange.first = minimumTileLevel;
    d->m_tileLevelRange.second = maximumTileLevel;
}

QVector<TileCoordsPyramid> DownloadRegion::region( const TextureLayer *textureLayer, const GeoDataLatLonAltBox &downloadRegion ) const
{
    Q_ASSERT( textureLayer );
    int const westX = d->rad2PixelX( downloadRegion.west(), textureLayer );
    int const northY = d->rad2PixelY( downloadRegion.north(), textureLayer );
    int const eastX = d->rad2PixelX( downloadRegion.east(), textureLayer );
    int const southY = d->rad2PixelY( downloadRegion.south(), textureLayer );

    // FIXME: remove this stuff
    mDebug() << "DownloadRegionDialog downloadRegion:"
             << "north:" << downloadRegion.north()
             << "south:" << downloadRegion.south()
             << "east:" << downloadRegion.east()
             << "west:" << downloadRegion.west();
    mDebug() << "north/west (x/y):" << westX << northY;
    mDebug() << "south/east (x/y):" << eastX << southY;

    int const tileWidth = textureLayer->tileSize().width();
    int const tileHeight = textureLayer->tileSize().height();
    mDebug() << "DownloadRegionDialog downloadRegion: tileSize:" << tileWidth << tileHeight;

    int const visibleLevelX1 = qMin( westX, eastX );
    int const visibleLevelY1 = qMin( northY, southY );
    int const visibleLevelX2 = qMax( westX, eastX );
    int const visibleLevelY2 = qMax( northY, southY );

    mDebug() << "visible level pixel coords (level/x1/y1/x2/y2):" << d->m_visibleTileLevel
             << visibleLevelX1 << visibleLevelY1 << visibleLevelX2 << visibleLevelY2;

    int bottomLevelX1, bottomLevelY1, bottomLevelX2, bottomLevelY2;
    // the pixel coords calculated above are referring to the visible tile level,
    // if the bottom level is a different level, we have to take it into account
    if ( d->m_visibleTileLevel > d->m_tileLevelRange.second ) {
        int const deltaLevel = d->m_visibleTileLevel - d->m_tileLevelRange.second;
        bottomLevelX1 = visibleLevelX1 >> deltaLevel;
        bottomLevelY1 = visibleLevelY1 >> deltaLevel;
        bottomLevelX2 = visibleLevelX2 >> deltaLevel;
        bottomLevelY2 = visibleLevelY2 >> deltaLevel;
    }
    else if ( d->m_visibleTileLevel < d->m_tileLevelRange.second ) {
        int const deltaLevel = d->m_tileLevelRange.second - d->m_visibleTileLevel;
        bottomLevelX1 = visibleLevelX1 << deltaLevel;
        bottomLevelY1 = visibleLevelY1 << deltaLevel;
        bottomLevelX2 = visibleLevelX2 << deltaLevel;
        bottomLevelY2 = visibleLevelY2 << deltaLevel;
    }
    else {
        bottomLevelX1 = visibleLevelX1;
        bottomLevelY1 = visibleLevelY1;
        bottomLevelX2 = visibleLevelX2;
        bottomLevelY2 = visibleLevelY2;
    }
    mDebug() << "bottom level pixel coords (level/x1/y1/x2/y2):"
             << d->m_tileLevelRange.second
             << bottomLevelX1 << bottomLevelY1 << bottomLevelX2 << bottomLevelY2;

    TileCoordsPyramid coordsPyramid( d->m_tileLevelRange.first, d->m_tileLevelRange.second );
    QRect bottomLevelTileCoords;
    bottomLevelTileCoords.setCoords
            ( bottomLevelX1 / tileWidth,
              bottomLevelY1 / tileHeight,
              bottomLevelX2 / tileWidth + ( bottomLevelX2 % tileWidth > 0 ? 1 : 0 ),
              bottomLevelY2 / tileHeight + ( bottomLevelY2 % tileHeight > 0 ? 1 : 0 ));
    mDebug() << "bottom level tile coords: (x1/y1/size):" << bottomLevelTileCoords;
    coordsPyramid.setBottomLevelCoords( bottomLevelTileCoords );
    mDebug() << "tiles count:" << coordsPyramid.tilesCount( );
    QVector<TileCoordsPyramid> pyramid;
    pyramid << coordsPyramid;
    return pyramid;
}

void DownloadRegion::setVisibleTileLevel(const int tileLevel)
{
    d->m_visibleTileLevel = tileLevel;
}

QVector<TileCoordsPyramid> DownloadRegion::fromPath( const TextureLayer *textureLayer, qreal offset, const GeoDataLineString &waypoints ) const
{
    if ( !d->m_marbleModel ) {
        return QVector<TileCoordsPyramid>();
    }

    int const topLevel = d->m_tileLevelRange.first;
    int const bottomLevel = d->m_tileLevelRange.second;
    TileCoordsPyramid coordsPyramid( topLevel, bottomLevel );

    int const tileWidth = textureLayer->tileSize().width();
    int const tileHeight = textureLayer->tileSize().height();

    qreal radius = d->m_marbleModel->planetRadius();
    QVector<TileCoordsPyramid> pyramid;
    qreal radianOffset = offset / radius;

    for( int i = 1; i < waypoints.size(); ++i ) {
        GeoDataCoordinates position = waypoints[i];
        qreal lonCenter = position.longitude();
        qreal latCenter = position.latitude();

        // coordinates of the of the vertices of the square(topleft and bottomright) at an offset distance from the waypoint
        qreal latNorth = asin( sin( latCenter ) *  cos( radianOffset ) +  cos( latCenter ) * sin( radianOffset )  * cos( 7*M_PI/4 ) );
        qreal dlonWest = atan2( sin( 7*M_PI/4 ) * sin( radianOffset ) * cos( latCenter ),  cos( radianOffset ) -  sin( latCenter ) * sin( latNorth ) );
        qreal lonWest  = fmod( lonCenter - dlonWest + M_PI, 2*M_PI ) - M_PI;
        qreal latSouth = asin( sin( latCenter ) * cos( radianOffset ) + cos( latCenter ) * sin( radianOffset ) * cos( 3*M_PI/4 ) );
        qreal dlonEast =  atan2( sin( 3*M_PI/4 ) * sin( radianOffset ) * cos( latCenter ),  cos( radianOffset ) -  sin( latCenter ) * sin( latSouth ) );
        qreal lonEast  = fmod( lonCenter - dlonEast+M_PI, 2*M_PI ) - M_PI;

        int const northY = d->rad2PixelY( latNorth, textureLayer );
        int const southY = d->rad2PixelY( latSouth, textureLayer );
        int const eastX =  d->rad2PixelX( lonEast, textureLayer );
        int const westX =  d->rad2PixelX( lonWest, textureLayer );

        int const west  = qMin( westX, eastX );
        int const north = qMin( northY, southY );
        int const east  = qMax( westX, eastX );
        int const south = qMax( northY, southY );

        int bottomLevelTileX1 = 0;
        int bottomLevelTileY1 = 0;
        int bottomLevelTileX2 = 0;
        int bottomLevelTileY2 = 0;

        if ( d->m_visibleTileLevel > d->m_tileLevelRange.second ) {
            int const deltaLevel = d->m_visibleTileLevel - d->m_tileLevelRange.second;
            bottomLevelTileX1 = west  >> deltaLevel;
            bottomLevelTileY1 = north >> deltaLevel;
            bottomLevelTileX2 = east  >> deltaLevel;
            bottomLevelTileY2 = south >> deltaLevel;
        }
        else if ( d->m_visibleTileLevel < bottomLevel ) {
            int const deltaLevel = bottomLevel - d->m_visibleTileLevel;
            bottomLevelTileX1 = west  << deltaLevel;
            bottomLevelTileY1 = north << deltaLevel;
            bottomLevelTileX2 = east  << deltaLevel;
            bottomLevelTileY2 = south << deltaLevel;
        }
        else {
            bottomLevelTileX1 = west;
            bottomLevelTileY1 = north;
            bottomLevelTileX2 = east;
            bottomLevelTileY2 = south;
        }

        QRect waypointRegion;
        //square region around the waypoint
        waypointRegion.setCoords( bottomLevelTileX1/tileWidth, bottomLevelTileY1/tileHeight,
                                  bottomLevelTileX2/tileWidth, bottomLevelTileY2/tileHeight );
        coordsPyramid.setBottomLevelCoords( waypointRegion );
        pyramid << coordsPyramid;
    }

    return pyramid;
}

}

#include "moc_DownloadRegion.cpp"
