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
#include "katlasdirs.h"
#include "TextureTile.h"

#include <QtCore/QDebug>
#include <QtCore/QObject>

#ifdef Q_CC_MSVC
# ifndef KDEWIN_MATH_H
   long double log(int i) { return log((long double)i); }
# endif
#endif

const int tileDigits = 6;


TileLoader::TileLoader( const QString& theme )
{
    setMapTheme( theme );
    m_downloadManager = new HttpDownloadManager( QUrl("http://download.kde.org/apps/marble/") );
    connect( m_downloadManager, SIGNAL( downloadComplete( QString, int ) ), 
        this, SLOT( reloadTile( QString, int ) ) );

    m_tileCache.clear();
    m_tileCache.setCacheLimit( 20000 ); // Cache size measured in kiloByte
}

TileLoader::~TileLoader()
{
    cleanupTilehash();
    m_downloadManager->disconnect();
    delete m_downloadManager;
}

void TileLoader::setMapTheme( const QString& theme )
{
    // Initialize map theme.
    flush();
    m_tileCache.clear();

    m_theme = theme;

    m_tile = new TextureTile( 0 );
    m_tile->loadTile( 0, 0, 0, m_theme, false );

    // We assume that all tiles have the same size. TODO: check to be safe
    m_tileWidth  = m_tile->rawtile().width();
    m_tileHeight = m_tile->rawtile().height();

    delete m_tile;
}

void TileLoader::resetTilehash()
{
    QHash<int, TextureTile*>::const_iterator it = m_tileHash.constBegin();
    while ( it != m_tileHash.constEnd() ) {
        it.value()->setUsed( false );
        ++it;
    }
}

void TileLoader::cleanupTilehash()
{
    // Make sure that tiles which haven't been used during the last
    // rendering of the map at all get removed from the tile hash.

    QHashIterator<int, TextureTile*> it(m_tileHash);
    while (it.hasNext()) {
        it.next();
        if ( it.value()->used() == false ){

            bool inCache = m_tileCache.insert( it.key(), it.value() );
            m_tileHash.remove( it.key() );
            if ( inCache == false )
                delete it.value();
        }
    }
}

void TileLoader::flush()
{
    // Remove all tiles from m_tileHash
    QHashIterator<int, TextureTile*> it(m_tileHash);
    while (it.hasNext()) {
        it.next();

        bool inCache = m_tileCache.insert( it.key(), it.value() );
        m_tileHash.remove( it.key() );
        if ( inCache == false )
            delete it.value();
    }

    m_tileHash.clear();
}


TextureTile* TileLoader::loadTile( int tilx, int tily, int tileLevel )
{
    // Choosing the correct tile via Lng/Lat info 

    int tileId = tileLevel * 100000000 + ( tily * 10000 ) + tilx;
    // If the tile hasn't been loaded into the m_tileHash yet, then do so...
    if ( !m_tileHash.contains( tileId ) ) {
        if ( m_tileCache.contains( tileId ) )
        {
            m_tile = m_tileCache.take( tileId );
            m_tileHash[tileId] = m_tile;
        }
        else
        {
//            qDebug() << "load Tile from Disk: " << tileId;
            m_tile = new TextureTile( tileId );
            m_tileHash[tileId] = m_tile;

            connect( m_tile,            SIGNAL( downloadTile( const QString&, int ) ), 
                     m_downloadManager, SLOT( addJob( const QString&, int ) ) );
            connect( m_tile,            SIGNAL( tileUpdateDone() ), 
                 this,              SIGNAL( tileUpdateAvailable() ) );
            m_tile->loadTile( tilx, tily, tileLevel, m_theme, false );
        }
    } 

    // ...otherwise pick the correct one from the hash
    else {
        m_tile = m_tileHash.value( tileId );
        if ( !m_tile->used() ) {
            m_tile->setUsed( true );
        }
    }

    return m_tile;
}

int TileLoader::levelToRow( int level )
{
    return (int)pow( 2.0, (double)( level ) );
}

int TileLoader::levelToColumn( int level )
{
    return (int)pow( 2.0, (double)( level + 1 ) );
}

int TileLoader::rowToLevel( int row )
{
    return (int)( log( row ) / log( 2 ) );
}

int TileLoader::columnToLevel( int column )
{
    return (int)( log( column / 2 ) / log( 2 ) );
}

int TileLoader::maxCompleteTileLevel( const QString& theme )
{
    bool  noerr = true; 

    int   tilelevel = -1;
    int   trylevel  = 0;

    // if ( m_bitmaplayer.type.toLower() == "bitmap" ){
    while ( noerr == true ) {
        int nmaxit = TileLoader::levelToRow( trylevel );

        for ( int n = 0; n < nmaxit; ++n ) {
            int mmaxit = TileLoader::levelToColumn( trylevel );

            for ( int m = 0; m < mmaxit; ++m ) {
                QString tilepath = KAtlasDirs::path( QString("%1/%2/%3/%3_%4.jpg")
                                                     .arg(theme).arg( trylevel ).arg( n, tileDigits, 10, QChar('0') ).arg( m, tileDigits, 10, QChar('0') ) );
                // qDebug() << tilepath;
                noerr = QFile::exists( tilepath );
                if ( noerr == false )
                    break; 
            }
            if ( noerr == false )
                break; 
        }	

        if ( noerr == true)
            tilelevel = trylevel;
        ++trylevel;
    }

    if ( tilelevel == -1 ){
        qDebug("No Tiles Found!");
    }

    qDebug() << "Detected maximum complete tile level: " << tilelevel;

    return tilelevel;
}


int TileLoader::maxPartialTileLevel( const QString& theme )
{
    QString     tilepath = KAtlasDirs::path( QString("%1").arg(theme) );
    QStringList leveldirs = ( QDir( tilepath ) ).entryList( QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );

    int      maxtilelevel = -1;
    QString  str;
    bool     ok = true;

    QStringList::const_iterator  constIterator;
    for ( constIterator = leveldirs.constBegin();
          constIterator != leveldirs.constEnd();
         ++constIterator)
    {
        int value = (*constIterator).toInt( &ok, 10 );
        // qDebug() << "Value: " << value  << "Ok: " << ok;
        if ( ok && value > maxtilelevel )
            maxtilelevel = value;
    }

    qDebug() << "Detected maximum tile level that contains data: "
             << maxtilelevel;

    return maxtilelevel;
}


bool TileLoader::baseTilesAvailable( const QString& theme )
{
    bool  noerr = true; 

    // Check whether the two tiles from the lowest texture level are available
    for ( int m = 0; m < 2; ++m ) {
        QString tilepath = KAtlasDirs::path( QString("%1/%2/%3/%3_%4.jpg").arg(theme).arg( 0 ).arg( 0, tileDigits, 10, QChar('0') ).arg( m, tileDigits, 10, QChar('0') ) );

        noerr = QFile::exists( tilepath );

        if ( noerr == false ) 
            break; 
    }

    // qDebug() << "Mandatory most basic tile level is fully available: " << noerr;

    return noerr;
}

void TileLoader::reloadTile( QString relativeUrlString, int id )
{
    qDebug() << "Reloading Tile" << relativeUrlString << "id:" << id;

    if ( m_tileHash.contains(id) )
    {
        int level =  id / 100000000;
        int y = ( id - level * 100000000 ) / 10000;
        int x = id - ( level * 100000000 + y * 10000 );
        (m_tileHash[id]) -> reloadTile( x, y, level, m_theme );
    }
    else
    {
         qDebug() << "No such ID";
    }
}

#include "TileLoader.moc"
