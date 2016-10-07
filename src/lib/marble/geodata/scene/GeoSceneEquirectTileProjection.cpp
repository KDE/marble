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

#include "GeoSceneEquirectTileProjection.h"

#include <GeoDataLatLonBox.h>
#include <MarbleMath.h>

namespace Marble
{

GeoSceneEquirectTileProjection::GeoSceneEquirectTileProjection()
{
}


GeoSceneEquirectTileProjection::~GeoSceneEquirectTileProjection()
{
}

GeoSceneAbstractTileProjection::Type GeoSceneEquirectTileProjection::type() const
{
    return Equirectangular;
}


static inline
unsigned int lowerBoundTileIndex(qreal baseTileIndex)
{
    const qreal floorBaseTileIndex = floor(baseTileIndex);
    unsigned int tileIndex = static_cast<unsigned int>(floorBaseTileIndex);
    return (baseTileIndex == floorBaseTileIndex) ? tileIndex-1 : tileIndex;
}

static inline
unsigned int upperBoundTileIndex(qreal baseTileIndex)
{
    return (unsigned int)floor(baseTileIndex);
}

static inline
qreal baseTileXFromLon(qreal lon, unsigned int tileCount)
{
    return 0.5 * (lon / M_PI + 1.0) * tileCount;
}

static inline
qreal baseTileYFromLat(qreal lat, unsigned int tileCount)
{
    return (0.5 - lat / M_PI) * tileCount;
}


// on tile borders selects the tile to the east
static inline
unsigned int eastBoundTileXFromLon(qreal lon, unsigned int tileCount)
{
    // special casing tile-map end
    if (lon == M_PI) {
        return 0;
    }
    return upperBoundTileIndex(baseTileXFromLon(lon, tileCount));
}

// on tile borders selects the tile to the west
static inline
unsigned int westBoundTileXFromLon(qreal lon, unsigned int tileCount)
{
    // special casing tile-map end
    if (lon == -M_PI) {
        return tileCount-1;
    }
    return lowerBoundTileIndex(baseTileXFromLon(lon, tileCount));
}

// on tile borders selects the tile to the south
static inline
unsigned int southBoundTileYFromLat(qreal lat, unsigned int tileCount)
{
    // special casing tile-map end
    if (lat == -M_PI*0.5) {
        return 0;
    }
    return upperBoundTileIndex(baseTileYFromLat(lat, tileCount));
}

// on tile borders selects the tile to the north
static inline
unsigned int northBoundTileYFromLat(qreal lat, unsigned int tileCount)
{
    // special casing tile-map end
    if (lat == M_PI*0.5) {
        return tileCount-1;
    }
    return lowerBoundTileIndex(baseTileYFromLat(lat, tileCount));
}


void GeoSceneEquirectTileProjection::tileIndexes(const GeoDataLatLonBox& latLonBox, int zoomLevel,
                                                 int& westX, int& northY, int& eastX, int& southY) const
{
    const unsigned int xTileCount = (1 << zoomLevel) * levelZeroColumns();

    westX =   eastBoundTileXFromLon(latLonBox.west(),  xTileCount);
    eastX =   westBoundTileXFromLon(latLonBox.east(),  xTileCount);

    const unsigned int yTileCount = (1 << zoomLevel) * levelZeroRows();

    northY = southBoundTileYFromLat(latLonBox.north(), yTileCount);
    southY = northBoundTileYFromLat(latLonBox.south(), yTileCount);
}

void GeoSceneEquirectTileProjection::geoCoordinates(int zoomLevel,
                                                    int x, int y,
                                                    qreal& westernTileEdgeLon, qreal& northernTileEdgeLat) const
{
    qreal radius = (1 << zoomLevel) * levelZeroColumns() / 2.0;

    westernTileEdgeLon  = (x - radius ) / radius * M_PI;

    radius = (1 << zoomLevel) * levelZeroRows() / 2.0;

    northernTileEdgeLat = (radius - y) / radius *  M_PI / 2.0;
}

void GeoSceneEquirectTileProjection::geoCoordinates(int zoomLevel,
                                                    int x, int y,
                                                    GeoDataLatLonBox& latLonBox) const
{
    qreal radius = (1 << zoomLevel) * levelZeroColumns() / 2.0;

    qreal lonLeft   = (x - radius ) / radius * M_PI;
    qreal lonRight  = (x - radius + 1 ) / radius * M_PI;

    radius = (1 << zoomLevel) * levelZeroRows() / 2.0;

    qreal latTop = (radius - y) / radius *  M_PI / 2.0;
    qreal latBottom = (radius - y - 1) / radius *  M_PI / 2.0;

    latLonBox.setBoundaries(latTop, latBottom, lonRight, lonLeft);
}

}
