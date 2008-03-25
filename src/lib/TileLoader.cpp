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

#include <cmath>

#include "HttpDownloadManager.h"
#include "TextureTile.h"
#include "MarbleDirs.h"
#include "TileCache.h"

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
        Private( SunLocator *sunLocator )
            : m_sunLocator( sunLocator ),
              m_downloadManager( 0 )
        {
            m_tileCache.clear();
            m_tileCache.setCacheLimit( 20000 ); // Cache size measured in kiloByte
        }

        ~Private()
        {
            delete m_sunLocator;
        }

        SunLocator* m_sunLocator;
        HttpDownloadManager *m_downloadManager;
        QString       m_theme;
        QHash <int, TextureTile*>  m_tileHash;
        int           m_tileWidth;
        int           m_tileHeight;
        TileCache     m_tileCache;
};

TileLoader::TileLoader( HttpDownloadManager *downloadManager,
                        SunLocator* sunLocator )
    : d( new Private(sunLocator) )
{
    setDownloadManager( downloadManager );
}

TileLoader::TileLoader( const QString& theme,
                        HttpDownloadManager *downloadManager,
                        SunLocator *sunLocator )
    : d( new Private( sunLocator ) )
{
    setDownloadManager( downloadManager );
    setMapTheme( theme );
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

void TileLoader::setMapTheme( const QString& theme )
{
    // Initialize map theme.
    flush();
    d->m_tileCache.clear();

    d->m_theme = theme;

    TextureTile *tile = new TextureTile( 0 );
    tile->loadTile( 0, 0, 0, d->m_theme, false );

    // We assume that all tiles have the same size. TODO: check to be safe
    d->m_tileWidth  = tile->rawtile().width();
    d->m_tileHeight = tile->rawtile().height();

    delete tile;
}

void TileLoader::resetTilehash()
{
    QHash<int, TextureTile*>::const_iterator it = d->m_tileHash.constBegin();
    while ( it != d->m_tileHash.constEnd() ) {
        it.value()->setUsed( false );
        ++it;
    }
}

void TileLoader::cleanupTilehash()
{
    // Make sure that tiles which haven't been used during the last
    // rendering of the map at all get removed from the tile hash.

    QHashIterator<int, TextureTile*> it( d->m_tileHash );
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
    QHashIterator<int, TextureTile*> it( d->m_tileHash );
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
    return d->m_tileWidth * levelToColumn( level );
}

int TileLoader::globalHeight( int level ) const
{
    return d->m_tileHeight * levelToRow( level );
}

TextureTile* TileLoader::loadTile( int tilx, int tily, int tileLevel )
{
    // Choosing the correct tile via Lon/Lat info 
    TextureTile* tile = 0;
    int tileId = tileLevel * 100000000 + ( tily * 10000 ) + tilx;


    // If the tile hasn't been loaded into the m_tileHash yet, then do so...
    if ( !d->m_tileHash.contains( tileId ) ) {
        if ( d->m_tileCache.contains( tileId ) ) {
            tile = d->m_tileCache.take( tileId );
            d->m_tileHash[tileId] = tile;
        } else {
            // qDebug() << "load Tile from Disk: " << tileId;
            tile = new TextureTile( tileId );
            d->m_tileHash[tileId] = tile;

            if ( d->m_downloadManager != 0 ) {
                connect( tile->painter(), SIGNAL( downloadTile( const QString&, const QString& ) ),
                         d->m_downloadManager, SLOT( addJob( const QString&, const QString& ) ) );
            }

            connect( tile, SIGNAL( tileUpdateDone() ),
                     this, SIGNAL( tileUpdateAvailable() ) );

            tile->loadTile( tilx, tily, tileLevel, d->m_theme, false, d->m_sunLocator );
        }
    } else { // ...otherwise pick the correct one from the hash
        tile = d->m_tileHash.value( tileId );
        if ( !tile->used() ) {
            tile->setUsed( true );
        }
    }

    return tile;
}

const QString TileLoader::mapTheme() const
{
    return d->m_theme;
}

int TileLoader::levelToRow( int level )
{
    if ( level < 0 ) {
        qDebug() << QString( "TileLoader::levelToRow(): Invalid level: %1" )
            .arg( level );
        return 0;
    }
    return (int)pow( 2.0, (double)( level ) );
}

int TileLoader::levelToColumn( int level )
{
    if ( level < 0 ) {
        qDebug() << QString( "TileLoader::levelToColumn(): Invalid level: %1" )
            .arg( level );
        return 0;
    }
    return (int)pow( 2.0, (double)( level + 1 ) );
}

int TileLoader::rowToLevel( int row )
{
    if ( row < 1 )    {
        qDebug() << QString( "TileLoader::rowToLevel(): Invalid number of rows: %1" )
            .arg( row );
        return 0;
    }
    return (int)( log( row ) / log( 2 ) );
}

int TileLoader::columnToLevel( int column )
{
    if ( column < 2 ) {
        qDebug() << QString( "TileLoader::columnToLevel(): Invalid number of columns: %1" )
        .arg( column );
        return 0;
    }
    return (int)( log( column / 2 ) / log( 2 ) );
}

quint64 TileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.cacheLimit();
}

int TileLoader::maxCompleteTileLevel( const QString& theme )
{
    bool noerr = true;

    int tilelevel = -1;
    int trylevel  = 0;

    // if ( m_bitmaplayer.type.toLower() == "bitmap" ){
    while ( noerr == true ) {
        int nmaxit = TileLoader::levelToRow( trylevel );

        for ( int n = 0; n < nmaxit; ++n ) {
            int mmaxit = TileLoader::levelToColumn( trylevel );

            for ( int m = 0; m < mmaxit; ++m ) {
                QString tilepath = MarbleDirs::path( QString("%1/%2/%3/%3_%4.jpg")
                                                     .arg( theme ).arg( trylevel )
                                                     .arg( n, tileDigits, 10, QChar( '0' ) )
                                                     .arg( m, tileDigits, 10, QChar( '0' ) ) );
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


int TileLoader::maxPartialTileLevel( const QString& theme )
{
    QString tilepath = MarbleDirs::path( QString( "%1" ).arg( theme ) );
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


bool TileLoader::baseTilesAvailable( const QString& theme )
{
    bool noerr = true; 

    // Check whether the two tiles from the lowest texture level are available
    for ( int m = 0; m < 2; ++m ) {
        QString tilepath = MarbleDirs::path( QString( "%1/%2/%3/%3_%4.jpg" ).arg( theme ).arg( 0 )
                                                  .arg( 0, tileDigits, 10, QChar( '0' ) )
                                                  .arg( m, tileDigits, 10, QChar( '0' ) ) );

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

    const int id = _id.toInt();
    if ( d->m_tileHash.contains( id ) ) {
        int  level =  id / 100000000;
        int  y     = ( id - level * 100000000 ) / 10000;
        int  x     = id - ( level * 100000000 + y * 10000 );

        (d->m_tileHash[id]) -> reloadTile( x, y, level, d->m_theme, d->m_sunLocator );
    } else {
         qDebug() << "No such ID";
    }
}

void TileLoader::update()
{
    flush(); // trigger a reload of all tiles that are currently in use
    d->m_tileCache.clear(); // clear the tile cache in physical memory
}

#include "TileLoader.moc"
