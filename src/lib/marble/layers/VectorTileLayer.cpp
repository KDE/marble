/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2008-2009      Patrick Spendrin  <ps_ml@gmx.de>
 Copyright 2010           Thibaut Gridel <tgridel@free.fr>
 Copyright 2012           Ander Pijoan <ander.pijoan@deusto.es>
 Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
*/

#include "VectorTileLayer.h"

#include <qmath.h>
#include <QThreadPool>

#include "VectorTileModel.h"
#include "GeoPainter.h"
#include "GeoSceneGroup.h"
#include "GeoSceneTypes.h"
#include "GeoSceneVectorTileDataset.h"
#include "MarbleDebug.h"
#include "TileLoader.h"
#include "ViewportParams.h"
#include "RenderState.h"
#include "GeoDataDocument.h"
#include "GeoDataLatLonAltBox.h"
#include "HttpDownloadManager.h"

namespace Marble
{

class Q_DECL_HIDDEN VectorTileLayer::Private
{
public:
    Private(HttpDownloadManager *downloadManager,
            const PluginManager *pluginManager,
            VectorTileLayer *parent,
            GeoDataTreeModel *treeModel);

    ~Private();

    void updateTile(const TileId &tileId, GeoDataDocument* document);
    void updateLayerSettings();

public:
    VectorTileLayer  *const m_parent;
    TileLoader m_loader;
    QVector<VectorTileModel *> m_tileModels;
    QVector<VectorTileModel *> m_activeTileModels;
    const GeoSceneGroup *m_layerSettings;

    // TreeModel for displaying GeoDataDocuments
    GeoDataTreeModel *const m_treeModel;

    QThreadPool m_threadPool; // a shared thread pool for all layers to keep CPU usage sane
};

VectorTileLayer::Private::Private(HttpDownloadManager *downloadManager,
                                  const PluginManager *pluginManager,
                                  VectorTileLayer *parent,
                                  GeoDataTreeModel *treeModel) :
    m_parent(parent),
    m_loader(downloadManager, pluginManager),
    m_tileModels(),
    m_activeTileModels(),
    m_layerSettings(0),
    m_treeModel(treeModel)
{
    m_threadPool.setMaxThreadCount(1);
}

VectorTileLayer::Private::~Private()
{
    qDeleteAll(m_activeTileModels);
}

void VectorTileLayer::Private::updateTile(const TileId &tileId, GeoDataDocument* document)
{
    for (VectorTileModel *mapper: m_activeTileModels) {
        mapper->updateTile(tileId, document);
    }
}

void VectorTileLayer::Private::updateLayerSettings()
{
    m_activeTileModels.clear();

    for (VectorTileModel *candidate: m_tileModels) {
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

VectorTileLayer::VectorTileLayer(HttpDownloadManager *downloadManager,
                                 const PluginManager *pluginManager,
                                 GeoDataTreeModel *treeModel)
    : QObject()
    , d(new Private(downloadManager, pluginManager, this, treeModel))
{
    qRegisterMetaType<TileId>("TileId");
    qRegisterMetaType<GeoDataDocument*>("GeoDataDocument*");

    connect(&d->m_loader, SIGNAL(tileCompleted(TileId, GeoDataDocument*)), this, SLOT(updateTile(TileId, GeoDataDocument*)));
}

VectorTileLayer::~VectorTileLayer()
{
    delete d;
}

QStringList VectorTileLayer::renderPosition() const
{
    return QStringList(QStringLiteral("SURFACE"));
}

RenderState VectorTileLayer::renderState() const
{
    return RenderState(QStringLiteral("Vector Tiles"));
}

int VectorTileLayer::tileZoomLevel() const
{
    int level = -1;
    for (const auto *mapper: d->m_activeTileModels) {
        level = qMax(level, mapper->tileZoomLevel());
    }
    return level;
}

QString VectorTileLayer::runtimeTrace() const
{
    int tiles = 0;
    for (const auto *mapper: d->m_activeTileModels) {
        tiles += mapper->cachedDocuments();
    }
    int const layers = d->m_activeTileModels.size();
    return QStringLiteral("Vector Tiles: %1 tiles in %2 layers").arg(tiles).arg(layers);
}

bool VectorTileLayer::render(GeoPainter *painter, ViewportParams *viewport,
                             const QString &renderPos, GeoSceneLayer *layer)
{
    Q_UNUSED(painter);
    Q_UNUSED(renderPos);
    Q_UNUSED(layer);

    int const oldLevel = tileZoomLevel();
    int level = 0;
    for (VectorTileModel *mapper: d->m_activeTileModels) {
        mapper->setViewport(viewport->viewLatLonAltBox());
        level = qMax(level, mapper->tileZoomLevel());
    }
    if (oldLevel != level && level >= 0) {
        emit tileLevelChanged(level);
    }

    return true;
}

void VectorTileLayer::reload()
{
    for (auto mapper : d->m_activeTileModels) {
        mapper->reload();
    }
}

void VectorTileLayer::reset()
{
    for (VectorTileModel *mapper: d->m_tileModels) {
        mapper->clear();
    }
}

void VectorTileLayer::setMapTheme(const QVector<const GeoSceneVectorTileDataset *> &textures, const GeoSceneGroup *textureLayerSettings)
{
    qDeleteAll(d->m_tileModels);
    d->m_tileModels.clear();
    d->m_activeTileModels.clear();

    for (const GeoSceneVectorTileDataset *layer: textures) {
        d->m_tileModels << new VectorTileModel(&d->m_loader, layer, d->m_treeModel, &d->m_threadPool);
    }

    d->m_layerSettings = textureLayerSettings;

    if (d->m_layerSettings) {
        connect(d->m_layerSettings, SIGNAL(valueChanged(QString, bool)),
                this,                      SLOT(updateLayerSettings()));
    }

    d->updateLayerSettings();
    auto const level = tileZoomLevel();
    if (level >= 0) {
        emit tileLevelChanged(level);
    }
}

}

#include "moc_VectorTileLayer.cpp"
