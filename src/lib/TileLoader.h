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

#include "SunLocator.h"

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

class TileLoader : public QObject {
    Q_OBJECT
 public:
    explicit TileLoader( HttpDownloadManager *downloadManager, 
                         SunLocator* sunLocator = 0 );
    TileLoader( const QString& theme,
                HttpDownloadManager *downloadManager,
                SunLocator* sunLocator = 0 );
    virtual ~TileLoader();

    void setDownloadManager( HttpDownloadManager *downloadManager );

    TextureTile* loadTile( int tilx, int tily, int tileLevel );

    void setMapTheme( const QString& );
    const QString mapTheme() const { return m_theme; }

    void resetTilehash();
    void cleanupTilehash();

    void flush();

    int tileWidth()  const { return m_tileWidth; }
    int tileHeight() const { return m_tileHeight; }

    int globalWidth( int level ) const 
    {
        return m_tileWidth * levelToColumn( level );
    };
    int globalHeight( int level ) const 
    {
        return m_tileHeight * levelToRow( level );
    };

    /**
     * @brief  Returns the limit of the volatile (in RAM) cache.
     * @return the cache limit in bytes
     */
    quint64 volatileCacheLimit() const;

    /**
     * @brief Get the maximum number of tile rows for a given tile level.
     * @param level  the tile level
     * @return       the maximum number of rows that a map level was tiled into. 
     *               If the tile level number is invalid then "-1" gets 
     *               returned so this case of wrong input data can get caught 
     *               by the code which makes use of it.
     */
    static int levelToRow( int level );

    /**
     * @brief Get the maximum number of tile columns for a given tile level.
     * @param level  the tile level
     * @return       the maximum number of columns that a map level was tiled into. 
     *               If the tile level number is invalid then "-1" gets 
     *               returned so this case of wrong input data can get caught 
     *               by the code which makes use of it.
     */
    static int levelToColumn( int level );

    /**
     * @brief Get the tile level for the given maximum number of tile columns.
     * @param row    the maximum number of rows that a map level was tiled into.
     * @return       the corresponding tile level.
     *               If the number of rows is invalid then "-1" gets 
     *               returned so this case of wrong input data can get caught 
     *               by the code which makes use of it.
     */
    static int rowToLevel( int row );

    /**
     * @brief Get the tile level for the given maximum number of tile columns.
     * @param column the maximum number of columns that a map level was tiled into.
     * @return       the corresponding tile level.
     *               If the number of columns is invalid then "-1" gets 
     *               returned so this case of wrong input data can get caught 
     *               by the code which makes use of it.
     */
    static int columnToLevel( int column );

    // highest level in which all tiles are available
    static int maxCompleteTileLevel( const QString& theme );

    // highest level in which some tiles are available
    static int maxPartialTileLevel( const QString& theme );

    // the mandatory most basic tile level is fully available
    static bool baseTilesAvailable( const QString& theme );

    void update();

 private:
    void init();

 protected:

    HttpDownloadManager *m_downloadManager;

    QString       m_theme;

    QHash <int, TextureTile*>  m_tileHash;

    int           m_tileWidth;
    int           m_tileHeight;

    TileCache     m_tileCache;
    
    SunLocator* m_sunLocator;

 Q_SIGNALS:
    void tileUpdateAvailable();

public Q_SLOTS:

    /**
     * @brief Set the limit of the volatile (in RAM) cache.
     * @param bytes The limit in bytes.
     */
    void setVolatileCacheLimit( quint64 bytes );

    void reloadTile( QString relativeUrlString, QString id );

    void message( const QString& test ){ qDebug() << test; }
};


#endif // __MARBLE__TILELOADER_H
