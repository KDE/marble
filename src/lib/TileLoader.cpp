/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>"
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
#include "HttpDownloadManager.h"
#include "TextureTile.h"
#include "MarbleDirs.h"
#include "TileCache.h"
#include "TileId.h"
#include "MarbleModel.h"
#include "TileLoaderHelper.h"

#include <QtCore/QDebug>
#include <QtCore/QHash>

#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   long double log(int i) { return log((long double)i); }
# endif
#endif

class TileLoader::Private
{
    public:
        Private()
            : m_downloadManager( 0 ),
              m_textureLayer( 0 )
        {
            m_tileCache.clear();
            m_tileCache.setCacheLimit( 20000 ); // Cache size measured in kiloByte
        }

        ~Private()
        {
        }

        HttpDownloadManager *m_downloadManager;
        GeoSceneTexture     *m_textureLayer;
        QHash <TileId, TextureTile*>  m_tileHash;
        int           m_tileWidth;
        int           m_tileHeight;
        TileCache     m_tileCache;
};

TileLoader::TileLoader( HttpDownloadManager *downloadManager, MarbleModel* parent)
    : d( new Private() ),
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

void TileLoader::setTextureLayer( GeoSceneTexture *textureLayer )
{
    // Initialize map theme.
    flush();
    d->m_tileCache.clear();

    d->m_textureLayer = textureLayer;

    TextureTile *tile = new TextureTile( TileId() );
    tile->loadRawTile(d->m_textureLayer, 0, 0, 0);

    // We assume that all tiles have the same size. TODO: check to be safe
    d->m_tileWidth  = tile->rawtile().width();
    d->m_tileHeight = tile->rawtile().height();

    qDebug() << "TileLoader::setTextureLayer: delete tile";
    delete tile;
}

void TileLoader::resetTilehash()
{
    QHash<TileId, TextureTile*>::const_iterator it = d->m_tileHash.constBegin();
    while ( it != d->m_tileHash.constEnd() ) {
        it.value()->setUsed( false );
        ++it;
    }
}

void TileLoader::cleanupTilehash()
{
    // Make sure that tiles which haven't been used during the last
    // rendering of the map at all get removed from the tile hash.

    QHashIterator<TileId, TextureTile*> it( d->m_tileHash );
    while ( it.hasNext() ) {
        it.next();
        if ( it.value()->used() == false ) {

            bool inCache = d->m_tileCache.insert( it.key(), it.value() );
            d->m_tileHash.remove( it.key() );
            if ( inCache == false )
                delete it.value();
        }
    }
}

void TileLoader::flush()
{
    // Remove all tiles from m_tileHash
    QHashIterator<TileId, TextureTile*> it( d->m_tileHash );
    while ( it.hasNext() ) {
        it.next();

        bool inCache = d->m_tileCache.insert( it.key(), it.value() );
        d->m_tileHash.remove( it.key() );
        if ( inCache == false )
            delete it.value();
    }

    d->m_tileHash.clear();
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
    return d->m_tileWidth * TileLoaderHelper::levelToColumn( level );
}

int TileLoader::globalHeight( int level ) const
{
    return d->m_tileHeight * TileLoaderHelper::levelToRow( level );
}

TextureTile* TileLoader::loadTile( int tilx, int tily, int tileLevel )
{
    // Choosing the correct tile via Lon/Lat info 
    TextureTile* tile = 0;
    TileId tileId( tileLevel, tilx, tily );


    // If the tile hasn't been loaded into the m_tileHash yet, then do so...
    if ( !d->m_tileHash.contains( tileId ) ) {
        if ( d->m_tileCache.contains( tileId ) ) {
            tile = d->m_tileCache.take( tileId );
            d->m_tileHash[tileId] = tile;
        } else {
            // qDebug() << "load Tile from Disk: " << tileId.toString();
            tile = new TextureTile( tileId );
            d->m_tileHash[tileId] = tile;

            if ( d->m_downloadManager != 0 ) {
                connect( tile, SIGNAL( downloadTile( const QString&, const QString& ) ),
                         d->m_downloadManager, SLOT( addJob( const QString&, const QString& ) ) );
            }

            connect( tile, SIGNAL( tileUpdateDone() ),
                     this, SIGNAL( tileUpdateAvailable() ) );

            tile->loadRawTile(d->m_textureLayer, tileLevel, tilx, tily);
            tile->loadTile(false);
            // TODO should emit signal rather than directly calling paintTile
//             emit paintTile( tile, tilx, tily, tileLevel, d->m_theme, false );
            m_parent->paintTile( tile, tilx, tily, tileLevel, d->m_textureLayer, false );
        }
    } else { // ...otherwise pick the correct one from the hash
        tile = d->m_tileHash.value( tileId );
        if ( !tile->used() ) {
            tile->setUsed( true );
        }
    }

    return tile;
}

GeoSceneTexture* TileLoader::textureLayer() const
{
    return d->m_textureLayer;
}

quint64 TileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.cacheLimit();
}

int TileLoader::maxCompleteTileLevel( GeoSceneTexture *textureLayer )
{
    bool noerr = true;

    int tilelevel = -1;
    int trylevel  = 0;

    // if ( m_bitmaplayer.type.toLower() == "bitmap" ){
    while ( noerr == true ) {
        int nmaxit = TileLoaderHelper::levelToRow( trylevel );

        for ( int n = 0; n < nmaxit; ++n ) {
            int mmaxit = TileLoaderHelper::levelToColumn( trylevel );

            for ( int m = 0; m < mmaxit; ++m ) {
                QString tilepath = MarbleDirs::path(
                    TileLoaderHelper::relativeTileFileName( textureLayer, trylevel, m, n ));
                // qDebug() << tilepath;
                noerr = QFile::exists( tilepath );
                if ( noerr == false )
                    break;
            }
            if ( noerr == false )
                break;
        }

        if ( noerr == true )
            tilelevel = trylevel;

        ++trylevel;
    }

    if ( tilelevel == -1 ) {
        qDebug("No Tiles Found!");
    }

    //qDebug() << "Detected maximum complete tile level: " << tilelevel;

    return tilelevel;
}


int TileLoader::maxPartialTileLevel( GeoSceneTexture *textureLayer )
{
    QString tilepath = MarbleDirs::path( TileLoaderHelper::themeStr( textureLayer ) );
    QStringList leveldirs = ( QDir( tilepath ) ).entryList( QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );

    int maxtilelevel = -1;
    QString str;
    bool ok = true;

    QStringList::const_iterator constIterator;
    for ( constIterator = leveldirs.constBegin();
          constIterator != leveldirs.constEnd();
         ++constIterator)
    {
        int value = (*constIterator).toInt( &ok, 10 );
        // qDebug() << "Value: " << value  << "Ok: " << ok;
        if ( ok && value > maxtilelevel )
            maxtilelevel = value;
    }

    //qDebug() << "Detected maximum tile level that contains data: "
    //         << maxtilelevel;

    return maxtilelevel;
}


bool TileLoader::baseTilesAvailable( GeoSceneTexture *textureLayer )
{
    bool noerr = true; 

    // Check whether the two tiles from the lowest texture level are available
    // FIXME: - this assumes that there are 2 tiles in level 0 which is not true for OpenStreetMap
    //        - marble could theoretically start without local tiles, too. They can be downloaded.
    for ( int m = 0; m < 2; ++m ) {
        QString tilepath = MarbleDirs::path( TileLoaderHelper::relativeTileFileName(
            textureLayer, 0, m, 0 ));

        noerr = QFile::exists( tilepath );

        if ( noerr == false )
            break;
    }

    // qDebug() << "Mandatory most basic tile level is fully available: " << noerr;

    return noerr;
}

void TileLoader::setVolatileCacheLimit( quint64 kiloBytes )
{
    qDebug() << QString("Setting tile cache to %1 kilobytes.").arg( kiloBytes );
    d->m_tileCache.setCacheLimit( kiloBytes );
}

void TileLoader::reloadTile( const QString &relativeUrlString, const QString &_id )
{
    Q_UNUSED( relativeUrlString );
    // qDebug() << "Reloading Tile" << relativeUrlString << "id:" << _id;

    const TileId id = TileId::fromString( _id );
    if ( d->m_tileHash.contains( id ) ) {
        int  level = id.zoomLevel();
        int  y     = id.y();
        int  x     = id.x();

        // TODO should emit signal rather than directly calling paintTile
//         emit paintTile( d->m_tileHash[id], x, y, level, d->m_theme, true );
        m_parent->paintTile( d->m_tileHash[id], x, y, level, d->m_textureLayer, true );
//         (d->m_tileHash[id]) -> reloadTile( x, y, level, d->m_theme );
    } else {
        qDebug() << "No such ID:" << id.toString();
    }
}

void TileLoader::update()
{
    flush(); // trigger a reload of all tiles that are currently in use
    d->m_tileCache.clear(); // clear the tile cache in physical memory
    emit tileUpdateAvailable();
}

#include "TileLoader.moc"
