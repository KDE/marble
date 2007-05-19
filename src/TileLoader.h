/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright (C) 2005 Torsten Rahn (rahn@kde.org)
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
 * @author Torsten Rahn rahn @ kde.org
 */

class TileLoader : public QObject {
    Q_OBJECT
 public:
    TileLoader( const QString& theme );
    virtual ~TileLoader(){}

    TextureTile* loadTile( int tilx, int tily, int tileLevel );

    void setMap( const QString& );

    void resetTilehash();
    void cleanupTilehash();
    void flush();

    const int tileWidth() const { return m_tileWidth; }
    const int tileHeight() const { return m_tileHeight; }

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

    TextureTile  *m_tile;
    HttpDownloadManager *m_downloadManager;

    QString       m_theme;

    QHash <int, TextureTile*>  m_tileHash;
    int           m_tileId;

    int           m_tileWidth;
    int           m_tileHeight;

public slots:
    void message( const QString& test ){ qDebug() << test; }
};


#endif // __MARBLE__TILELOADER_H
