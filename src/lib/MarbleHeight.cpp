//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MarbleHeight.h"
#include "MarbleHeight.moc"

#include "MapThemeManager.h"
#include "TextureTile.h"
#include "TileLoaderHelper.h"
#include "global.h"
#include "MarbleDebug.h"

#include <GeoSceneDocument.h>
#include <GeoSceneHead.h>
#include <GeoSceneMap.h>
#include <GeoSceneLayer.h>
#include <GeoSceneTexture.h>
#include "TileLoader.h"

namespace Marble
{

const GeoSceneTexture* MarbleHeight::srtmLayer()
{
    const GeoSceneDocument *srtmTheme = MapThemeManager::loadMapTheme( "earth/srtm/srtm.dgml" );
    Q_ASSERT( srtmTheme );

    const GeoSceneHead *head = srtmTheme->head();
    Q_ASSERT( head );

    const GeoSceneMap *map = srtmTheme->map();
    Q_ASSERT( map );

    const GeoSceneLayer *sceneLayer = map->layer( head->theme() );
    Q_ASSERT( sceneLayer );

    const GeoSceneTexture *textureLayer = dynamic_cast<GeoSceneTexture*>( sceneLayer->datasets().first() );
    Q_ASSERT( textureLayer );

    return textureLayer;
}

MarbleHeight::MarbleHeight( QObject* parent )
    : QObject( parent )
    , m_textureLayer( srtmLayer() )
    , m_hash( qHash( m_textureLayer->sourceDir() ) )
    , m_tileLoader( 0 )
    , m_level( 0 )
    , m_numXTiles( 1 )
    , m_numYTiles( 1 )
{
}


void MarbleHeight::setTileLoader( TileLoader *tileLoader )
{
    m_tileLoader = tileLoader;
}


void MarbleHeight::setRadius( int radius )
{
    m_radius = radius;
    m_level = 0;

    m_numXTiles = TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(), m_level );
    m_numYTiles = TileLoaderHelper::levelToRow( m_textureLayer->levelZeroRows(), m_level );

    while ( m_numXTiles * m_textureLayer->tileSize().width() < radius * 2 && m_level < m_textureLayer->maximumTileLevel()) {
        ++m_level;
        m_numXTiles = TileLoaderHelper::levelToColumn( m_textureLayer->levelZeroColumns(), m_level );
        m_numYTiles = TileLoaderHelper::levelToRow( m_textureLayer->levelZeroRows(), m_level );
    };
}


qreal MarbleHeight::altitude( qreal lon, qreal lat )
{
    if ( m_tileLoader == 0 )
        return 0.0;

    const int width = m_textureLayer->tileSize().width();
    const int height = m_textureLayer->tileSize().height();

    const qreal textureX = ( 0.5 + 0.5 * lon / M_PI ) * m_numXTiles * width;
    const qreal textureY = ( 0.5 -       lat / M_PI ) * m_numYTiles * height;

    qreal color = 0;

    for ( int i = 0; i < 4; ++i ) {
        const int x = ( static_cast<int>( textureX ) + ( i % 2 ) );
        const int y = ( static_cast<int>( textureY ) + ( i / 2 ) );

        const TileId id( m_hash, m_level, ( x  % ( m_numXTiles * width ) ) / width, ( y % ( m_numYTiles * height ) ) / height );

        const QImage *image = m_cache[id];
        if ( image == 0 )
        {
            QSharedPointer<TextureTile> tile = m_tileLoader->loadTile( id, id, DownloadBrowse );
            image = new QImage( *tile->image() );
            m_cache.insert( id, image );
        }
        Q_ASSERT( image );

        Q_ASSERT( width == image->width() );
        Q_ASSERT( height == image->height() );

        const qreal dx = ( textureX > (qreal)x ) ? textureX - (qreal)x : (qreal)x - textureX;
        const qreal dy = ( textureY > (qreal)y ) ? textureY - (qreal)y : (qreal)y - textureY;
        Q_ASSERT( 0 <= dx && dx <= 1 );
        Q_ASSERT( 0 <= dy && dy <= 1 );
        color += image->pixel( x % width, y % height ) * (1- dx) * (1-dy);
    }

    return color * m_radius / 6378137.0 / 1000;
}

}
