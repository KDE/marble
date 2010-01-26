/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 * Copyright 2008,2009 Jens-Michael Hoffmann <jensmh@gmx.de>
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


#include "TileLoader.h"

#include "global.h"
#include "GeoSceneLayer.h"
#include "GeoSceneTexture.h"
#include "HttpDownloadManager.h"
#include "DatasetProvider.h"
#include "TextureTile.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "TileLoaderHelper.h"
#include "MarbleDebug.h"

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

class TileLoaderPrivate
{
    public:
        TileLoaderPrivate()
            : m_datasetProvider( 0 ),
              m_downloadManager( 0 ),
              m_layer( 0 ),
              m_tileWidth( 0 ),
              m_tileHeight( 0 )
        {
            m_tileCache.setMaxCost( 20000 * 1024 ); // Cache size measured in bytes
        }

        DatasetProvider *m_datasetProvider;
        HttpDownloadManager *m_downloadManager;
        GeoSceneLayer *m_layer;
        QHash <TileId, TextureTile*>  m_tilesOnDisplay;
        int           m_tileWidth;
        int           m_tileHeight;
        QCache <TileId, TextureTile>  m_tileCache;
};



TileLoader::TileLoader( HttpDownloadManager *downloadManager, MarbleModel* parent)
    : d( new TileLoaderPrivate() ),
      m_parent(parent)
{
    setDownloadManager( downloadManager );
}

TileLoader::~TileLoader()
{
    flush();
    d->m_tileCache.clear();
    if ( d->m_downloadManager != 0 )
        d->m_downloadManager->disconnect( this );

    delete d;
}

void TileLoader::setDownloadManager( HttpDownloadManager *downloadManager )
{
    if ( d->m_downloadManager != 0 ) {
        d->m_downloadManager->disconnect( this );
        d->m_downloadManager = 0;
    }

    d->m_downloadManager = downloadManager;
    if ( d->m_downloadManager != 0 ) {
        connect( d->m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ),
                 this,              SLOT( reloadTile( QString, QString ) ) );
    }
}

void TileLoader::setLayer( GeoSceneLayer * layer )
{
    // Initialize map theme.
    flush();
    d->m_tileCache.clear();

    if ( !layer ) {
        mDebug() << "No layer specified! (GeoSceneLayer * layer = 0)";
        return;
    }

    d->m_layer = layer;

    TileId id( 0, 0, 0 );
    TextureTile tile( id );

    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( d->m_layer->groundDataset() );

    tile.loadDataset( texture, id );

    // We assume that all tiles have the same size. TODO: check to be safe
    d->m_tileWidth  = tile.rawtile().width();
    d->m_tileHeight = tile.rawtile().height();
}

void TileLoader::resetTilehash()
{
    QHash<TileId, TextureTile*>::const_iterator it = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, TextureTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();
    for (; it != end; ++it ) {
        it.value()->setUsed( false );
    }
}

void TileLoader::cleanupTilehash()
{
    // Make sure that tiles which haven't been used during the last
    // rendering of the map at all get removed from the tile hash.

    QHashIterator<TileId, TextureTile*> it( d->m_tilesOnDisplay );
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

void TileLoader::flush()
{
    // Remove all tiles from m_tilesOnDisplay
    QHashIterator<TileId, TextureTile*> it( d->m_tilesOnDisplay );
    while ( it.hasNext() ) {
        it.next();
        // If insert call result is false then the cache is too small to store the tile 
        // but the item will get deleted nevertheless and the pointer we have 
        // doesn't get set to zero (so don't delete it in this case or it will crash!)
        d->m_tileCache.insert( it.key(), it.value(), it.value()->numBytes() );
        d->m_tilesOnDisplay.remove( it.key() );
    }

    d->m_tilesOnDisplay.clear();
}

int TileLoader::tileWidth() const
{
    return d->m_tileWidth;
}

int TileLoader::tileHeight() const
{
    return d->m_tileHeight;
}

int TileLoader::globalWidth( int level ) const
{
    if ( !d->m_layer ) return 0;

    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( d->m_layer->groundDataset() );

    return d->m_tileWidth * TileLoaderHelper::levelToColumn( 
                                texture->levelZeroColumns(), level );
}

int TileLoader::globalHeight( int level ) const
{
    if ( !d->m_layer ) return 0;

    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( d->m_layer->groundDataset() );

    return d->m_tileHeight * TileLoaderHelper::levelToRow( 
                                texture->levelZeroRows(), level );
}

TextureTile* TileLoader::loadTile( int tilx, int tily, int tileLevel )
{
    if ( !d->m_layer ) return 0;

    TileId tileId( tileLevel, tilx, tily );

    // check if the tile is in the hash
    TextureTile * tile = d->m_tilesOnDisplay.value( tileId, 0 );
    if ( tile ) {
        tile->setUsed( true );
        return tile;
    }
    // here ends the performance critical section of this method

    // the tile was not in the hash or has been removed because of expiration
    // so check if it is in the cache
    tile = d->m_tileCache.take( tileId );

    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( d->m_layer->groundDataset() );

    if ( tile ) {
        // the tile was in the cache, but is it up to date?
        const QDateTime now = QDateTime::currentDateTime();

        if ( tile->created().secsTo( now ) < texture->expire()) {
            d->m_tilesOnDisplay[tileId] = tile;
            tile->setUsed( true );
            return tile;
        } else {
            delete tile;
            tile = 0;
        }
    }

    // tile (valid) has not been found in hash or cache, so load it from disk
    // and place it in the hash from where it will get transferred to the cache

    // mDebug() << "load Tile from Disk: " << tileId.toString();
    tile = new TextureTile( tileId );
    d->m_tilesOnDisplay[tileId] = tile;

    // FIXME: Implement asynchronous tile loading
    // d->m_datasetProvider->loadDatasets( tile );

    if ( d->m_downloadManager != 0 ) {
        connect( tile, SIGNAL( downloadTile( QUrl, QString, QString, DownloadUsage ) ),
                 d->m_downloadManager, SLOT( addJob( QUrl, QString, QString, DownloadUsage ) ) );
    }
    connect( tile, SIGNAL( tileUpdateDone() ),
             this, SIGNAL( tileUpdateAvailable() ) );

    tile->loadDataset( texture, tileId, &d->m_tileCache );
    tile->initJumpTables( false );

    // TODO should emit signal rather than directly calling paintTile
    // emit paintTile( tile, tilx, tily, tileLevel, d->m_theme, false );
    m_parent->paintTile( tile, texture, false );

    return tile;
}

GeoSceneLayer * TileLoader::layer() const
{
    return d->m_layer;
}

quint64 TileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.maxCost() / 1024;
}

QList<TileId> TileLoader::tilesOnDisplay() const
{
    QList<TileId> result;
    QHash<TileId, TextureTile*>::const_iterator pos = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, TextureTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();
    for (; pos != end; ++pos ) {
        if ( pos.value()->used() ) {
            result.append( pos.key() );
        }
    }
    return result;
}

int TileLoader::maxPartialTileLevel( GeoSceneLayer * layer )
{
    int maxtilelevel = -1;

    if ( !layer ) return maxtilelevel;

    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( layer->groundDataset() );

    if ( !texture ) return maxtilelevel;

    QString tilepath = MarbleDirs::path( TileLoaderHelper::themeStr( texture ) );
//    mDebug() << "TileLoader::maxPartialTileLevel tilepath" << tilepath;
    QStringList leveldirs = QDir( tilepath ).entryList( QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );

    bool ok = true;

    QStringList::const_iterator it = leveldirs.constBegin();
    QStringList::const_iterator const end = leveldirs.constEnd();
    for (; it != end; ++it ) {
        int value = (*it).toInt( &ok, 10 );
        if ( ok && value > maxtilelevel )
            maxtilelevel = value;
    }

//    mDebug() << "Detected maximum tile level that contains data: "
//             << maxtilelevel;

    return maxtilelevel;
}


bool TileLoader::baseTilesAvailable( GeoSceneLayer * layer )
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

            const QString tilepath = MarbleDirs::path( TileLoaderHelper::relativeTileFileName(
                texture, 0, column, row ));
            noerr = QFile::exists( tilepath );
        }
    }

    return noerr;
}

void TileLoader::setVolatileCacheLimit( quint64 kiloBytes )
{
    mDebug() << QString("Setting tile cache to %1 kilobytes.").arg( kiloBytes );
    d->m_tileCache.setMaxCost( kiloBytes * 1024 );
}

void TileLoader::reloadTile( const QString &idStr )
{
    if ( !d->m_layer ) return;

//    mDebug() << "TileLoader::reloadTile:" << idStr;
 
    const TileId id = TileId::fromString( idStr );
    if ( d->m_tilesOnDisplay.contains( id ) ) {

        // TODO should emit signal rather than directly calling paintTile
//         emit paintTile( d->m_tilesOnDisplay[id], x, y, level, d->m_theme, true );
        GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( d->m_layer->groundDataset() );

        d->m_tilesOnDisplay[id]->loadDataset( texture, id, &d->m_tileCache ); 
        m_parent->paintTile( d->m_tilesOnDisplay[id], texture, true );
//         (d->m_tilesOnDisplay[id])->reloadTile( x, y, level, d->m_theme );
    } else {
      // Remove "false" tile from cache so it doesn't get loaded anymore
      d->m_tileCache.remove( id );
      mDebug() << "No such ID:" << idStr;
    }
}

void TileLoader::reloadTile( const QString &relativeUrlString, const QString &_id )
{
    Q_UNUSED( relativeUrlString );
    // mDebug() << "Reloading Tile" << relativeUrlString << "id:" << _id;

    reloadTile( _id );
}

void TileLoader::update()
{
    mDebug() << "TileLoader::update()";
    flush(); // trigger a reload of all tiles that are currently in use
    d->m_tileCache.clear(); // clear the tile cache in physical memory
    emit tileUpdateAvailable();
}

}

#include "TileLoader.moc"
