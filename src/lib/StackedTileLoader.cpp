/**
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 * Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jensmh@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "StackedTileLoader.h"

#include "GeoSceneTexture.h"
#include "MarbleDebug.h"
#include "MergedLayerDecorator.h"
#include "StackedTile.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "global.h"

#include <QtCore/QCache>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QReadWriteLock>
#include <QtGui/QImage>


namespace Marble
{

class StackedTileLoaderPrivate
{
public:
    StackedTileLoaderPrivate( HttpDownloadManager * const downloadManager,
                              MapThemeManager *const mapThemeManager,
                              SunLocator * const sunLocator )
        : m_tileLoader( downloadManager, mapThemeManager ),
          m_layerDecorator( &m_tileLoader, sunLocator ),
          m_maxTileLevel( 0 )
    {
        m_tileCache.setMaxCost( 20000 * 1024 ); // Cache size measured in bytes
    }

    void detectMaxTileLevel();
    QVector<GeoSceneTexture const *>
        findRelevantTextureLayers( TileId const & stackedTileId ) const;

    TileLoader  m_tileLoader;
    MergedLayerDecorator m_layerDecorator;
    int         m_maxTileLevel;
    QVector<GeoSceneTexture const *> m_textureLayers;
    QHash <TileId, StackedTile*>  m_tilesOnDisplay;
    QCache <TileId, StackedTile>  m_tileCache;
    QReadWriteLock m_cacheLock;
};

StackedTileLoader::StackedTileLoader( HttpDownloadManager * const downloadManager,
                                      MapThemeManager * const mapThemeManager,
                                      SunLocator * const sunLocator )
    : d( new StackedTileLoaderPrivate( downloadManager, mapThemeManager, sunLocator ) )
{
    connect( &d->m_tileLoader, SIGNAL( tileCompleted( TileId, const QImage & )),
             SLOT( updateTile( TileId, const QImage & )));
}

StackedTileLoader::~StackedTileLoader()
{
    qDeleteAll( d->m_tilesOnDisplay );
    delete d;
}

void StackedTileLoader::setTextureLayers( QVector<GeoSceneTexture const *> & textureLayers )
{
    mDebug() << "StackedTileLoader::setTextureLayers";

    d->m_textureLayers = textureLayers;

    d->m_tilesOnDisplay.clear();
    d->m_tileCache.clear();

    d->detectMaxTileLevel();
    d->m_layerDecorator.setThemeId( "maps/" + d->m_textureLayers.at( 0 )->sourceDir() );
}

void StackedTileLoader::setShowTileId( bool show )
{
    d->m_layerDecorator.setShowTileId( show );
}

int StackedTileLoader::tileColumnCount( int level ) const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    const int levelZeroColumns = d->m_textureLayers.at( 0 )->levelZeroColumns();

    return TileLoaderHelper::levelToColumn( levelZeroColumns, level );
}

int StackedTileLoader::tileRowCount( int level ) const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    const int levelZeroRows = d->m_textureLayers.at( 0 )->levelZeroRows();

    return TileLoaderHelper::levelToRow( levelZeroRows, level );
}

GeoSceneTexture::Projection StackedTileLoader::tileProjection() const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    return d->m_textureLayers.at( 0 )->projection();
}

QSize StackedTileLoader::tileSize() const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    return d->m_textureLayers.at( 0 )->tileSize();
}

void StackedTileLoader::resetTilehash()
{
    QHash<TileId, StackedTile*>::const_iterator it = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, StackedTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();
    for (; it != end; ++it ) {
        it.value()->setUsed( false );
    }
}

void StackedTileLoader::cleanupTilehash()
{
    // Make sure that tiles which haven't been used during the last
    // rendering of the map at all get removed from the tile hash.

    QHashIterator<TileId, StackedTile*> it( d->m_tilesOnDisplay );
    while ( it.hasNext() ) {
        it.next();
        if ( !it.value()->used() ) {
            // If insert call result is false then the cache is too small to store the tile
            // but the item will get deleted nevertheless and the pointer we have
            // doesn't get set to zero (so don't delete it in this case or it will crash!)
            d->m_tileCache.insert( it.key(), it.value(), it.value()->numBytes() );
            d->m_tilesOnDisplay.remove( it.key() );
        }
    }
}

StackedTile* StackedTileLoader::loadTile( TileId const & stackedTileId )
{
    // check if the tile is in the hash
    d->m_cacheLock.lockForRead();
    StackedTile * stackedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    d->m_cacheLock.unlock();
    if ( stackedTile ) {
        return stackedTile;
    }
    // here ends the performance critical section of this method

    d->m_cacheLock.lockForWrite();

    // has another thread loaded our tile due to a race condition?
    stackedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    if ( d->m_tilesOnDisplay.contains( stackedTileId ) ) {
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    mDebug() << "StackedTileLoader::loadTile" << stackedTileId.toString();

    // the tile was not in the hash so check if it is in the cache
    stackedTile = d->m_tileCache.take( stackedTileId );
    if ( stackedTile ) {
        d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    // tile (valid) has not been found in hash or cache, so load it from disk
    // and place it in the hash from where it will get transferred to the cache

    // mDebug() << "load Tile from Disk: " << stackedTileId.toString();

    QVector<QSharedPointer<TextureTile> > tiles;
    QVector<GeoSceneTexture const *> const textureLayers = d->findRelevantTextureLayers( stackedTileId );
    QVector<GeoSceneTexture const *>::const_iterator pos = textureLayers.constBegin();
    QVector<GeoSceneTexture const *>::const_iterator const end = textureLayers.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneTexture const * const textureLayer = *pos;
        TileId const tileId( textureLayer->sourceDir(), stackedTileId.zoomLevel(),
                             stackedTileId.x(), stackedTileId.y() );
        mDebug() << "StackedTileLoader::loadTile: tile" << textureLayer->sourceDir()
                 << tileId.toString() << textureLayer->tileSize();
        const QImage tileImage = d->m_tileLoader.loadTile( tileId, DownloadBrowse );
        QSharedPointer<TextureTile> tile( new TextureTile( tileId, tileImage, textureLayer->blending() ) ); 
        tiles.append( tile );
    }
    Q_ASSERT( !tiles.isEmpty() );

    const QImage resultImage = d->m_layerDecorator.merge( stackedTileId, tiles );
    stackedTile = new StackedTile( stackedTileId, resultImage, tiles );

    d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
    d->m_cacheLock.unlock();
    return stackedTile;
}

void StackedTileLoader::downloadTile( TileId const & stackedTileId )
{
    QVector<GeoSceneTexture const *> const textureLayers = d->findRelevantTextureLayers( stackedTileId );
    QVector<GeoSceneTexture const *>::const_iterator pos = textureLayers.constBegin();
    QVector<GeoSceneTexture const *>::const_iterator const end = textureLayers.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneTexture const * const textureLayer = *pos;
        TileId const tileId( textureLayer->sourceDir(), stackedTileId.zoomLevel(),
                             stackedTileId.x(), stackedTileId.y() );
        d->m_tileLoader.downloadTile( tileId );
    }
}

quint64 StackedTileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.maxCost() / 1024;
}

void StackedTileLoader::reloadVisibleTiles()
{
    foreach ( StackedTile * const displayedTile, d->m_tilesOnDisplay.values() ) {
        Q_ASSERT( displayedTile != 0 );
        foreach ( QSharedPointer<TextureTile> const & tile, displayedTile->tiles() ) {
            // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
            // but since "reload" or "refresh" seems to be a common action of a browser and it
            // allows for more connections (in our model), use "DownloadBrowse"
            d->m_tileLoader.reloadTile( tile->id(), DownloadBrowse );
        }
    }

    emit tileUpdatesAvailable();
}

int StackedTileLoader::maximumTileLevel() const
{
    return d->m_maxTileLevel;
}

void StackedTileLoaderPrivate::detectMaxTileLevel()
{
    if ( m_textureLayers.isEmpty() ) {
        m_maxTileLevel = -1;
        return;
    }

    m_maxTileLevel = TileLoader::maximumTileLevel( *m_textureLayers.at( 0 ) );
}


void StackedTileLoader::setVolatileCacheLimit( quint64 kiloBytes )
{
    mDebug() << QString("Setting tile cache to %1 kilobytes.").arg( kiloBytes );
    d->m_tileCache.setMaxCost( kiloBytes * 1024 );
}

void StackedTileLoader::updateTile( TileId const &tileId, QImage const &tileImage )
{
    d->detectMaxTileLevel();

    const TileId stackedTileId( 0, tileId.zoomLevel(), tileId.x(), tileId.y() );

    StackedTile * displayedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    if ( displayedTile ) {
        Q_ASSERT( !d->m_tileCache.contains( stackedTileId ) );

        QVector<QSharedPointer<TextureTile> > tiles = displayedTile->tiles();
        delete displayedTile;
        displayedTile = 0;

        for ( int i = 0; i < tiles.count(); ++ i) {
            if ( tiles[i]->id() == tileId ) {
                const Blending *blending = tiles[i]->blending();
                tiles[i] = QSharedPointer<TextureTile>( new TextureTile( tileId, tileImage, blending ) );
            }
        }

        const QImage resultImage = d->m_layerDecorator.merge( stackedTileId, tiles );
        displayedTile = new StackedTile( stackedTileId, resultImage, tiles );
        d->m_tilesOnDisplay.insert( stackedTileId, displayedTile );

        emit tileUpdateAvailable( stackedTileId );
        return;
    }

    d->m_tileCache.remove( stackedTileId );
}

void StackedTileLoader::clear()
{
    mDebug() << "StackedTileLoader::clear()";
    qDeleteAll( d->m_tilesOnDisplay );
    d->m_tilesOnDisplay.clear();
    d->m_tileCache.clear(); // clear the tile cache in physical memory
}

// 
QVector<GeoSceneTexture const *>
StackedTileLoaderPrivate::findRelevantTextureLayers( TileId const & stackedTileId ) const
{
    QVector<GeoSceneTexture const *> result;
    QVector<GeoSceneTexture const *>::const_iterator pos = m_textureLayers.constBegin();
    QVector<GeoSceneTexture const *>::const_iterator const end = m_textureLayers.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneTexture const * const candidate = dynamic_cast<GeoSceneTexture const *>( *pos );
        // check if layer is enabled. A layer is considered to be enabled if one of the
        // following conditions is true:
        // 1) it is the first layer
        // 2) there are no settings available (group "Texture Layers" not defined in DGML)
        // 3) the layer is configured and enabled in the settings
        // also check, if layer provides tiles for the current level
        if ( candidate
             && ( !candidate->hasMaximumTileLevel()
                  || stackedTileId.zoomLevel() <= candidate->maximumTileLevel() ))
            result.append( candidate );
    }
    return result;
}

}

#include "StackedTileLoader.moc"
