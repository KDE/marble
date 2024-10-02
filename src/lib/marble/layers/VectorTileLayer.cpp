/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>
    SPDX-FileCopyrightText: 2010 Thibaut Gridel <tgridel@free.fr>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
    SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#include "VectorTileLayer.h"

#include <QThreadPool>
#include <qmath.h>

#include "GeoDataDocument.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoPainter.h"
#include "GeoSceneAbstractTileProjection.h"
#include "GeoSceneGroup.h"
#include "GeoSceneTypes.h"
#include "GeoSceneVectorTileDataset.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "RenderState.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "VectorTileModel.h"
#include "ViewportParams.h"

namespace Marble
{

class Q_DECL_HIDDEN VectorTileLayer::Private
{
public:
    Private(HttpDownloadManager *downloadManager, const PluginManager *pluginManager, VectorTileLayer *parent, GeoDataTreeModel *treeModel);

    ~Private();

    void updateTile(const TileId &tileId, GeoDataDocument *document);
    void updateLayerSettings();

    QList<const GeoSceneVectorTileDataset *> findRelevantVectorLayers(const TileId &stackedTileId) const;

public:
    VectorTileLayer *const m_parent;
    TileLoader m_loader;
    QList<VectorTileModel *> m_tileModels;
    QList<VectorTileModel *> m_activeTileModels;
    const GeoSceneGroup *m_layerSettings;

    // TreeModel for displaying GeoDataDocuments
    GeoDataTreeModel *const m_treeModel;

    QThreadPool m_threadPool; // a shared thread pool for all layers to keep CPU usage sane
};

VectorTileLayer::Private::Private(HttpDownloadManager *downloadManager,
                                  const PluginManager *pluginManager,
                                  VectorTileLayer *parent,
                                  GeoDataTreeModel *treeModel)
    : m_parent(parent)
    , m_loader(downloadManager, pluginManager)
    , m_tileModels()
    , m_activeTileModels()
    , m_layerSettings(nullptr)
    , m_treeModel(treeModel)
{
    m_threadPool.setMaxThreadCount(1);
}

VectorTileLayer::Private::~Private()
{
    qDeleteAll(m_activeTileModels);
}

void VectorTileLayer::Private::updateTile(const TileId &tileId, GeoDataDocument *document)
{
    for (VectorTileModel *mapper : std::as_const(m_activeTileModels)) {
        mapper->updateTile(tileId, document);
    }
}

void VectorTileLayer::Private::updateLayerSettings()
{
    m_activeTileModels.clear();

    for (VectorTileModel *candidate : std::as_const(m_tileModels)) {
        bool enabled = true;
        if (m_layerSettings) {
            const bool propertyExists = m_layerSettings->propertyValue(candidate->name(), enabled);
            enabled |= !propertyExists; // if property doesn't exist, enable layer nevertheless
        }
        if (enabled) {
            m_activeTileModels.append(candidate);
            mDebug() << "enabling vector layer" << candidate->name();
        } else {
            candidate->clear();
            mDebug() << "disabling vector layer" << candidate->name();
        }
    }
}

VectorTileLayer::VectorTileLayer(HttpDownloadManager *downloadManager, const PluginManager *pluginManager, GeoDataTreeModel *treeModel)
    : TileLayer()
    , d(new Private(downloadManager, pluginManager, this, treeModel))
{
    qRegisterMetaType<TileId>("TileId");
    qRegisterMetaType<GeoDataDocument *>("GeoDataDocument*");

    connect(&d->m_loader, SIGNAL(tileCompleted(TileId, GeoDataDocument *)), this, SLOT(updateTile(TileId, GeoDataDocument *)));
}

VectorTileLayer::~VectorTileLayer()
{
    delete d;
}

RenderState VectorTileLayer::renderState() const
{
    return RenderState(QStringLiteral("Vector Tiles"));
}

int VectorTileLayer::tileZoomLevel() const
{
    int level = -1;
    for (const auto mapper : std::as_const(d->m_activeTileModels)) {
        level = qMax(level, mapper->tileZoomLevel());
    }
    return level;
}

QString VectorTileLayer::runtimeTrace() const
{
    int tiles = 0;
    for (const auto mapper : std::as_const(d->m_activeTileModels)) {
        tiles += mapper->cachedDocuments();
    }
    int const layers = d->m_activeTileModels.size();
    return QStringLiteral("Vector Tiles: %1 tiles in %2 layers").arg(tiles).arg(layers);
}

bool VectorTileLayer::render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer)
{
    Q_UNUSED(painter);
    Q_UNUSED(renderPos);
    Q_UNUSED(layer);

    int const oldLevel = tileZoomLevel();
    int level = 0;
    for (VectorTileModel *mapper : std::as_const(d->m_activeTileModels)) {
        mapper->setViewport(viewport->viewLatLonAltBox());
        level = qMax(level, mapper->tileZoomLevel());
    }
    if (oldLevel != level && level >= 0) {
        Q_EMIT tileLevelChanged(level);
    }

    return true;
}

void VectorTileLayer::reload()
{
    for (auto mapper : std::as_const(d->m_activeTileModels)) {
        mapper->reload();
    }
}

QSize VectorTileLayer::tileSize() const
{
    return {256, 256};
}

const GeoSceneAbstractTileProjection *VectorTileLayer::tileProjection() const
{
    if (!d->m_activeTileModels.isEmpty())
        return d->m_activeTileModels.first()->layer()->tileProjection();
    return nullptr;
}

int VectorTileLayer::tileColumnCount(int level) const
{
    // So far we only support Vector tiles with a single level zero tile
    return TileLoaderHelper::levelToColumn(1, level);
}

int VectorTileLayer::tileRowCount(int level) const
{
    // So far we only support Vector tiles with a single level zero tile
    return TileLoaderHelper::levelToRow(1, level);
}

int VectorTileLayer::layerCount() const
{
    // So far we only support one sublayer of vector tiles
    return 1;
}

void VectorTileLayer::downloadTile(const TileId &id)
{
    const QList<const GeoSceneVectorTileDataset *> vectorLayers = d->findRelevantVectorLayers(id);

    for (const GeoSceneVectorTileDataset *vectorLayer : vectorLayers) {
        if (vectorLayer->tileLevels().isEmpty() || vectorLayer->tileLevels().contains(id.zoomLevel())) {
            if (TileLoader::tileStatus(vectorLayer, id) != TileLoader::Available) {
                d->m_loader.downloadTile(vectorLayer, id, DownloadBulk);
            }
        }
    }
}

void VectorTileLayer::reset()
{
    for (VectorTileModel *mapper : std::as_const(d->m_tileModels)) {
        mapper->clear();
    }
}

void VectorTileLayer::setMapTheme(const QList<const GeoSceneVectorTileDataset *> &textures, const GeoSceneGroup *textureLayerSettings)
{
    qDeleteAll(d->m_tileModels);
    d->m_tileModels.clear();
    d->m_activeTileModels.clear();

    for (const GeoSceneVectorTileDataset *layer : textures) {
        d->m_tileModels << new VectorTileModel(&d->m_loader, layer, d->m_treeModel, &d->m_threadPool);
    }

    d->m_layerSettings = textureLayerSettings;

    if (d->m_layerSettings) {
        connect(d->m_layerSettings, SIGNAL(valueChanged(QString, bool)), this, SLOT(updateLayerSettings()));
    }

    d->updateLayerSettings();
    auto const level = tileZoomLevel();
    if (level >= 0) {
        Q_EMIT tileLevelChanged(level);
    }
}

QList<const GeoSceneVectorTileDataset *> VectorTileLayer::Private::findRelevantVectorLayers(const TileId &tileId) const
{
    QList<const GeoSceneVectorTileDataset *> result;

    for (VectorTileModel *candidate : m_activeTileModels) {
        Q_ASSERT(candidate);
        const GeoSceneVectorTileDataset *vectorTileDataset = candidate->layer();
        // check, if layer provides tiles for the current level
        if (!vectorTileDataset->hasMaximumTileLevel() || vectorTileDataset->maximumTileLevel() >= tileId.zoomLevel()) {
            // check if the tile intersects with texture bounds
            if (vectorTileDataset->latLonBox().isNull()) {
                result.append(vectorTileDataset);
            } else {
                const GeoDataLatLonBox bbox = vectorTileDataset->tileProjection()->geoCoordinates(tileId);

                if (vectorTileDataset->latLonBox().intersects(bbox)) {
                    result.append(vectorTileDataset);
                }
            }
        }
    }

    return result;
}

}

#include "moc_VectorTileLayer.cpp"
