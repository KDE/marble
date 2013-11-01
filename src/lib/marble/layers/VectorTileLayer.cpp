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
#include "GeoSceneVectorTile.h"
#include "MarbleDebug.h"
#include "TileLoader.h"
#include "ViewportParams.h"
#include "GeoDataLatLonAltBox.h"

namespace Marble
{

class VectorTileLayer::Private
{
public:
    Private(HttpDownloadManager *downloadManager,
            const PluginManager *pluginManager,
            VectorTileLayer *parent,
            GeoDataTreeModel *treeModel);

    ~Private();

    void updateTextureLayers();

public:
    VectorTileLayer  *const m_parent;
    TileLoader m_loader;
    QVector<VectorTileModel *> m_texmappers;
    QVector<VectorTileModel *> m_activeTexmappers;
    const GeoSceneGroup *m_textureLayerSettings;

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
    m_treeModel( treeModel )
{
    m_threadPool.setMaxThreadCount( 1 );
}

VectorTileLayer::Private::~Private()
{
    qDeleteAll( m_activeTexmappers );
}

void VectorTileLayer::Private::updateTextureLayers()
{
    m_activeTexmappers.clear();

    foreach ( VectorTileModel *candidate, m_texmappers ) {
        // Check if the GeoSceneTiled is a TextureTile or VectorTile.
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
}

VectorTileLayer::~VectorTileLayer()
{
    delete d;
}

QStringList VectorTileLayer::renderPosition() const
{
    return QStringList() << "SURFACE";
}

bool VectorTileLayer::render( GeoPainter *painter, ViewportParams *viewport,
                              const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    foreach ( VectorTileModel *mapper, d->m_activeTexmappers ) {
        mapper->setViewport( viewport->viewLatLonAltBox(), viewport->radius() );
    }

    return true;
}

void VectorTileLayer::reset()
{
    foreach ( VectorTileModel *mapper, d->m_texmappers ) {
        mapper->clear();
    }
}

void VectorTileLayer::setMapTheme( const QVector<const GeoSceneVectorTile *> &textures, const GeoSceneGroup *textureLayerSettings )
{
    qDeleteAll( d->m_texmappers );
    d->m_texmappers.clear();
    d->m_activeTexmappers.clear();

    foreach ( const GeoSceneVectorTile *layer, textures ) {
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

#include "VectorTileLayer.moc"
