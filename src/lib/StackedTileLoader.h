/*
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 * Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
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

#ifndef MARBLE_STACKEDTILELOADER_H
#define MARBLE_STACKEDTILELOADER_H

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QVector>

#include "GeoSceneTiled.h"
#include "TileId.h"
#include "MarbleGlobal.h"

class QImage;
class QString;

namespace Marble
{

class MergedLayerDecorator;
class StackedTile;

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
        explicit StackedTileLoader( MergedLayerDecorator *mergedLayerDecorator, QObject *parent = 0 );
        virtual ~StackedTileLoader();

        void setTextureLayers( QVector<GeoSceneTiled const *> & );

        int tileColumnCount( int level ) const;

        int tileRowCount( int level ) const;

        GeoSceneTiled::Projection tileProjection() const;

        QSize tileSize() const;

        /**
         * Loads a tile and returns it.
         *
         * @param stackedTileId The Id of the requested tile, containing the x and y coordinate
         *                      and the zoom level.
         */
        const StackedTile* loadTile( TileId const &stackedTileId );
        void downloadStackedTile( TileId const & stackedTileId );

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
         * @brief  Returns the limit of the volatile (in RAM) cache.
         * @return the cache limit in kilobytes
         */
        quint64 volatileCacheLimit() const;

        /**
         * @brief Reloads the tiles that are currently displayed.
         */
        void reloadVisibleTiles();

        /**
         * Returns the highest level in which some tiles are theoretically
         * available for the current texture layers.
         */
        int maximumTileLevel() const;

        /**
         * @brief Return the number of tiles in the cache.
         * @return number of tiles in cache
         */
        int tileCount() const;

        /**
         * @brief Set the limit of the volatile (in RAM) cache.
         * @param bytes The limit in kilobytes.
         */
        void setVolatileCacheLimit( quint64 kiloBytes );

        /**
         * Effectively triggers a reload of all tiles that are currently in use
         * and clears the tile cache in physical memory.
         */
        void clear();

        /**
         */
        void updateTile( TileId const & tileId, QImage const &tileImage );

    Q_SIGNALS:
        void tileLoaded( TileId const &tileId );
        void cleared();

    private:
        Q_DISABLE_COPY( StackedTileLoader )

        friend class StackedTileLoaderPrivate;
        StackedTileLoaderPrivate* const d;
};

}

#endif
