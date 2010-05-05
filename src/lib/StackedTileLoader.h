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

#ifndef MARBLE_STACKEDTILELOADER_H
#define MARBLE_STACKEDTILELOADER_H

#include <QtCore/QObject>

#include "TileId.h"
#include "global.h"

class QString;

namespace Marble
{

class StackedTile;
class HttpDownloadManager;
class MapThemeManager;
class MarbleModel;
class GeoSceneGroup;
class GeoSceneLayer;
class GeoSceneTexture;

class StackedTileLoaderPrivate;

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

class StackedTileLoader : public QObject
{
    Q_OBJECT

    public:
        /**
         * Creates a new tile loader.
         *
         * @param downloadManager The download manager that shall be used to fetch
         *                        the tiles from a remote resource.
         */
        StackedTileLoader( MapThemeManager const * const mapThemeManager,
                           GeoSceneGroup * const textureLayerSettings,
                           HttpDownloadManager * const downloadManager, MarbleModel * const model );
        virtual ~StackedTileLoader();

        /**
         * Sets the download manager that shall be used to fetch the
         * tiles from a remote resource.
         */
        void setDownloadManager( HttpDownloadManager *downloadManager );
        void setTextureLayerSettings( GeoSceneGroup * const textureLayerSettings );

        /**
         * Loads a tile and returns it.
         *
         * @param stackedTileId The Id of the requested tile, containing the x and y coordinate
         *                      and the zoom level.
         */
        StackedTile* loadTile( TileId const &stackedTileId, DownloadUsage const,
                               bool const forMergedLayerDecorator = false );
        StackedTile* reloadTile( TileId const & stackedTileId, DownloadUsage const );
        void downloadTile( TileId const & stackedTileId );

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
         * Returns the highest level in which some tiles are theoretically
         * available for the given @p texture layer.
         */
        static int maximumTileLevel( GeoSceneTexture const * const textureLayer );

        /**
         * Returns whether the mandatory most basic tile level is fully available for
         * the given @p texture layer.
         */
        static bool baseTilesAvailable( GeoSceneLayer * layer );

    public Q_SLOTS:
        void reset();

        /**
         * @brief Set the limit of the volatile (in RAM) cache.
         * @param bytes The limit in kilobytes.
         */
        void setVolatileCacheLimit( quint64 kiloBytes );

        /**
         */
        void updateTile( TileId const & stackedTileId, TileId const & tileId );

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

    private Q_SLOTS:
        void updateTextureLayers();

    private:
        Q_DISABLE_COPY( StackedTileLoader )
        GeoSceneLayer const * findSceneLayer( TileId const & ) const;
        GeoSceneTexture const * findTextureLayer( TileId const & ) const;
        GeoSceneTexture * findTextureLayer( TileId const & );
        QVector<GeoSceneTexture const *>
            findRelevantTextureLayers( TileId const & stackedTileId ) const;
        void mergeDecorations( StackedTile * const, GeoSceneTexture * const ) const;
        void reloadCachedTile( StackedTile * const cachedTile, DownloadUsage const );

        StackedTileLoaderPrivate* const d;
        MarbleModel* m_parent;
};

}

#endif
