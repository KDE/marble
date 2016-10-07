/*
    Copyright 2016 Friedrich W. H. Kossebau  <kossebau@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MARBLE_GEOSCENEABSTRACTTILEPROJECTION_H
#define MARBLE_GEOSCENEABSTRACTTILEPROJECTION_H

#include "geodata_export.h"
#include <TileId.h>

#include <QScopedPointer>

namespace Marble
{

class GeoSceneAbstractTileProjectionPrivate;

class GeoDataLatLonBox;

/**
 * @short A base class for projections between tile indizes and geo coordinates in Marble.
 *
 * For map tiling with indizes in x and y dimensions and 1 or multiple zoomlevels.
 * The lowest zoomlevel is 0.
 */
class GEODATA_EXPORT GeoSceneAbstractTileProjection
{
public:
    enum Type { Equirectangular, Mercator };

    /**
     * @brief Construct a new GeoSceneAbstractTileProjection.
     */
    GeoSceneAbstractTileProjection();

    virtual ~GeoSceneAbstractTileProjection();

public:
    virtual GeoSceneAbstractTileProjection::Type type() const = 0;

    /**
     * @return the number of tiles on level 0 in x dimension
     */
    int levelZeroColumns() const;
    /**
     * @brief Sets the number of tiles on level 0 in x dimension
     *
     * @param levelZeroColumns new number of tiles on level 0 in x dimension
     *
     * Default value of the levelZeroColumns property is 1.
     */
    void setLevelZeroColumns(int levelZeroColumns);

    /**
     * @return the number of tiles on level 0 in y dimension
     */
    int levelZeroRows() const;
    /**
     * @brief Sets the number of tiles on level 0 in y dimension
     *
     * @param levelZeroColumns new number of tiles on level 0 in y dimension
     *
     * Default value of the levelZeroRows property is 1.
     */
    void setLevelZeroRows(int levelZeroRows);

    /**
     * @brief Get the tile indexes which cover the given geographical box.
     * If @p latLonBox or @p zoomLevel have values out-of-bounds, the behaviour is undefined.
     *
     * @param latLonBox the geo coordinates of the requested tiles
     * @param zoomLevel the zoomlevel of the requested tiles
     * @param westX     the x index of the tiles covering the western boundary is returned through this parameter
     * @param northY    the y index of the tiles covering the northern boundary is returned through this parameter
     * @param eastX     the x index of the tiles covering the eastern boundary is returned through this parameter
     * @param southY    the y index of the tiles covering the southern boundary is returned through this parameter
     */
     virtual void tileIndexes(const GeoDataLatLonBox& latLonBox, int zoomLevel,
                              int& westX, int& northY, int& eastX, int& southY) const = 0;

    /**
     * @brief Get the north-west geo coordinates corresponding to a tile.
     * If @p x, @p y or @p zoomLevel have values out-of-bounds, the behaviour is undefined.
     *
     * @param zoomLevel the zoomlevel of the tile
     * @param x         the x index of the tile
     * @param y         the y index of the tile
     * @param westernTileEdgeLon  the longitude angle in radians of the western tile edge tis returned through this parameter
     * @param northernTileEdgeLat the latitude angle in radians of the northern tile edge is returned through this parameter
     */
    virtual void geoCoordinates(int zoomLevel,
                                int x, int y,
                                qreal& westernTileEdgeLon, qreal& northernTileEdgeLat) const = 0;

    /**
     * @brief Get the boundary geo coordinates corresponding to a tile.
     * If @p x, @p y or @p zoomLevel have values out-of-bounds, the behaviour is undefined.
     *
     * @param zoomLevel the zoomlevel of the tile
     * @param x         the x index of the tile
     * @param y         the y index of the tile
     * @param latLonBox the boundary geo coordinates are set to this GeoDataLatLonBox
     */
    virtual void geoCoordinates(int zoomLevel,
                                int x, int y,
                                GeoDataLatLonBox& latLonBox) const = 0;

    /**
     * @brief Get the boundary geo coordinates corresponding to a tile.
     * If @p tildId  has values out-of-bounds, the behaviour is undefined.
     *
     * @param tileId    the id of the tile
     * @param latLonBox the boundary geo coordinates are set to this GeoDataLatLonBox
     */
    void geoCoordinates(const TileId& tileId,
                        GeoDataLatLonBox& latLonBox) const
    {
        geoCoordinates(tileId.zoomLevel(), tileId.x(), tileId.y(), latLonBox);
    }

 private:
     Q_DISABLE_COPY(GeoSceneAbstractTileProjection)
     const QScopedPointer<GeoSceneAbstractTileProjectionPrivate> d_ptr;
};

}

#endif
