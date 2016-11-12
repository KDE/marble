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
#include <QGuiApplication>
#include <QScreen>

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
    void updateTextureLayers();

public:
    VectorTileLayer  *const m_parent;
    TileLoader m_loader;
    QVector<VectorTileModel *> m_texmappers;
    QVector<VectorTileModel *> m_activeTexmappers;
    const GeoSceneGroup *m_textureLayerSettings;
    int m_averageScreenArea;

    // TreeModel for displaying GeoDataDocuments
    GeoDataTreeModel *const m_treeModel;

    QThreadPool m_threadPool; // a shared thread pool for all layers to keep CPU usage sane
};

VectorTileLayer::Private::Private(HttpDownloadManager *downloadManager,
                                  const PluginManager *pluginManager,
                                  VectorTileLayer *parent,
                                  GeoDataTreeModel *treeModel) :
    m_parent( parent ),
    m_loader( downloadManager, pluginManager ),
    m_texmappers(),
    m_activeTexmappers(),
    m_textureLayerSettings( 0 ),
    m_averageScreenArea(0),
    m_treeModel( treeModel )
{
    m_threadPool.setMaxThreadCount( 1 );
}

VectorTileLayer::Private::~Private()
{
    qDeleteAll( m_activeTexmappers );
}

void VectorTileLayer::Private::updateTile(const TileId &tileId, GeoDataDocument* document)
{
    foreach ( VectorTileModel *mapper, m_activeTexmappers ) {
        mapper->updateTile(tileId, document);
    }
}

void VectorTileLayer::Private::updateTextureLayers()
{
    m_activeTexmappers.clear();

    foreach ( VectorTileModel *candidate, m_texmappers ) {
        // Check if the GeoSceneTileDataset is a TextureTile or VectorTile.
        // Only VectorTiles have to be used.
        bool enabled = true;
        if ( m_textureLayerSettings ) {
            const bool propertyExists = m_textureLayerSettings->propertyValue( candidate->name(), enabled );
            enabled |= !propertyExists; // if property doesn't exist, enable texture nevertheless
        }
        if ( enabled ) {
            m_activeTexmappers.append( candidate );
            mDebug() << "enabling texture" << candidate->name();
        } else {
            candidate->clear();
            mDebug() << "disabling texture" << candidate->name();
        }
    }
}

VectorTileLayer::VectorTileLayer(HttpDownloadManager *downloadManager,
                                 const PluginManager *pluginManager,
                                 GeoDataTreeModel *treeModel )
    : QObject()
    , d( new Private( downloadManager, pluginManager, this, treeModel ) )
{
    qRegisterMetaType<TileId>( "TileId" );
    qRegisterMetaType<GeoDataDocument*>( "GeoDataDocument*" );

    connect(&d->m_loader, SIGNAL(tileCompleted(TileId, GeoDataDocument*)), this, SLOT(updateTile(TileId, GeoDataDocument*)));

    d->m_averageScreenArea = 0;
    foreach (QScreen *screen, QGuiApplication::screens()) {
        d->m_averageScreenArea += screen->availableSize().width() * screen->availableSize().height();
    }
    d->m_averageScreenArea /= qMax(1, QGuiApplication::screens().size());
    // any screen size lower than 1024x768 is treated as 1024x768
    d->m_averageScreenArea = qMax(1024*768, d->m_averageScreenArea);
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
    int level = 0;
    foreach(const auto *mapper, d->m_activeTexmappers ) {
        level = qMax(level, mapper->tileZoomLevel());
    }
    return level;
}

QString VectorTileLayer::runtimeTrace() const
{
    int tiles = 0;
    foreach(const auto *mapper, d->m_activeTexmappers ) {
        tiles += mapper->cachedDocuments();
    }
    int const layers = d->m_activeTexmappers.size();
    return QStringLiteral("Vector Tiles: %1 tiles in %2 layers").arg(tiles).arg(layers);
}

bool VectorTileLayer::render( GeoPainter *painter, ViewportParams *viewport,
                              const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    int const oldLevel = tileZoomLevel();
    int level = 0;
    qreal const referenceArea = 1600.0 * 1200.0;
    qreal const adjustedRadius = viewport->radius() * referenceArea / d->m_averageScreenArea;
    foreach ( VectorTileModel *mapper, d->m_activeTexmappers ) {
        mapper->setViewport( viewport->viewLatLonAltBox(), adjustedRadius );
        level = qMax(level, mapper->tileZoomLevel());
    }
    if (oldLevel != level) {
        emit tileLevelChanged(level);
    }

    return true;
}

void VectorTileLayer::reset()
{
    foreach ( VectorTileModel *mapper, d->m_texmappers ) {
        mapper->clear();
    }
}

void VectorTileLayer::setMapTheme( const QVector<const GeoSceneVectorTileDataset *> &textures, const GeoSceneGroup *textureLayerSettings )
{
    qDeleteAll( d->m_texmappers );
    d->m_texmappers.clear();
    d->m_activeTexmappers.clear();

    foreach ( const GeoSceneVectorTileDataset *layer, textures ) {
        d->m_texmappers << new VectorTileModel( &d->m_loader, layer, d->m_treeModel, &d->m_threadPool );
    }

    d->m_textureLayerSettings = textureLayerSettings;

    if ( d->m_textureLayerSettings ) {
        connect( d->m_textureLayerSettings, SIGNAL(valueChanged(QString,bool)),
                 this,                      SLOT(updateTextureLayers()) );
    }

    d->updateTextureLayers();
}

}

#include "moc_VectorTileLayer.cpp"
