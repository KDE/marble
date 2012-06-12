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

#include "GeoSceneTiled.h"
#include "MarbleDebug.h"
#include "MergedLayerDecorator.h"
#include "StackedTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "MarbleGlobal.h"

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
    StackedTileLoaderPrivate( MergedLayerDecorator *mergedLayerDecorator )
        : m_layerDecorator( mergedLayerDecorator ),
          m_maxTileLevel( 0 )
    {
        m_tileCache.setMaxCost( 20000 * 1024 ); // Cache size measured in bytes
    }

    void detectMaxTileLevel();
    QVector<GeoSceneTiled const *>
        findRelevantTextureLayers( TileId const & stackedTileId ) const;

    MergedLayerDecorator *const m_layerDecorator;
    int         m_maxTileLevel;
    QVector<GeoSceneTiled const *> m_textureLayers;
    QHash <TileId, StackedTile*>  m_tilesOnDisplay;
    QCache <TileId, StackedTile>  m_tileCache;
    QReadWriteLock m_cacheLock;
};

StackedTileLoader::StackedTileLoader( MergedLayerDecorator *mergedLayerDecorator )
    : d( new StackedTileLoaderPrivate( mergedLayerDecorator ) )
{
}

StackedTileLoader::~StackedTileLoader()
{
    qDeleteAll( d->m_tilesOnDisplay );
    delete d;
}

void StackedTileLoader::setTextureLayers( QVector<GeoSceneTiled const *> & textureLayers )
{
    mDebug() << Q_FUNC_INFO;

    d->m_textureLayers = textureLayers;

    clear();

    d->detectMaxTileLevel();
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

GeoSceneTiled::Projection StackedTileLoader::tileProjection() const
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

const StackedTile* StackedTileLoader::loadTile( TileId const & stackedTileId )
{
    // check if the tile is in the hash
    d->m_cacheLock.lockForRead();
    StackedTile * stackedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    d->m_cacheLock.unlock();
    if ( stackedTile ) {
        stackedTile->setUsed( true );
        return stackedTile;
    }
    // here ends the performance critical section of this method

    d->m_cacheLock.lockForWrite();

    // has another thread loaded our tile due to a race condition?
    stackedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    if ( stackedTile ) {
        stackedTile->setUsed( true );
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    mDebug() << "StackedTileLoader::loadTile" << stackedTileId.toString();

    // the tile was not in the hash so check if it is in the cache
    stackedTile = d->m_tileCache.take( stackedTileId );
    if ( stackedTile ) {
        stackedTile->setUsed( true );
        d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    // tile (valid) has not been found in hash or cache, so load it from disk
    // and place it in the hash from where it will get transferred to the cache

    // mDebug() << "load Tile from Disk: " << stackedTileId.toString();

    QVector<GeoSceneTiled const *> const textureLayers = d->findRelevantTextureLayers( stackedTileId );

    stackedTile = d->m_layerDecorator->loadTile( stackedTileId, textureLayers );
    stackedTile->setUsed( true );

    d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
    d->m_cacheLock.unlock();
    return stackedTile;
}

void StackedTileLoader::downloadTile( TileId const & stackedTileId )
{
    QVector<GeoSceneTiled const *> const textureLayers = d->findRelevantTextureLayers( stackedTileId );
    d->m_layerDecorator->downloadTile( stackedTileId, textureLayers );
}

quint64 StackedTileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.maxCost() / 1024;
}

void StackedTileLoader::reloadVisibleTiles()
{
    foreach ( const StackedTile *displayedTile, d->m_tilesOnDisplay.values() ) {
        d->m_layerDecorator->reloadTile( *displayedTile );
    }
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

void StackedTileLoader::updateTile( TileId const &tileId, QImage const &tileImage, const QString &format )
{
    Q_ASSERT( !tileImage.isNull() );

    if (tileImage.isNull())
        mDebug()<< "null";

    d->detectMaxTileLevel();

    const TileId stackedTileId( 0, tileId.zoomLevel(), tileId.x(), tileId.y() );

    StackedTile * displayedTile = d->m_tilesOnDisplay.take( stackedTileId );
    if ( displayedTile ) {
        Q_ASSERT( !d->m_tileCache.contains( stackedTileId ) );

        StackedTile *const stackedTile = d->m_layerDecorator->createTile( *displayedTile, tileId, tileImage, format );
        d->m_tilesOnDisplay.insert( stackedTileId, stackedTile );

        delete displayedTile;
        displayedTile = 0;
    } else {
        d->m_tileCache.remove( stackedTileId );
    }
}

void StackedTileLoader::clear()
{
    mDebug() << "StackedTileLoader::clear()";
    qDeleteAll( d->m_tilesOnDisplay );
    d->m_tilesOnDisplay.clear();
    d->m_tileCache.clear(); // clear the tile cache in physical memory
}

// 
QVector<GeoSceneTiled const *>
StackedTileLoaderPrivate::findRelevantTextureLayers( TileId const & stackedTileId ) const
{
    QVector<GeoSceneTiled const *> result;
    QVector<GeoSceneTiled const *>::const_iterator pos = m_textureLayers.constBegin();
    QVector<GeoSceneTiled const *>::const_iterator const end = m_textureLayers.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneTiled const * const candidate = dynamic_cast<GeoSceneTiled const *>( *pos );
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
