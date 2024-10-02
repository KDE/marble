/*
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlStorageLayoutTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"
#include "ServerLayout.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(StorageLayout)

GeoNode *DgmlStorageLayoutTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_StorageLayout)));

    // Attribute levelZeroColumns, default to value of the oldest tile themes
    int levelZeroColumns = 2;
    const QString levelZeroColumnsStr = parser.attribute(dgmlAttr_levelZeroColumns).trimmed();
    if (!levelZeroColumnsStr.isEmpty()) {
        levelZeroColumns = levelZeroColumnsStr.toInt();
    }

    // Attribute levelZeroRows, default to value of the oldest tile themes
    int levelZeroRows = 1;
    const QString levelZeroRowsStr = parser.attribute(dgmlAttr_levelZeroRows).trimmed();
    if (!levelZeroRowsStr.isEmpty()) {
        levelZeroRows = levelZeroRowsStr.toInt();
    }

    // Attribute minimumTileLevel
    int minimumTileLevel = 0;
    const QString minimumTileLevelStr = parser.attribute(dgmlAttr_minimumTileLevel).trimmed();
    if (!minimumTileLevelStr.isEmpty()) {
        minimumTileLevel = minimumTileLevelStr.toInt();
    }

    // Attribute maximumTileLevel
    int maximumTileLevel = -1;
    const QString maximumTileLevelStr = parser.attribute(dgmlAttr_maximumTileLevel).trimmed();
    if (!maximumTileLevelStr.isEmpty()) {
        maximumTileLevel = maximumTileLevelStr.toInt();
    }

    // Attribute maximumTileLevel
    const QString tileLevels = parser.attribute(dgmlAttr_tileLevels).trimmed();

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Texture) || parentItem.represents(dgmlTag_Vectortile)) {
        auto texture = parentItem.nodeAs<GeoSceneTileDataset>();

        // Attribute mode
        GeoSceneTileDataset::StorageLayout storageLayout = GeoSceneTileDataset::OpenStreetMap;
        ServerLayout *serverLayout = nullptr;
        const QString modeStr = parser.attribute(dgmlAttr_mode).trimmed();
        if (modeStr == QLatin1StringView("OpenStreetMap"))
            serverLayout = new OsmServerLayout(texture);
        else if (modeStr == QLatin1StringView("Custom"))
            serverLayout = new CustomServerLayout(texture);
        else if (modeStr == QLatin1StringView("WebMapService"))
            serverLayout = new WmsServerLayout(texture);
        else if (modeStr == QLatin1StringView("WebMapTileService"))
            serverLayout = new WmtsServerLayout(texture);
        else if (modeStr == QLatin1StringView("QuadTree"))
            serverLayout = new QuadTreeServerLayout(texture);
        else if (modeStr == QLatin1StringView("TileMapService")) {
            storageLayout = GeoSceneTileDataset::TileMapService;
            serverLayout = new TmsServerLayout(texture);
        } else {
            storageLayout = GeoSceneTileDataset::Marble;
            serverLayout = new MarbleServerLayout(texture);

            if (!modeStr.isEmpty()) {
                mDebug() << "Unknown storage layout mode " << modeStr << ", falling back to default.";
            }
        }

        texture->setLevelZeroColumns(levelZeroColumns);
        texture->setLevelZeroRows(levelZeroRows);
        texture->setMinimumTileLevel(minimumTileLevel);
        texture->setMaximumTileLevel(maximumTileLevel);
        texture->setTileLevels(tileLevels);
        texture->setStorageLayout(storageLayout);
        texture->setServerLayout(serverLayout);
    }

    return nullptr;
}

}
}
