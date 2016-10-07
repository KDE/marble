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

#ifndef MARBLE_GEOSCENEMERCATORTILEPROJECTION_H
#define MARBLE_GEOSCENEMERCATORTILEPROJECTION_H

#include "GeoSceneAbstractTileProjection.h"

namespace Marble
{

/**
 * Convertes the x and y indizes of tiles to and from geo coordinates.
 * For tiles of maps in Mercator projection.
 *
 * Tiles do have the same width and the same height per zoomlevel.
 * The number of tiles per dimension is twice that of the previous lower zoomlevel.
 * The indexing is done in x dimension eastwards, with the first tiles beginning at -180 degree
 * and an x value of 0 and the last tiles ending at +180 degree,
 * in y dimension southwards with the first tiles beginning at +85.05113 degree and a y value of 0
 * and the last tiles ending at -85.05113 degree.
 *
 * NOTE: The method @c tileIndexes() handles any latitude value >= +85.0 degree as
 * exactly +85.0 degree and any latitude value <= -85.0 as exactly -85.0 degree.
 * So for higher zoomlevels the outermost tiles will be masked by that and not included in any results.
 */
class GEODATA_EXPORT GeoSceneMercatorTileProjection : public GeoSceneAbstractTileProjection
{
public:
    /**
     * @brief Construct a new GeoSceneMercatorTileProjection.
     */
    GeoSceneMercatorTileProjection();

    ~GeoSceneMercatorTileProjection() override;

public:
    /**
     * @copydoc
     */
    GeoSceneAbstractTileProjection::Type type() const override;

    /**
     * @copydoc
     */
    void tileIndexes(const GeoDataLatLonBox& latLonBox, int zoomLevel,
                     int& westX, int& northY, int& eastX, int& southY) const override;

    /**
     * @copydoc
     */
    void geoCoordinates(int zoomLevel,
                        int x, int y,
                        qreal& westernTileEdgeLon, qreal& northernTileEdgeLat) const override;

    /**
     * @copydoc
     */
    void geoCoordinates(int zoomLevel,
                        int x, int y,
                        GeoDataLatLonBox& latLonBox) const override;

    using GeoSceneAbstractTileProjection::geoCoordinates;

private:
    Q_DISABLE_COPY(GeoSceneMercatorTileProjection)
};

}

#endif
