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

#include "GeoSceneMercatorTileProjection.h"

#include <GeoDataLatLonBox.h>
#include <MarbleMath.h>


namespace Marble
{

GeoSceneMercatorTileProjection::GeoSceneMercatorTileProjection()
{
}


GeoSceneMercatorTileProjection::~GeoSceneMercatorTileProjection()
{
}

GeoSceneAbstractTileProjection::Type GeoSceneMercatorTileProjection::type() const
{
    return Mercator;
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
qreal baseTileYFromLat(qreal latitude, unsigned int tileCount)
{
    // We need to calculate the tile position from the latitude
    // projected using the Mercator projection. This requires the inverse Gudermannian
    // function which is only defined between -85°S and 85°N. Therefore in order to
    // prevent undefined results we need to restrict our calculation.
    // Using 85.0 instead of some more correct 85.05113, to avoid running into NaN issues.
    qreal maxAbsLat = 85.0 * DEG2RAD;
    qreal lat = (qAbs(latitude) > maxAbsLat) ? latitude/qAbs(latitude) * maxAbsLat : latitude;
    return (0.5 * (1.0 - gdInv(lat) / M_PI) * tileCount);
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
        // calculate with normal lat value
        lat = M_PI * 0.5;
    }
    return upperBoundTileIndex(baseTileYFromLat(lat, tileCount));
}

// on tile borders selects the tile to the north
static inline
unsigned int northBoundTileYFromLat(qreal lat, unsigned int tileCount)
{
    // special casing tile-map end
    if (lat == M_PI*0.5) {
        // calculate with normal lat value
        lat = - M_PI * 0.5;
    }
    return lowerBoundTileIndex(baseTileYFromLat(lat, tileCount));
}


static inline
qreal lonFromTileX(unsigned int x, unsigned int tileCount)
{
    return ( (2*M_PI * x) / tileCount - M_PI );
}

static inline
qreal latFromTileY(unsigned int y, unsigned int tileCount)
{
    return gd(M_PI * (1.0 - (2.0 * y) / tileCount));
}


void GeoSceneMercatorTileProjection::tileIndexes(const GeoDataLatLonBox& latLonBox, int zoomLevel,
                                                 int& westX, int& northY, int& eastX, int& southY) const
{
    const unsigned int xTileCount = (1 << zoomLevel) * levelZeroColumns();

    westX =   eastBoundTileXFromLon(latLonBox.west(),  xTileCount);
    eastX =   westBoundTileXFromLon(latLonBox.east(),  xTileCount);

    const unsigned int yTileCount = (1 << zoomLevel) * levelZeroRows();

    northY = southBoundTileYFromLat(latLonBox.north(), yTileCount);
    southY = northBoundTileYFromLat(latLonBox.south(), yTileCount);
}

void GeoSceneMercatorTileProjection::geoCoordinates(int zoomLevel,
                                                    int x, int y,
                                                    qreal& westernTileEdgeLon, qreal& northernTileEdgeLat) const
{
    const unsigned int xTileCount = (1 << zoomLevel) * levelZeroColumns();
    westernTileEdgeLon = lonFromTileX(x, xTileCount);

    const unsigned int yTileCount = (1 << zoomLevel) * levelZeroRows();
    northernTileEdgeLat = latFromTileY(y, yTileCount);
}


void GeoSceneMercatorTileProjection::geoCoordinates(int zoomLevel,
                                                    int x, int y,
                                                    GeoDataLatLonBox& latLonBox) const
{
    const unsigned int xTileCount = (1 << zoomLevel) * levelZeroColumns();

    const qreal west = lonFromTileX(x,     xTileCount);
    const qreal east = lonFromTileX(x + 1, xTileCount);

    const unsigned int yTileCount = (1 << zoomLevel) * levelZeroRows();

    const qreal north = latFromTileY(y,     yTileCount);
    const qreal south = latFromTileY(y + 1, yTileCount);

    latLonBox.setBoundaries(north, south, east, west);
}

}
