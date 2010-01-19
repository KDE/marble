/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 * Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
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

#ifndef MARBLE_TILELOADER_H
#define MARBLE_TILELOADER_H

#include <QtCore/QObject>

#include "TileId.h"

class QString;

namespace Marble
{

class TextureTile;
class HttpDownloadManager;
class MarbleModel;
class GeoSceneLayer;

class TileLoaderPrivate;

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

class TileLoader : public QObject
{
    Q_OBJECT

    public:
        /**
         * Creates a new tile loader.
         *
         * @param downloadManager The download manager that shall be used to fetch
         *                        the tiles from a remote resource.
         */
        TileLoader( HttpDownloadManager *downloadManager, MarbleModel* parent);

        /**
         * Destroys the tile loader.
         */
        virtual ~TileLoader();

        /**
         * Sets the download manager that shall be used to fetch the
         * tiles from a remote resource.
         */
        void setDownloadManager( HttpDownloadManager *downloadManager );

        /**
         * Loads a tile and returns it.
         *
         * @param tilx The x coordinate of the requested tile.
         * @param tily The y coordinate of the requested tile.
         * @param tileLevel The zoom level of the requested tile.
         */
        TextureTile* loadTile( int tilx, int tily, int tileLevel );

        /**
         * Sets the texture layer @p the tiles shall be loaded for.
         */
        void setLayer( GeoSceneLayer * layer );

        /**
         * Returns the texture layer the tiles shall be loaded for.
         */
        GeoSceneLayer * layer() const;

        /**
         * Resets the internal tile hash.
         */
        void resetTilehash();

        /**
         * Cleans up the internal tile hash.
         *
         * Removes all superfluous tiles from the hash.
         */
        void cleanupTilehash();

        /**
         * Clears the internal tile hash.
         *
         * Removes all tiles from the hash.
         */
        void flush();

        /**
         * Returns the width of a tile loaded by this tile loader.
         */
        int tileWidth() const;

        /**
         * Returns the height of a tile loaded by this tile loader.
         */
        int tileHeight() const;

        /**
         * Returns the global width for the given @p level.
         */
        int globalWidth( int level ) const;

        /**
         * Returns the global height for the given @p level.
         */
        int globalHeight( int level ) const;

        /**
         * @brief  Returns the limit of the volatile (in RAM) cache.
         * @return the cache limit in kilobytes
         */
        quint64 volatileCacheLimit() const;

        /**
         * @brief Returns a list of TileIds of the tiles which are currently
         *        displayed. This is used for example for the map reload
         *        functionality.
         */
        QList<TileId> tilesOnDisplay() const;

        /**
         * Returns the highest level in which some tiles are available for the given @p
         * texture layer.
         */
        static int maxPartialTileLevel( GeoSceneLayer * layer );

        /**
         * Returns whether the mandatory most basic tile level is fully available for
         * the given @p texture layer.
         */
        static bool baseTilesAvailable( GeoSceneLayer * layer );

    public Q_SLOTS:
        /**
         * @brief Set the limit of the volatile (in RAM) cache.
         * @param bytes The limit in kilobytes.
         */
        void setVolatileCacheLimit( quint64 kiloBytes );

        /**
         * Reloads the tile with the given @p id.
         */
        void reloadTile( const QString &relativeUrlString, const QString &id );

        /**
         * Effectively triggers a reload of all tiles that are currently in use
         * and clears the tile cache in physical memory.
         */
        void update();

    Q_SIGNALS:
        /**
         * This signal is emitted whenever a requested tile has been
         * downloaded and is available now.
         */
        void tileUpdateAvailable();
        
        void paintTile(TextureTile* tile, int x, int y, int level,
                       GeoSceneLayer * layer,
                       bool requestTileUpdate);

    private:
        /**
         * @brief Helper method for the slots reloadTile.
         */
        void reloadTile( const QString &idStr );

        Q_DISABLE_COPY( TileLoader )

        TileLoaderPrivate* const d;
        MarbleModel* m_parent;
};

}

#endif // MARBLE_TILELOADER_H
