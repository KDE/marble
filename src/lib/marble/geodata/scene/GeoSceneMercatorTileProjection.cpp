/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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


QRect GeoSceneMercatorTileProjection::tileIndexes(const GeoDataLatLonBox &latLonBox, int zoomLevel) const
{
    const unsigned int xTileCount = (1 << zoomLevel) * levelZeroColumns();

    const int westX =   eastBoundTileXFromLon(latLonBox.west(),  xTileCount);
    const int eastX =   westBoundTileXFromLon(latLonBox.east(),  xTileCount);

    const unsigned int yTileCount = (1 << zoomLevel) * levelZeroRows();

    const int northY = southBoundTileYFromLat(latLonBox.north(), yTileCount);
    const int southY = northBoundTileYFromLat(latLonBox.south(), yTileCount);

    return QRect(QPoint(westX, northY), QPoint(eastX, southY));
}


GeoDataLatLonBox GeoSceneMercatorTileProjection::geoCoordinates(int zoomLevel, int x, int y) const
{
    const unsigned int xTileCount = (1 << zoomLevel) * levelZeroColumns();
    const unsigned int yTileCount = (1 << zoomLevel) * levelZeroRows();

    const qreal west = lonFromTileX(x, xTileCount);
    const qreal north = latFromTileY(y, yTileCount);

    const qreal east = lonFromTileX(x + 1, xTileCount);
    const qreal south = latFromTileY(y + 1, yTileCount);

    return GeoDataLatLonBox(north, south, east, west);
}

}
