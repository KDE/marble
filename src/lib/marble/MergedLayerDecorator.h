// SPDX-FileCopyrightText: 2008 David Roberts <dvdr18@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_MERGEDLAYERDECORATOR_H
#define MARBLE_MERGEDLAYERDECORATOR_H

#include <QList>

#include "MarbleGlobal.h"

class QImage;
class QString;
class QSize;

namespace Marble
{

class GeoDataGroundOverlay;
class GeoSceneAbstractTileProjection;
class GeoSceneTextureTileDataset;
class SunLocator;
class StackedTile;
class Tile;
class TileId;
class TileLoader;
class RenderState;

class MergedLayerDecorator
{
public:
    MergedLayerDecorator(TileLoader *const tileLoader, const SunLocator *sunLocator);
    virtual ~MergedLayerDecorator();

    void setTextureLayers(const QList<const GeoSceneTextureTileDataset *> &textureLayers);
    void updateGroundOverlays(const QList<const GeoDataGroundOverlay *> &groundOverlays);

    int textureLayersSize() const;

    /**
     * Returns the highest level in which some tiles are theoretically
     * available for the current texture layers.
     */
    int maximumTileLevel() const;

    int tileColumnCount(int level) const;

    int tileRowCount(int level) const;

    const GeoSceneAbstractTileProjection *tileProjection() const;

    QSize tileSize() const;

    StackedTile *loadTile(const TileId &id);

    StackedTile *updateTile(const StackedTile &stackedTile, const TileId &tileId, const QImage &tileImage);

    void downloadStackedTile(const TileId &id, DownloadUsage usage);

    void setShowSunShading(bool show);
    bool showSunShading() const;

    void setShowCityLights(bool show);
    bool showCityLights() const;

    void setShowTileId(bool show);

    RenderState renderState(const TileId &stackedTileId) const;

    bool hasTextureLayer() const;

protected:
    Q_DISABLE_COPY(MergedLayerDecorator)

    class Private;
    Private *const d;
};

}

#endif
