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

#ifndef __MARBLE__TILELOADER_H
#define __MARBLE__TILELOADER_H

#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "TileCache.h"

class TextureTile;
class HttpDownloadManager;

/**
 * @short Tile loading from a quad tree
 *
 * This class loads tiles into memory. For faster access
 * we keep the tileIDs and their respective pointers to 
 * the tiles in a hashtable.
 * The class also contains convenience methods to remove entries 
 * from the hashtable and to return more detailed properties
 * about each tile level and their tiles.
 *
 * @author Torsten Rahn <rahn@kde.org>
 **/

class FileStoragePolicy;

class TileLoader : public QObject {
    Q_OBJECT
 public:
    TileLoader( const QString& theme );
    virtual ~TileLoader();

    TextureTile* loadTile( int tilx, int tily, int tileLevel );

    void setMapTheme( const QString& );
    const QString mapTheme() const { return m_theme; }

    void resetTilehash();
    void cleanupTilehash();

    void flush();

    int tileWidth()  const { return m_tileWidth; }
    int tileHeight() const { return m_tileHeight; }

    static int levelToRow( int level );
    static int levelToColumn( int level );
    static int rowToLevel( int row );
    static int columnToLevel( int column );

    // highest level in which all tiles are available
    static int maxCompleteTileLevel( const QString& theme );

    // highest level in which some tiles are available
    static int maxPartialTileLevel( const QString& theme );

    // the mandatory most basic tile level is fully available
    static bool baseTilesAvailable( const QString& theme );

 protected:

    FileStoragePolicy *m_storagePolicy;
    HttpDownloadManager *m_downloadManager;

    QString       m_theme;

    QHash <int, TextureTile*>  m_tileHash;

    int           m_tileWidth;
    int           m_tileHeight;

    TileCache     m_tileCache;

 Q_SIGNALS:
    void tileUpdateAvailable();

public Q_SLOTS:

    void reloadTile( QString relativeUrlString, QString id );

    void message( const QString& test ){ qDebug() << test; }
};


#endif // __MARBLE__TILELOADER_H
