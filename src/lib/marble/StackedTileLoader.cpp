/*
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 * Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jensmh@gmx.de>
 * Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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

#include "MarbleDebug.h"
#include "MergedLayerDecorator.h"
#include "StackedTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "MarbleGlobal.h"

#include <QCache>
#include <QHash>
#include <QReadWriteLock>
#include <QImage>


namespace Marble
{

class StackedTileLoaderPrivate
{
public:
    explicit StackedTileLoaderPrivate( MergedLayerDecorator *mergedLayerDecorator )
        : m_layerDecorator( mergedLayerDecorator )
    {
        m_tileCache.setMaxCost( 20000 * 1024 ); // Cache size measured in bytes
    }

    MergedLayerDecorator *const m_layerDecorator;
    QHash <TileId, StackedTile*>  m_tilesOnDisplay;
    QCache <TileId, StackedTile>  m_tileCache;
    QReadWriteLock m_cacheLock;
};

StackedTileLoader::StackedTileLoader( MergedLayerDecorator *mergedLayerDecorator, QObject *parent )
    : QObject( parent ),
      d( new StackedTileLoaderPrivate( mergedLayerDecorator ) )
{
}

StackedTileLoader::~StackedTileLoader()
{
    qDeleteAll( d->m_tilesOnDisplay );
    delete d;
}

int StackedTileLoader::tileColumnCount( int level ) const
{
    return d->m_layerDecorator->tileColumnCount( level );
}

int StackedTileLoader::tileRowCount( int level ) const
{
    return d->m_layerDecorator->tileRowCount( level );
}

GeoSceneAbstractTileProjection::Type StackedTileLoader::tileProjectionType() const
{
    return d->m_layerDecorator->tileProjectionType();
}

QSize StackedTileLoader::tileSize() const
{
    return d->m_layerDecorator->tileSize();
}

void StackedTileLoader::resetTilehash()
{
    QHash<TileId, StackedTile*>::const_iterator it = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, StackedTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();
    for (; it != end; ++it ) {
        Q_ASSERT( it.value()->used() && "contained in m_tilesOnDisplay should imply used()" );
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
            d->m_tileCache.insert( it.key(), it.value(), it.value()->byteCount() );
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
        Q_ASSERT( stackedTile->used() && "other thread should have marked tile as used" );
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    // the tile was not in the hash so check if it is in the cache
    stackedTile = d->m_tileCache.take( stackedTileId );
    if ( stackedTile ) {
        Q_ASSERT( !stackedTile->used() && "tiles in m_tileCache are invisible and should thus be marked as unused" );
        stackedTile->setUsed( true );
        d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    // tile (valid) has not been found in hash or cache, so load it from disk
    // and place it in the hash from where it will get transferred to the cache

    mDebug() << "load tile from disk:" << stackedTileId;

    stackedTile = d->m_layerDecorator->loadTile( stackedTileId );
    Q_ASSERT( stackedTile );
    stackedTile->setUsed( true );

    d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
    d->m_cacheLock.unlock();

    emit tileLoaded( stackedTileId );

    return stackedTile;
}

quint64 StackedTileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.maxCost() / 1024;
}

QList<TileId> StackedTileLoader::visibleTiles() const
{
    return d->m_tilesOnDisplay.keys();
}

int StackedTileLoader::tileCount() const
{
    return d->m_tileCache.count() + d->m_tilesOnDisplay.count();
}

void StackedTileLoader::setVolatileCacheLimit( quint64 kiloBytes )
{
    mDebug() << QString("Setting tile cache to %1 kilobytes.").arg( kiloBytes );
    d->m_tileCache.setMaxCost( kiloBytes * 1024 );
}

void StackedTileLoader::updateTile( TileId const &tileId, QImage const &tileImage )
{
    const TileId stackedTileId( 0, tileId.zoomLevel(), tileId.x(), tileId.y() );

    StackedTile * displayedTile = d->m_tilesOnDisplay.take( stackedTileId );
    if ( displayedTile ) {
        Q_ASSERT( !d->m_tileCache.contains( stackedTileId ) );

        StackedTile *const stackedTile = d->m_layerDecorator->updateTile( *displayedTile, tileId, tileImage );
        stackedTile->setUsed( true );
        d->m_tilesOnDisplay.insert( stackedTileId, stackedTile );

        delete displayedTile;
        displayedTile = 0;

        emit tileLoaded( stackedTileId );
    } else {
        d->m_tileCache.remove( stackedTileId );
    }
}

RenderState StackedTileLoader::renderState() const
{
    RenderState renderState( "Stacked Tiles" );
    QHash<TileId, StackedTile*>::const_iterator it = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, StackedTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();
    for (; it != end; ++it ) {
        renderState.addChild( d->m_layerDecorator->renderState( it.key() ) );
    }
    return renderState;
}

void StackedTileLoader::clear()
{
    mDebug() << Q_FUNC_INFO;

    qDeleteAll( d->m_tilesOnDisplay );
    d->m_tilesOnDisplay.clear();
    d->m_tileCache.clear(); // clear the tile cache in physical memory

    emit cleared();
}

}

#include "moc_StackedTileLoader.cpp"
