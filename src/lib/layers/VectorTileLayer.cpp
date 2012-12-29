/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2008-2009      Patrick Spendrin  <ps_ml@gmx.de>
 Copyright 2010           Thibaut Gridel <tgridel@free.fr>
 Copyright 2012           Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "VectorTileLayer.h"

#include <QtCore/qmath.h>
#include <QtCore/QCache>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

#include "VectorTileMapper.h"
#include "GeoPainter.h"
#include "GeoSceneGroup.h"
#include "GeoSceneTypes.h"
#include "MergedLayerDecorator.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "SunLocator.h"
#include "TileLoader.h"
#include "ViewportParams.h"
#include "GeoDataTreeModel.h"
#include "GeoDataLatLonAltBox.h"

namespace Marble
{

const int REPAINT_SCHEDULING_INTERVAL = 1000;

struct CacheDocument
{
    /** The CacheDocument takes ownership of doc */
    CacheDocument( GeoDataDocument* doc, GeoDataTreeModel* model );

    /** Remove the document from the tree and delete the document */
    ~CacheDocument();

    GeoDataDocument* document;
    GeoDataTreeModel* owner;

private:
    Q_DISABLE_COPY( CacheDocument )
};

class VectorTileLayer::Private
{
public:
    Private(HttpDownloadManager *downloadManager,
            const SunLocator *sunLocator,
            const PluginManager *pluginManager,
            VectorTileLayer *parent,
            GeoDataTreeModel *treeModel);

    void updateTextureLayers();

public:
    VectorTileLayer  *const m_parent;
    const SunLocator *const m_sunLocator;
    TileLoader m_loader;
    MergedLayerDecorator m_layerDecorator;
    StackedTileLoader    m_tileLoader;
    VectorTileMapper *m_texmapper;
    QVector<const GeoSceneTiled *> m_textures;
    GeoSceneGroup *m_textureLayerSettings;

    // For scheduling repaints
    QTimer           m_repaintTimer;

    // Vector tile managing

    // TreeModel for displaying GeoDataDocuments
    GeoDataTreeModel *m_treeModel;

    // GeoDataDocuments cache. GeoDataDocuments being displayed
    // will be here. When map changes, if we remove one GeoDataDocument
    // from the TreeModel and the cache, the GeoDataDocument can't be deleted
    // because a StackedTile in TileLoader will be pointing to it not to have it
    // already parsed if necessary. In order to delete GeoDataDocuments, we will have
    // to notify the TileLoader.
    QCache< GeoDataLatLonAltBox, CacheDocument> m_documents;

};

CacheDocument::CacheDocument( GeoDataDocument* doc, GeoDataTreeModel* model ) :
    document( doc ), owner( model )
{
    // nothing to do
}

CacheDocument::~CacheDocument()
{
    Q_ASSERT( owner );
    owner->removeDocument( document );
}

VectorTileLayer::Private::Private(HttpDownloadManager *downloadManager,
                                  const SunLocator *sunLocator,
                                  const PluginManager *pluginManager,
                                  VectorTileLayer *parent,
                                  GeoDataTreeModel *treeModel)
    : m_parent( parent )
    , m_sunLocator( sunLocator )
    , m_loader( downloadManager, pluginManager )
    , m_layerDecorator( &m_loader, sunLocator )
    , m_tileLoader( &m_layerDecorator )
    , m_texmapper( 0 )
    , m_textureLayerSettings( 0 )
    , m_repaintTimer()
    , m_treeModel( treeModel )
{
}

void VectorTileLayer::Private::updateTextureLayers()
{
    QVector<GeoSceneTiled const *> result;

    foreach ( const GeoSceneTiled *candidate, m_textures ) {

        // Check if the GeoSceneTiled is a TextureTile or VectorTile.
        // Only VectorTiles have to be used.
        if ( candidate->nodeType() == GeoSceneTypes::GeoSceneVectorTileType ){

            bool enabled = true;
            if ( m_textureLayerSettings ) {
                const bool propertyExists = m_textureLayerSettings->propertyValue( candidate->name(), enabled );
                enabled |= !propertyExists; // if property doesn't exist, enable texture nevertheless
            }
            if ( enabled ) {
                result.append( candidate );
                mDebug() << "enabling texture" << candidate->name();
            } else {
                mDebug() << "disabling texture" << candidate->name();
            }
        }
    }

    if ( !result.isEmpty() ) {
        const GeoSceneTiled *const firstTexture = result.at( 0 );
        m_layerDecorator.setLevelZeroLayout( firstTexture->levelZeroColumns(), firstTexture->levelZeroRows() );
        m_layerDecorator.setThemeId( "maps/" + firstTexture->sourceDir() );
    }

    m_tileLoader.setTextureLayers( result );
}

VectorTileLayer::VectorTileLayer(HttpDownloadManager *downloadManager,
                                 const SunLocator *sunLocator,
                                 const PluginManager *pluginManager,
                                 GeoDataTreeModel *treeModel )
    : QObject()
    , d( new Private( downloadManager, sunLocator, pluginManager, this, treeModel ) )
{
    qRegisterMetaType<TileId>( "TileId" );
    qRegisterMetaType<GeoDataDocument*>( "GeoDataDocument*" );

}

VectorTileLayer::~VectorTileLayer()
{
    foreach( GeoDataLatLonAltBox box , d->m_documents.keys() ){
            CacheDocument * document = d->m_documents.take( box );
            d->m_treeModel->removeDocument( document->document );
            d->m_documents.remove( box );
            delete document;
        }
    d->m_documents.clear();
    delete d->m_texmapper;
    delete d;
}

QStringList VectorTileLayer::renderPosition() const
{
    return QStringList() << "SURFACE";
}

bool VectorTileLayer::showSunShading() const
{
    return d->m_layerDecorator.showSunShading();
}

bool VectorTileLayer::showCityLights() const
{
    return d->m_layerDecorator.showCityLights();
}

void VectorTileLayer::updateTile(TileId const & tileId, GeoDataDocument * document, QString const &format )
{
    Q_UNUSED( format );
    Q_UNUSED( tileId );

    if ( !d->m_documents.contains( document->latLonAltBox() ) ){
        d->m_treeModel->addDocument( document );
        d->m_documents.insert( document->latLonAltBox(), new CacheDocument( document, d->m_treeModel ) );
    }
}

bool VectorTileLayer::render( GeoPainter *painter, ViewportParams *viewport,
                              const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    // Stop repaint timer if it is already running
    d->m_repaintTimer.stop();

    if ( d->m_textures.isEmpty() )
        return false;

    if ( !d->m_texmapper )
        return false;

    // choose the smaller dimension for selecting the tile level, leading to higher-resolution results
    const int levelZeroWidth = d->m_tileLoader.tileSize().width() * d->m_tileLoader.tileColumnCount( 0 );
    const int levelZeroHight = d->m_tileLoader.tileSize().height() * d->m_tileLoader.tileRowCount( 0 );
    const int levelZeroMinDimension = qMin( levelZeroWidth, levelZeroHight );

    qreal linearLevel = ( 4.0 * (qreal)( viewport->radius() ) / (qreal)( levelZeroMinDimension ) );

    if ( linearLevel < 1.0 )
        linearLevel = 1.0; // Dirty fix for invalid entry linearLevel

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)

    qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    int tileLevel = (int)( tileLevelF * 1.00001 );
    // snap to the sharper tile level a tiny bit earlier
    // to work around rounding errors when the radius
    // roughly equals the global texture width


    if ( tileLevel > d->m_tileLoader.maximumTileLevel() )
        tileLevel = d->m_tileLoader.maximumTileLevel();

    const bool changedTileLevel = tileLevel != d->m_texmapper->tileZoomLevel();

    d->m_texmapper->setTileLevel( tileLevel );

    // if zoom level has changed, empty vectortile cache
    if ( changedTileLevel ) {
        d->m_documents.clear();
        d->m_tileLoader.cleanupTilehash();
        d->m_texmapper->initTileRangeCoords();
    }
    // else remove only tiles that are not shown on the screen
    else{
        foreach( GeoDataLatLonAltBox box , d->m_documents.keys() )
            if ( !box.intersects( viewport->viewLatLonAltBox() ) ){
                CacheDocument * document = d->m_documents.take( box );
                d->m_documents.remove( box );
                delete document;
            }
    }

    const QRect dirtyRect = QRect( QPoint( 0, 0), viewport->size() );

    d->m_texmapper->mapTexture( painter, viewport, dirtyRect, 0 );

    return true;
}

void VectorTileLayer::setShowSunShading( bool show )
{
    disconnect( d->m_sunLocator, SIGNAL( positionChanged( qreal, qreal ) ),
                this, SLOT( reset() ) );

    if ( show ) {
        connect( d->m_sunLocator, SIGNAL( positionChanged( qreal, qreal ) ),
                 this,       SLOT( reset() ) );
    }

    d->m_layerDecorator.setShowSunShading( show );

    reset();
}

void VectorTileLayer::setShowCityLights( bool show )
{
    d->m_layerDecorator.setShowCityLights( show );

    reset();
}

void VectorTileLayer::setShowTileId( bool show )
{
    d->m_layerDecorator.setShowTileId( show );

    reset();
}

void VectorTileLayer::setupTextureMapper( )
{
    if ( d->m_textures.isEmpty() )
        return;

    // FIXME: replace this with an approach based on the factory method pattern.
    delete d->m_texmapper;

    d->m_texmapper = new VectorTileMapper( &d->m_tileLoader );

    Q_ASSERT( d->m_texmapper );

    connect( d->m_texmapper, SIGNAL( tileCompleted( TileId, GeoDataDocument*, QString ) ),
             this, SLOT( updateTile( TileId, GeoDataDocument*, QString ) ) );
}

void VectorTileLayer::setNeedsUpdate()
{
    if ( d->m_texmapper ) {
        d->m_texmapper->setRepaintNeeded();
    }
}

void VectorTileLayer::setVolatileCacheLimit( quint64 kilobytes )
{
    d->m_tileLoader.setVolatileCacheLimit( kilobytes );
}

void VectorTileLayer::reset()
{
    foreach( GeoDataLatLonAltBox box , d->m_documents.keys() ){
            CacheDocument * document = d->m_documents.take( box );
            d->m_treeModel->removeDocument( document->document );
            d->m_documents.remove( box );
            delete document;
        }
    d->m_documents.clear();
    d->m_tileLoader.clear();
}

void VectorTileLayer::reload()
{
    d->m_tileLoader.reloadVisibleTiles();
}

void VectorTileLayer::downloadTile( const TileId &tileId )
{
    d->m_tileLoader.downloadStackedTile( tileId );
}

void VectorTileLayer::setMapTheme( const QVector<const GeoSceneTiled *> &textures, GeoSceneGroup *textureLayerSettings )
{
    d->m_textures = textures;
    d->m_textureLayerSettings = textureLayerSettings;

    if ( d->m_textureLayerSettings ) {
        connect( d->m_textureLayerSettings, SIGNAL( valueChanged( QString, bool ) ),
                 this,                      SLOT( updateTextureLayers() ) );
    }

    d->updateTextureLayers();
}

int VectorTileLayer::tileZoomLevel() const
{
    if (!d->m_texmapper)
        return -1;

    return d->m_texmapper->tileZoomLevel();
}

QSize VectorTileLayer::tileSize() const
{
    return d->m_tileLoader.tileSize();
}

GeoSceneTiled::Projection VectorTileLayer::tileProjection() const
{
    return d->m_tileLoader.tileProjection();
}

int VectorTileLayer::tileColumnCount( int level ) const
{
    return d->m_tileLoader.tileColumnCount( level );
}

int VectorTileLayer::tileRowCount( int level ) const
{
    return d->m_tileLoader.tileRowCount( level );
}

qint64 VectorTileLayer::volatileCacheLimit() const
{
    return d->m_tileLoader.volatileCacheLimit();
}

int VectorTileLayer::preferredRadiusCeil( int radius ) const
{
    const int tileWidth = d->m_tileLoader.tileSize().width();
    const int levelZeroColumns = d->m_tileLoader.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    const int tileLevel = qCeil( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

int VectorTileLayer::preferredRadiusFloor( int radius ) const
{
    const int tileWidth = d->m_tileLoader.tileSize().width();
    const int levelZeroColumns = d->m_tileLoader.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    const int tileLevel = qFloor( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

}

#include "VectorTileLayer.moc"
