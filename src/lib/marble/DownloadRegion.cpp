// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "DownloadRegion.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineString.h"
#include "GeoSceneAbstractTileProjection.h"
#include "GeoSceneDocument.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoSceneTileDataset.h"
#include "MarbleDebug.h"
#include "MarbleMap.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "TextureLayer.h"
#include "TileCoordsPyramid.h"

namespace Marble
{

class DownloadRegionPrivate
{
public:
    MarbleModel *m_marbleModel;

    QPair<int, int> m_tileLevelRange;

    int m_visibleTileLevel;

    DownloadRegionPrivate();

    int rad2PixelX(qreal const lon, const TileLayer *tileLayer) const;

    int rad2PixelY(qreal const lat, const TileLayer *tileLayer) const;
};

DownloadRegionPrivate::DownloadRegionPrivate()
    : m_marbleModel(nullptr)
    , m_tileLevelRange(0, 0)
    , m_visibleTileLevel(0)
{
    // nothing to do
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionPrivate::rad2PixelX(qreal const lon, const TileLayer *tileLayer) const
{
    qreal tileWidth = tileLayer && tileLayer->layerCount() > 0 ? tileLayer->tileSize().width() : 256;
    qreal const globalWidth = tileWidth * tileLayer->tileColumnCount(m_visibleTileLevel);
    return static_cast<int>(globalWidth * 0.5 * (1 + lon / M_PI));
}

// copied from AbstractScanlineTextureMapper and slightly adjusted
int DownloadRegionPrivate::rad2PixelY(qreal const lat, const TileLayer *tileLayer) const
{
    qreal tileHeight = tileLayer && tileLayer->layerCount() > 0 ? tileLayer->tileSize().height() : 256;
    qreal const globalHeight = tileHeight * tileLayer->tileRowCount(m_visibleTileLevel);

    switch (tileLayer->tileProjection()->type()) {
    case GeoSceneAbstractTileProjection::Equirectangular:
        return static_cast<int>(globalHeight * (0.5 - lat / M_PI));
    case GeoSceneAbstractTileProjection::Mercator:
        if (fabs(lat) < 1.4835)
            return static_cast<int>(globalHeight * 0.5 * (1 - gdInv(lat) / M_PI));
        if (lat >= +1.4835)
            return static_cast<int>(globalHeight * 0.5 * (1 - 3.1309587 / M_PI));
        if (lat <= -1.4835)
            return static_cast<int>(globalHeight * 0.5 * (1 + 3.1309587 / M_PI));
    }

    // Dummy value to avoid a warning.
    return 0;
}

DownloadRegion::DownloadRegion(QObject *parent)
    : QObject(parent)
    , d(new DownloadRegionPrivate)
{
    // nothing to do
}

void DownloadRegion::setMarbleModel(MarbleModel *model)
{
    d->m_marbleModel = model;
}

DownloadRegion::~DownloadRegion()
{
    delete d;
}

void DownloadRegion::setTileLevelRange(const int minimumTileLevel, const int maximumTileLevel)
{
    Q_ASSERT(minimumTileLevel >= 0);
    Q_ASSERT(maximumTileLevel >= 0);
    Q_ASSERT(minimumTileLevel <= maximumTileLevel);
    d->m_tileLevelRange.first = minimumTileLevel;
    d->m_tileLevelRange.second = maximumTileLevel;
}

QList<TileCoordsPyramid> DownloadRegion::region(const TileLayer *tileLayer, const GeoDataLatLonAltBox &downloadRegion) const
{
    Q_ASSERT(tileLayer);

    int tileLevelRangeFirst = d->m_tileLevelRange.first;
    int tileLevelRangeSecond = d->m_tileLevelRange.second;

    TileType tileType = dynamic_cast<const TextureLayer *>(tileLayer) ? TextureTileType : VectorTileType;

    QList<int> validLevels;
    validLevels = validTileLevels(tileType);

    // Align the tileLevelRangeSecond with the validTileLevels
    if (!validLevels.isEmpty()) {
        int lastIndex = validLevels.count() - 1;
        for (int i = 0; i < validLevels.count(); ++i) {
            if (validLevels.at(lastIndex - i) <= tileLevelRangeSecond && validLevels.at(lastIndex - i) >= tileLevelRangeFirst) {
                tileLevelRangeSecond = validLevels.at(lastIndex - i);
                break;
            }
        }
    }

    int const westX = d->rad2PixelX(downloadRegion.west(), tileLayer);
    int const northY = d->rad2PixelY(downloadRegion.north(), tileLayer);
    int const eastX = d->rad2PixelX(downloadRegion.east(), tileLayer);
    int const southY = d->rad2PixelY(downloadRegion.south(), tileLayer);

    // FIXME: remove this stuff
    mDebug() << "DownloadRegionDialog downloadRegion:"
             << "north:" << downloadRegion.north() << "south:" << downloadRegion.south() << "east:" << downloadRegion.east()
             << "west:" << downloadRegion.west();
    mDebug() << "north/west (x/y):" << westX << northY;
    mDebug() << "south/east (x/y):" << eastX << southY;

    int const tileWidth = tileLayer->tileSize().width();
    int const tileHeight = tileLayer->tileSize().height();
    mDebug() << "DownloadRegionDialog downloadRegion: tileSize:" << tileWidth << tileHeight;

    int const visibleLevelX1 = qMin(westX, eastX);
    int const visibleLevelY1 = qMin(northY, southY);
    int const visibleLevelX2 = qMax(westX, eastX);
    int const visibleLevelY2 = qMax(northY, southY);

    mDebug() << "visible level pixel coords (level/x1/y1/x2/y2):" << d->m_visibleTileLevel << visibleLevelX1 << visibleLevelY1 << visibleLevelX2
             << visibleLevelY2;

    int bottomLevelX1, bottomLevelY1, bottomLevelX2, bottomLevelY2;
    // the pixel coords calculated above are referring to the visible tile level,
    // if the bottom level is a different level, we have to take it into account
    if (d->m_visibleTileLevel > tileLevelRangeSecond) {
        int const deltaLevel = d->m_visibleTileLevel - tileLevelRangeSecond;
        bottomLevelX1 = visibleLevelX1 >> deltaLevel;
        bottomLevelY1 = visibleLevelY1 >> deltaLevel;
        bottomLevelX2 = visibleLevelX2 >> deltaLevel;
        bottomLevelY2 = visibleLevelY2 >> deltaLevel;
    } else if (d->m_visibleTileLevel < tileLevelRangeSecond) {
        int const deltaLevel = tileLevelRangeSecond - d->m_visibleTileLevel;
        bottomLevelX1 = visibleLevelX1 << deltaLevel;
        bottomLevelY1 = visibleLevelY1 << deltaLevel;
        bottomLevelX2 = visibleLevelX2 << deltaLevel;
        bottomLevelY2 = visibleLevelY2 << deltaLevel;
    } else {
        bottomLevelX1 = visibleLevelX1;
        bottomLevelY1 = visibleLevelY1;
        bottomLevelX2 = visibleLevelX2;
        bottomLevelY2 = visibleLevelY2;
    }
    mDebug() << "bottom level pixel coords (level/x1/y1/x2/y2):" << tileLevelRangeSecond << bottomLevelX1 << bottomLevelY1 << bottomLevelX2 << bottomLevelY2;

    TileCoordsPyramid coordsPyramid(tileLevelRangeFirst, tileLevelRangeSecond);
    coordsPyramid.setValidTileLevels(validLevels);

    QRect bottomLevelTileCoords;
    bottomLevelTileCoords.setCoords(bottomLevelX1 / tileWidth,
                                    bottomLevelY1 / tileHeight,
                                    bottomLevelX2 / tileWidth + (bottomLevelX2 % tileWidth > 0 ? 1 : 0) - 1, // -1 needed for proper counting
                                    bottomLevelY2 / tileHeight + (bottomLevelY2 % tileHeight > 0 ? 1 : 0) - 1); // -1 needed for proper counting
    mDebug() << "bottom level tile coords: (x1/y1/size):" << bottomLevelTileCoords;
    coordsPyramid.setBottomLevelCoords(bottomLevelTileCoords);

    mDebug() << "tiles count:" << coordsPyramid.tilesCount();
    QList<TileCoordsPyramid> pyramid;
    pyramid << coordsPyramid;
    return pyramid;
}

void DownloadRegion::setVisibleTileLevel(const int tileLevel)
{
    d->m_visibleTileLevel = tileLevel;
}

QList<TileCoordsPyramid> DownloadRegion::fromPath(const TileLayer *tileLayer, qreal offset, const GeoDataLineString &waypoints) const
{
    if (!d->m_marbleModel) {
        return {};
    }

    int tileLevelRangeFirst = d->m_tileLevelRange.first;
    int tileLevelRangeSecond = d->m_tileLevelRange.second;

    TileType tileType = dynamic_cast<const TextureLayer *>(tileLayer) ? TextureTileType : VectorTileType;

    QList<int> validLevels;
    validLevels = validTileLevels(tileType);

    // Align the tileLevelRangeSecond with the validTileLevels
    if (!validLevels.isEmpty()) {
        int lastIndex = validLevels.count() - 1;
        for (int i = 0; i < validLevels.count(); ++i) {
            if (validLevels.at(lastIndex - i) <= tileLevelRangeSecond && validLevels.at(lastIndex - i) >= tileLevelRangeFirst) {
                tileLevelRangeSecond = validLevels.at(lastIndex - i);
                break;
            }
        }
    }

    TileCoordsPyramid coordsPyramid(tileLevelRangeFirst, tileLevelRangeSecond);
    coordsPyramid.setValidTileLevels(validLevels);

    int const tileWidth = tileLayer->tileSize().width();
    int const tileHeight = tileLayer->tileSize().height();

    qreal radius = d->m_marbleModel->planetRadius();
    QList<TileCoordsPyramid> pyramid;
    qreal radianOffset = offset / radius;

    for (int i = 1; i < waypoints.size(); ++i) {
        GeoDataCoordinates position = waypoints[i];
        qreal lonCenter = position.longitude();
        qreal latCenter = position.latitude();

        // coordinates of the of the vertices of the square(topleft and bottomright) at an offset distance from the waypoint
        qreal latNorth = asin(sin(latCenter) * cos(radianOffset) + cos(latCenter) * sin(radianOffset) * cos(7 * M_PI / 4));
        qreal dlonWest = atan2(sin(7 * M_PI / 4) * sin(radianOffset) * cos(latCenter), cos(radianOffset) - sin(latCenter) * sin(latNorth));
        qreal lonWest = fmod(lonCenter - dlonWest + M_PI, 2 * M_PI) - M_PI;
        qreal latSouth = asin(sin(latCenter) * cos(radianOffset) + cos(latCenter) * sin(radianOffset) * cos(3 * M_PI / 4));
        qreal dlonEast = atan2(sin(3 * M_PI / 4) * sin(radianOffset) * cos(latCenter), cos(radianOffset) - sin(latCenter) * sin(latSouth));
        qreal lonEast = fmod(lonCenter - dlonEast + M_PI, 2 * M_PI) - M_PI;

        int const northY = d->rad2PixelY(latNorth, tileLayer);
        int const southY = d->rad2PixelY(latSouth, tileLayer);
        int const eastX = d->rad2PixelX(lonEast, tileLayer);
        int const westX = d->rad2PixelX(lonWest, tileLayer);

        int const west = qMin(westX, eastX);
        int const north = qMin(northY, southY);
        int const east = qMax(westX, eastX);
        int const south = qMax(northY, southY);

        int bottomLevelTileX1 = 0;
        int bottomLevelTileY1 = 0;
        int bottomLevelTileX2 = 0;
        int bottomLevelTileY2 = 0;

        if (d->m_visibleTileLevel > tileLevelRangeSecond) {
            int const deltaLevel = d->m_visibleTileLevel - tileLevelRangeSecond;
            bottomLevelTileX1 = west >> deltaLevel;
            bottomLevelTileY1 = north >> deltaLevel;
            bottomLevelTileX2 = east >> deltaLevel;
            bottomLevelTileY2 = south >> deltaLevel;
        } else if (d->m_visibleTileLevel < tileLevelRangeSecond) {
            int const deltaLevel = tileLevelRangeSecond - d->m_visibleTileLevel;
            bottomLevelTileX1 = west << deltaLevel;
            bottomLevelTileY1 = north << deltaLevel;
            bottomLevelTileX2 = east << deltaLevel;
            bottomLevelTileY2 = south << deltaLevel;
        } else {
            bottomLevelTileX1 = west;
            bottomLevelTileY1 = north;
            bottomLevelTileX2 = east;
            bottomLevelTileY2 = south;
        }

        QRect waypointRegion;
        // square region around the waypoint
        waypointRegion.setCoords(bottomLevelTileX1 / tileWidth, bottomLevelTileY1 / tileHeight, bottomLevelTileX2 / tileWidth, bottomLevelTileY2 / tileHeight);
        coordsPyramid.setBottomLevelCoords(waypointRegion);
        pyramid << coordsPyramid;
    }

    return pyramid;
}

QList<int> DownloadRegion::validTileLevels(const TileType tileType) const
{
    QList<int> validTileLevels;

    GeoSceneMap *map = d->m_marbleModel->mapTheme()->map();
    QList<GeoSceneLayer *> layers = map->layers();
    for (auto layer : std::as_const(layers)) {
        if ((layer->backend() == QLatin1StringView("vectortile") && tileType == VectorTileType)
            || (layer->backend() == QLatin1StringView("texture") && tileType == TextureTileType)) {
            GeoSceneTileDataset *dataset = dynamic_cast<GeoSceneTileDataset *>(layer->datasets().first());
            validTileLevels = dataset->tileLevels();
            break;
        }
    }

    return validTileLevels;
}

}

#include "moc_DownloadRegion.cpp"
