//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin  <ps_ml@gmx.de>
// Copyright 2010           Thibaut Gridel <tgridel@free.fr>
//

#include "VectorTileLayer.h"

#include <QtCore/qmath.h>
#include <QtCore/QCache>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "TileScalingTextureMapper.h"
#include "GeoPainter.h"
#include "GeoSceneGroup.h"
#include "MergedLayerDecorator.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "TileLoader.h"
#include "VectorComposer.h"
#include "ViewportParams.h"

namespace Marble
{

const int REPAINT_SCHEDULING_INTERVAL = 1000;

class VectorTileLayer::Private
{
public:
    Private(HttpDownloadManager *downloadManager,
            const SunLocator *sunLocator,
            VectorComposer *veccomposer,
            GeoDataTreeModel *treeModel,
            VectorTileLayer *parent );

    void mapChanged();
    void updateTextureLayers();
    //void updateTile(const TileId &tileId, const GeoDataDocument document , const QString &format);

public:
    VectorTileLayer  *const m_parent;
    const SunLocator *const m_sunLocator;
    VectorComposer *const m_veccomposer;
    TileLoader m_loader;
    MergedLayerDecorator m_layerDecorator;
    StackedTileLoader    m_tileLoader;
    QCache<TileId, const QPixmap> m_pixmapCache;
    TextureMapperInterface *m_texmapper;
    TextureColorizer *m_texcolorizer;
    QVector<const GeoSceneTiled *> m_textures;
    GeoSceneGroup *m_textureLayerSettings;

    // For scheduling repaints
    QTimer           m_repaintTimer;

    // For rendering vectorTiles
    GeoDataTreeModel m_treeModel;
};

VectorTileLayer::Private::Private( HttpDownloadManager *downloadManager,
                                   const SunLocator *sunLocator,
                                   VectorComposer *veccomposer,
                                   GeoDataTreeModel *treeModel,
                                   VectorTileLayer *parent )
    : m_parent( parent )
    , m_sunLocator( sunLocator )
    , m_veccomposer( veccomposer )
    , m_loader( downloadManager, treeModel )
    , m_layerDecorator( &m_loader, sunLocator )
    , m_tileLoader( &m_layerDecorator )
    , m_pixmapCache( 100 )
    , m_texmapper( 0 )
    , m_texcolorizer( 0 )
    , m_textureLayerSettings( 0 )
    , m_repaintTimer()
{
}

void VectorTileLayer::Private::mapChanged()
{
    if ( m_texmapper ) {
        m_texmapper->setRepaintNeeded();
    }

    if ( !m_repaintTimer.isActive() ) {
        m_repaintTimer.start();
    }
}

void VectorTileLayer::Private::updateTextureLayers()
{
    QVector<GeoSceneTiled const *> result;

    foreach ( const GeoSceneTiled *candidate, m_textures ) {

        // Check if the GeoSceneTiled is a TextureTile or VectorTile.
        // Only VectorTiles have to be used.
        if ( QString (candidate->nodeType()) == "GeoSceneVectorTile"){

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
    m_loader.setTextureLayers( result );
    m_pixmapCache.clear();
}

void VectorTileLayer::updateTile(TileId const & tileId, GeoDataDocument const &document, QString const &format )
{
    mDebug() << "--------------------------------- FIXME RECIEVE";
}


VectorTileLayer::VectorTileLayer(HttpDownloadManager *downloadManager,
                                 const SunLocator *sunLocator,
                                 VectorComposer *veccomposer ,
                                 GeoDataTreeModel *treeModel)
    : QObject()
    , d( new Private( downloadManager, sunLocator, veccomposer, treeModel, this ) )
{
    mDebug() << "----------------" << connect( &d->m_loader, SIGNAL( tileCompleted( TileId const & tileId, GeoDataDocument const & document, QString const & format ) ),
             this, SLOT( updateTile( TileId const & tileId, GeoDataDocument const & document, QString const & format ) ) );

    // Repaint timer
    d->m_repaintTimer.setSingleShot( true );
    d->m_repaintTimer.setInterval( REPAINT_SCHEDULING_INTERVAL );
    connect( &d->m_repaintTimer, SIGNAL( timeout() ),
             this, SIGNAL( repaintNeeded() ) );

    connect( d->m_veccomposer, SIGNAL( datasetLoaded() ),
             this, SLOT( mapChanged() ) );
}

VectorTileLayer::~VectorTileLayer()
{
    delete d->m_texmapper;
    delete d->m_texcolorizer;
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
    int tileLevel = (int)( tileLevelF * 1.00001 ); // snap to the sharper tile level a tiny bit earlier
    // to work around rounding errors when the radius
    // roughly equals the global texture width

    //    mDebug() << "tileLevelF: " << tileLevelF << " tileLevel: " << tileLevel;

    if ( tileLevel > d->m_tileLoader.maximumTileLevel() )
        tileLevel = d->m_tileLoader.maximumTileLevel();

    const bool changedTileLevel = tileLevel != d->m_texmapper->tileZoomLevel();

    //    mDebug() << "Texture Level was set to: " << tileLevel;
    d->m_texmapper->setTileLevel( tileLevel );

    if ( changedTileLevel ) {

        //FIXME ANDER empty tree model
        emit tileLevelChanged( tileLevel );
    }

    const QRect dirtyRect = QRect( QPoint( 0, 0), viewport->size() );

    d->m_texmapper->mapTexture( painter, viewport, dirtyRect, d->m_texcolorizer );

    return true;
}

void VectorTileLayer::setShowRelief( bool show )
{
    if ( d->m_texcolorizer ) {
        d->m_texcolorizer->setShowRelief( show );
    }
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

void VectorTileLayer::setupTextureMapper( Projection projection )
{
    if ( d->m_textures.isEmpty() )
        return;

    // FIXME: replace this with an approach based on the factory method pattern.
    delete d->m_texmapper;

    switch( projection ) {
    case Spherical:
        d->m_texmapper = new SphericalScanlineTextureMapper( &d->m_tileLoader );
        break;
    case Equirectangular:
        d->m_texmapper = new EquirectScanlineTextureMapper( &d->m_tileLoader );
        break;
    case Mercator:
        if ( d->m_tileLoader.tileProjection() == GeoSceneTiled::Mercator ) {
            d->m_texmapper = new TileScalingTextureMapper( &d->m_tileLoader, &d->m_pixmapCache );
        } else {
            d->m_texmapper = new MercatorScanlineTextureMapper( &d->m_tileLoader );
        }
        break;
    default:
        d->m_texmapper = 0;
    }
    Q_ASSERT( d->m_texmapper );
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
    mDebug() << Q_FUNC_INFO;

    d->m_tileLoader.clear();
    d->m_pixmapCache.clear();
    d->mapChanged();
}

void VectorTileLayer::reload()
{
    d->m_tileLoader.reloadVisibleTiles();
}

void VectorTileLayer::downloadTile( const TileId &tileId )
{
    d->m_tileLoader.downloadTile( tileId );
}

void VectorTileLayer::setMapTheme( const QVector<const GeoSceneTiled *> &textures, GeoSceneGroup *textureLayerSettings, const QString &seaFile, const QString &landFile )
{
    delete d->m_texcolorizer;
    d->m_texcolorizer = 0;

    if ( QFileInfo( seaFile ).isReadable() || QFileInfo( landFile ).isReadable() ) {
        d->m_texcolorizer = new TextureColorizer( seaFile, landFile, d->m_veccomposer );
    }

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

bool VectorTileLayer::isTileAvailable( const TileId &tileId ) const
{
    return d->m_loader.tileStatus( tileId ) == TileLoader::Available;
}

}

#include "VectorTileLayer.moc"
