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

#include "GeoSceneDocument.h"
#include "GeoSceneGroup.h"
#include "GeoSceneHead.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoSceneSettings.h"
#include "GeoSceneTexture.h"
#include "HttpDownloadManager.h"
#include "MapThemeManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MergedLayerDecorator.h"
#include "StackedTile.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "global.h"

#include <QtCore/QCache>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtGui/QImage>


#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   long double log(int i) { return log((long double)i); }
# endif
#endif

namespace Marble
{

class StackedTileLoaderPrivate
{
public:
    StackedTileLoaderPrivate( TileLoader *tileLoader, SunLocator *sunLocator )
        : m_layerDecorator( tileLoader, sunLocator ),
          m_tileLoader( tileLoader ),
          m_maxTileLevel( 0 )
    {
        m_tileCache.setMaxCost( 20000 * 1024 ); // Cache size measured in bytes
    }

    void detectMaxTileLevel();
    QVector<GeoSceneTexture const *>
        findRelevantTextureLayers( TileId const & stackedTileId ) const;

    MergedLayerDecorator m_layerDecorator;
    TileLoader *m_tileLoader;
    int         m_maxTileLevel;
    QVector<GeoSceneTexture const *> m_textureLayers;
    QHash <TileId, StackedTile*>  m_tilesOnDisplay;
    QCache <TileId, StackedTile>  m_tileCache;
};

StackedTileLoader::StackedTileLoader( TileLoader * const tileLoader,
                                      SunLocator * const sunLocator )
    : d( new StackedTileLoaderPrivate( tileLoader, sunLocator ) )
{
    connect( d->m_tileLoader, SIGNAL( tileCompleted( TileId )),
             SLOT( updateTile( TileId )));
}

StackedTileLoader::~StackedTileLoader()
{
    flush();
    d->m_tileCache.clear();
    delete d;
}

void StackedTileLoader::setTextureLayers( QVector<GeoSceneTexture const *> & textureLayers )
{
    mDebug() << "StackedTileLoader::setTextureLayers";

    d->m_textureLayers = textureLayers;

    d->m_tilesOnDisplay.clear();
    d->m_tileCache.clear();

    d->detectMaxTileLevel();
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

void StackedTileLoader::flush()
{
    // move all tiles from m_tilesOnDisplay into tile cache
    QHash<TileId, StackedTile*>::const_iterator it = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, StackedTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();
    for (; it != end; ++it ) {
        // If insert call result is false then the cache is too small to store the tile
        // but the item will get deleted nevertheless and the pointer we have
        // doesn't get set to zero (so don't delete it in this case or it will crash!)
        d->m_tileCache.insert( it.key(), it.value(), it.value()->numBytes() );
    }
    d->m_tilesOnDisplay.clear();
}

StackedTile* StackedTileLoader::loadTile( TileId const & stackedTileId )
{
    // check if the tile is in the hash
    StackedTile * stackedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    if ( stackedTile ) {
        return stackedTile;
    }
    // here ends the performance critical section of this method

    mDebug() << "StackedTileLoader::loadTile" << stackedTileId.toString();

    // the tile was not in the hash or has been removed because of expiration
    // so check if it is in the cache
    stackedTile = d->m_tileCache.take( stackedTileId );
    if ( stackedTile ) {
        // the tile was in the cache, but is it up to date?
        if ( !stackedTile->isExpired() ) {
            d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
            return stackedTile;
        } else {
            delete stackedTile;
            stackedTile = 0;
        }
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
        QSharedPointer<TextureTile> const tile = d->m_tileLoader->loadTile( stackedTileId, tileId,
                                                                            DownloadBrowse );
        if ( tile ) {
            tile->setBlending( textureLayer->blending() );
            tiles.append( tile );
        }
    }
    Q_ASSERT( !tiles.isEmpty() );

    stackedTile = new StackedTile( stackedTileId, tiles );
    d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
    mergeDecorations( stackedTile );

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
        d->m_tileLoader->downloadTile( tileId );
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
        foreach ( QSharedPointer<TextureTile> const & tile, *displayedTile->tiles() ) {
            // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
            // but since "reload" or "refresh" seems to be a common action of a browser and it
            // allows for more connections (in our model), use "DownloadBrowse"
            d->m_tileLoader->reloadTile( tile, DownloadBrowse );
        }
        mergeDecorations( displayedTile );
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

    GeoSceneTexture const * const texture = m_textureLayers.at( 0 );

    // if maximum tile level is configured in the DGML files,
    // then use it, otherwise use old detection code.
    if ( texture->maximumTileLevel() >= 0 ) {
        m_maxTileLevel = texture->maximumTileLevel();
        return;
    }

    int maximumTileLevel = -1;
    QString tilepath = MarbleDirs::path( texture->themeStr() );
    //    mDebug() << "StackedTileLoader::maxPartialTileLevel tilepath" << tilepath;
    QStringList leveldirs = QDir( tilepath ).entryList( QDir::AllDirs | QDir::NoSymLinks
                                                        | QDir::NoDotAndDotDot );

    QStringList::const_iterator it = leveldirs.constBegin();
    QStringList::const_iterator const end = leveldirs.constEnd();
    for (; it != end; ++it ) {
        bool ok = true;
        const int value = (*it).toInt( &ok, 10 );

        if ( ok && value > maximumTileLevel )
            maximumTileLevel = value;
    }

    //    mDebug() << "Detected maximum tile level that contains data: "
    //             << maxtilelevel;
    m_maxTileLevel = maximumTileLevel + 1;
}

bool StackedTileLoader::baseTilesAvailable( GeoSceneLayer * layer )
{
    if ( !layer ) return false;

    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( layer->groundDataset() );

    const int  levelZeroColumns = texture->levelZeroColumns();
    const int  levelZeroRows    = texture->levelZeroRows();

    bool noerr = true;

    // Check whether the tiles from the lowest texture level are available
    //
    for ( int column = 0; noerr && column < levelZeroColumns; ++column ) {
        for ( int row = 0; noerr && row < levelZeroRows; ++row ) {

            const QString tilepath = MarbleDirs::path( texture->relativeTileFileName(
                TileId( texture->sourceDir(), 0, column, row )));
            noerr = QFile::exists( tilepath );
        }
    }

    return noerr;
}


void StackedTileLoader::setVolatileCacheLimit( quint64 kiloBytes )
{
    mDebug() << QString("Setting tile cache to %1 kilobytes.").arg( kiloBytes );
    d->m_tileCache.setMaxCost( kiloBytes * 1024 );
}

void StackedTileLoader::updateTile( TileId const & stackedTileId )
{
    d->detectMaxTileLevel();

    StackedTile * const displayedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    if ( displayedTile ) {
        mergeDecorations( displayedTile );
        emit tileUpdateAvailable( stackedTileId );
    }
    else {
        StackedTile * const cachedTile = d->m_tileCache.object( stackedTileId );
        if ( cachedTile ) {
            mergeDecorations( cachedTile );
            emit tileUpdateAvailable( stackedTileId );
        }
    }
}

void StackedTileLoader::update()
{
    mDebug() << "StackedTileLoader::update()";
    flush(); // trigger a reload of all tiles that are currently in use
    d->m_tileCache.clear(); // clear the tile cache in physical memory
    emit tileUpdatesAvailable();
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

void StackedTileLoader::mergeDecorations( StackedTile * const tile ) const
{
    Q_ASSERT( tile != 0 );
    Q_ASSERT( !tile->tiles()->isEmpty() );
    Q_ASSERT( !d->m_textureLayers.isEmpty() );
//    mDebug() << "MarbleModel::paintTile: " << "x: " << x << "y:" << y << "level: " << level
//             << "requestTileUpdate" << requestTileUpdate;

    tile->initResultTile();

    d->m_layerDecorator.setInfo( tile->id() );
    d->m_layerDecorator.setTile( tile->resultTile() );

    d->m_layerDecorator.paint( "maps/" + d->m_textureLayers.at( 0 )->sourceDir() );
}

}

#include "StackedTileLoader.moc"
