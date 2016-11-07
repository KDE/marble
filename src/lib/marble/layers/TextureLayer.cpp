//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>//

#include "TextureLayer.h"

#include <qmath.h>
#include <QTimer>
#include <QList>
#include <QSortFilterProxyModel>

#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "GenericScanlineTextureMapper.h"
#include "TileScalingTextureMapper.h"
#include "GeoDataGroundOverlay.h"
#include "GeoPainter.h"
#include "GeoSceneGroup.h"
#include "GeoSceneTypes.h"
#include "MergedLayerDecorator.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "TileLoader.h"
#include "ViewportParams.h"

namespace Marble
{

const int REPAINT_SCHEDULING_INTERVAL = 1000;

class Q_DECL_HIDDEN TextureLayer::Private
{
public:
    Private( HttpDownloadManager *downloadManager,
             PluginManager* pluginManager,
             const SunLocator *sunLocator,
             QAbstractItemModel *groundOverlayModel,
             TextureLayer *parent );

    void requestDelayedRepaint();
    void updateTextureLayers();
    void updateTile( const TileId &tileId, const QImage &tileImage );

    void addGroundOverlays( const QModelIndex& parent, int first, int last );
    void removeGroundOverlays( const QModelIndex& parent, int first, int last );
    void resetGroundOverlaysCache();

    void updateGroundOverlays();
    void addCustomTextures();

    static bool drawOrderLessThan( const GeoDataGroundOverlay* o1, const GeoDataGroundOverlay* o2 );

public:
    TextureLayer  *const m_parent;
    const SunLocator *const m_sunLocator;
    TileLoader m_loader;
    MergedLayerDecorator m_layerDecorator;
    StackedTileLoader    m_tileLoader;
    GeoDataCoordinates m_centerCoordinates;
    int m_tileZoomLevel;
    TextureMapperInterface *m_texmapper;
    TextureColorizer *m_texcolorizer;
    QVector<const GeoSceneTextureTileDataset *> m_textures;
    const GeoSceneGroup *m_textureLayerSettings;
    QString m_runtimeTrace;
    QSortFilterProxyModel m_groundOverlayModel;
    QList<const GeoDataGroundOverlay *> m_groundOverlayCache;
    QMap<QString, GeoSceneTextureTileDataset *> m_customTextures;
    // For scheduling repaints
    QTimer           m_repaintTimer;
    RenderState m_renderState;
};

TextureLayer::Private::Private( HttpDownloadManager *downloadManager,
                                PluginManager* pluginManager,
                                const SunLocator *sunLocator,
                                QAbstractItemModel *groundOverlayModel,
                                TextureLayer *parent )
    : m_parent( parent )
    , m_sunLocator( sunLocator )
    , m_loader( downloadManager, pluginManager )
    , m_layerDecorator( &m_loader, sunLocator )
    , m_tileLoader( &m_layerDecorator )
    , m_centerCoordinates()
    , m_tileZoomLevel( -1 )
    , m_texmapper( 0 )
    , m_texcolorizer( 0 )
    , m_textureLayerSettings( 0 )
    , m_repaintTimer()
{
    m_groundOverlayModel.setSourceModel( groundOverlayModel );
    m_groundOverlayModel.setDynamicSortFilter( true );
    m_groundOverlayModel.setSortRole ( MarblePlacemarkModel::PopularityIndexRole );
    m_groundOverlayModel.sort (0, Qt::AscendingOrder );

    connect( &m_groundOverlayModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
             m_parent,              SLOT(addGroundOverlays(QModelIndex,int,int)) );

    connect( &m_groundOverlayModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
             m_parent,              SLOT(removeGroundOverlays(QModelIndex,int,int)) );

    connect( &m_groundOverlayModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             m_parent,              SLOT(resetGroundOverlaysCache()) );

    connect( &m_groundOverlayModel, SIGNAL(modelReset()),
             m_parent,              SLOT(resetGroundOverlaysCache()) );

    updateGroundOverlays();
}

void TextureLayer::Private::requestDelayedRepaint()
{
    if ( m_texmapper ) {
        m_texmapper->setRepaintNeeded();
    }

    if ( !m_repaintTimer.isActive() ) {
        m_repaintTimer.start();
    }
}

void TextureLayer::Private::updateTextureLayers()
{
    QVector<GeoSceneTextureTileDataset const *> result;

    foreach ( const GeoSceneTextureTileDataset *candidate, m_textures ) {
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

    updateGroundOverlays();

    m_layerDecorator.setTextureLayers( result );
    m_tileLoader.clear();

    m_tileZoomLevel = -1;
    m_parent->setNeedsUpdate();
}

void TextureLayer::Private::updateTile( const TileId &tileId, const QImage &tileImage )
{
    if ( tileImage.isNull() )
        return; // keep tiles in cache to improve performance

    m_tileLoader.updateTile( tileId, tileImage );

    requestDelayedRepaint();
}

bool TextureLayer::Private::drawOrderLessThan( const GeoDataGroundOverlay* o1, const GeoDataGroundOverlay* o2 )
{
    return o1->drawOrder() < o2->drawOrder();
}

void TextureLayer::Private::addGroundOverlays( const QModelIndex& parent, int first, int last )
{
    for ( int i = first; i <= last; ++i ) {
        QModelIndex index = m_groundOverlayModel.index( i, 0, parent );
        const GeoDataGroundOverlay *overlay = static_cast<GeoDataGroundOverlay *>( qvariant_cast<GeoDataObject *>( index.data( MarblePlacemarkModel::ObjectPointerRole ) ) );

        if ( overlay->icon().isNull() ) {
            continue;
        }

        int pos = qLowerBound( m_groundOverlayCache.begin(), m_groundOverlayCache.end(), overlay, drawOrderLessThan ) - m_groundOverlayCache.begin();
        m_groundOverlayCache.insert( pos, overlay );
    }

    updateGroundOverlays();

    m_parent->reset();
}

void TextureLayer::Private::removeGroundOverlays( const QModelIndex& parent, int first, int last )
{
    for ( int i = first; i <= last; ++i ) {
        QModelIndex index = m_groundOverlayModel.index( i, 0, parent );
        const GeoDataGroundOverlay *overlay = static_cast<GeoDataGroundOverlay *>( qvariant_cast<GeoDataObject *>( index.data( MarblePlacemarkModel::ObjectPointerRole ) ) );

        int pos = qLowerBound( m_groundOverlayCache.begin(), m_groundOverlayCache.end(), overlay, drawOrderLessThan ) - m_groundOverlayCache.begin();
        if (pos >= 0 && pos < m_groundOverlayCache.size() ) {
            m_groundOverlayCache.removeAt( pos );
        }
    }

    updateGroundOverlays();

    m_parent->reset();
}

void TextureLayer::Private::resetGroundOverlaysCache()
{
    m_groundOverlayCache.clear();

    updateGroundOverlays();

    m_parent->reset();
}

void TextureLayer::Private::updateGroundOverlays()
{
    if ( !m_texcolorizer ) {
        m_layerDecorator.updateGroundOverlays( m_groundOverlayCache );
    }
    else {
        m_layerDecorator.updateGroundOverlays( QList<const GeoDataGroundOverlay *>() );
    }
}

void TextureLayer::Private::addCustomTextures()
{
    m_textures.reserve(m_textures.size() + m_customTextures.size());
    foreach (GeoSceneTextureTileDataset *t, m_customTextures)
    {
        m_textures.append(t);
    }
}

TextureLayer::TextureLayer( HttpDownloadManager *downloadManager,
                            PluginManager* pluginManager,
                            const SunLocator *sunLocator,
                            QAbstractItemModel *groundOverlayModel )
    : QObject()
    , d( new Private( downloadManager, pluginManager, sunLocator, groundOverlayModel, this ) )
{
    connect( &d->m_loader, SIGNAL(tileCompleted(TileId,QImage)),
             this, SLOT(updateTile(TileId,QImage)) );

    // Repaint timer
    d->m_repaintTimer.setSingleShot( true );
    d->m_repaintTimer.setInterval( REPAINT_SCHEDULING_INTERVAL );
    connect( &d->m_repaintTimer, SIGNAL(timeout()),
             this, SIGNAL(repaintNeeded()) );
}

TextureLayer::~TextureLayer()
{
    qDeleteAll(d->m_customTextures);
    delete d->m_texmapper;
    delete d->m_texcolorizer;
    delete d;
}

QStringList TextureLayer::renderPosition() const
{
    return QStringList(QStringLiteral("SURFACE"));
}

void TextureLayer::addSeaDocument( const GeoDataDocument *seaDocument )
{
    if( d->m_texcolorizer ) {
        d->m_texcolorizer->addSeaDocument( seaDocument );
        reset();
    }
}

void TextureLayer::addLandDocument( const GeoDataDocument *landDocument )
{
    if( d->m_texcolorizer ) {
        d->m_texcolorizer->addLandDocument( landDocument );
        reset();
    }
}

int TextureLayer::textureLayerCount() const
{
    return d->m_layerDecorator.textureLayersSize();
}

bool TextureLayer::showSunShading() const
{
    return d->m_layerDecorator.showSunShading();
}

bool TextureLayer::showCityLights() const
{
    return d->m_layerDecorator.showCityLights();
}

bool TextureLayer::render( GeoPainter *painter, ViewportParams *viewport,
                           const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );
    d->m_runtimeTrace = QStringLiteral("Texture Cache: %1 ").arg(d->m_tileLoader.tileCount());
    d->m_renderState = RenderState(QStringLiteral("Texture Tiles"));

    // Stop repaint timer if it is already running
    d->m_repaintTimer.stop();

    if ( d->m_textures.isEmpty() )
        return false;

    if ( d->m_layerDecorator.textureLayersSize() == 0 )
        return false;

    if ( !d->m_texmapper )
        return false;

    if ( d->m_centerCoordinates.longitude() != viewport->centerLongitude() ||
         d->m_centerCoordinates.latitude() != viewport->centerLatitude() ) {
        d->m_centerCoordinates.setLongitude( viewport->centerLongitude() );
        d->m_centerCoordinates.setLatitude( viewport->centerLatitude() );
        d->m_texmapper->setRepaintNeeded();
    }

    // choose the smaller dimension for selecting the tile level, leading to higher-resolution results
    const int levelZeroWidth = d->m_layerDecorator.tileSize().width() * d->m_layerDecorator.tileColumnCount( 0 );
    const int levelZeroHight = d->m_layerDecorator.tileSize().height() * d->m_layerDecorator.tileRowCount( 0 );
    const int levelZeroMinDimension = qMin( levelZeroWidth, levelZeroHight );

    // limit to 1 as dirty fix for invalid entry linearLevel
    const qreal linearLevel = qMax<qreal>( 1.0, viewport->radius() * 4.0 / levelZeroMinDimension );

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 ) * 1.00001;  // snap to the sharper tile level a tiny bit earlier
                                                                         // to work around rounding errors when the radius
                                                                         // roughly equals the global texture width

    const int tileLevel = qMin<int>( d->m_layerDecorator.maximumTileLevel(), tileLevelF );

    if ( tileLevel != d->m_tileZoomLevel ) {
        d->m_tileZoomLevel = tileLevel;
        emit tileLevelChanged( d->m_tileZoomLevel );
    }

    const QRect dirtyRect = QRect( QPoint( 0, 0), viewport->size() );
    d->m_texmapper->mapTexture( painter, viewport, d->m_tileZoomLevel, dirtyRect, d->m_texcolorizer );
    d->m_renderState.addChild( d->m_tileLoader.renderState() );
    return true;
}

QString TextureLayer::runtimeTrace() const
{
    return d->m_runtimeTrace;
}

void TextureLayer::setShowRelief( bool show )
{
    if ( d->m_texcolorizer ) {
        d->m_texcolorizer->setShowRelief( show );
    }
}

void TextureLayer::setShowSunShading( bool show )
{
    disconnect( d->m_sunLocator, SIGNAL(positionChanged(qreal,qreal)),
                this, SLOT(reset()) );

    if ( show ) {
        connect( d->m_sunLocator, SIGNAL(positionChanged(qreal,qreal)),
                 this,       SLOT(reset()) );
    }

    d->m_layerDecorator.setShowSunShading( show );

    reset();
}

void TextureLayer::setShowCityLights( bool show )
{
    d->m_layerDecorator.setShowCityLights( show );

    reset();
}

void TextureLayer::setShowTileId( bool show )
{
    d->m_layerDecorator.setShowTileId( show );

    reset();
}

void TextureLayer::setProjection( Projection projection )
{
    if ( d->m_textures.isEmpty() ) {
        return;
    }

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
            if (d->m_textures.at(0)->tileProjectionType() == GeoSceneAbstractTileProjection::Mercator) {
                d->m_texmapper = new TileScalingTextureMapper( &d->m_tileLoader );
            } else {
                d->m_texmapper = new MercatorScanlineTextureMapper( &d->m_tileLoader );
            }
            break;
        case Gnomonic:
        case Stereographic:
        case LambertAzimuthal:
        case AzimuthalEquidistant:
        case VerticalPerspective:
            d->m_texmapper = new GenericScanlineTextureMapper( &d->m_tileLoader );
            break;
        default:
            d->m_texmapper = 0;
    }
    Q_ASSERT( d->m_texmapper );
}

void TextureLayer::setNeedsUpdate()
{
    if ( d->m_texmapper ) {
        d->m_texmapper->setRepaintNeeded();
    }

    emit repaintNeeded();
}

void TextureLayer::setVolatileCacheLimit( quint64 kilobytes )
{
    d->m_tileLoader.setVolatileCacheLimit( kilobytes );
}

void TextureLayer::reset()
{
    mDebug() << Q_FUNC_INFO;

    d->m_tileLoader.clear();
    setNeedsUpdate();
}

void TextureLayer::reload()
{
    foreach ( const TileId &id, d->m_tileLoader.visibleTiles() ) {
        // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
        // but since "reload" or "refresh" seems to be a common action of a browser and it
        // allows for more connections (in our model), use "DownloadBrowse"
        d->m_layerDecorator.downloadStackedTile( id, DownloadBrowse );
    }
}

void TextureLayer::downloadStackedTile( const TileId &stackedTileId )
{
    d->m_layerDecorator.downloadStackedTile( stackedTileId, DownloadBulk );
}

void TextureLayer::setMapTheme( const QVector<const GeoSceneTextureTileDataset *> &textures, const GeoSceneGroup *textureLayerSettings, const QString &seaFile, const QString &landFile )
{
    delete d->m_texcolorizer;
    d->m_texcolorizer = 0;

    if ( QFileInfo( seaFile ).isReadable() || QFileInfo( landFile ).isReadable() ) {
        d->m_texcolorizer = new TextureColorizer( seaFile, landFile );
    }

    d->m_textures = textures;
    d->addCustomTextures();
    d->m_textureLayerSettings = textureLayerSettings;

    if ( d->m_textureLayerSettings ) {
        connect( d->m_textureLayerSettings, SIGNAL(valueChanged(QString,bool)),
                 this,                      SLOT(updateTextureLayers()) );
    }

    d->updateTextureLayers();
}

int TextureLayer::tileZoomLevel() const
{
    return d->m_tileZoomLevel;
}

QSize TextureLayer::tileSize() const
{
    return d->m_layerDecorator.tileSize();
}

GeoSceneAbstractTileProjection::Type TextureLayer::tileProjectionType() const
{
    return d->m_layerDecorator.tileProjectionType();
}

int TextureLayer::tileColumnCount( int level ) const
{
    return d->m_layerDecorator.tileColumnCount( level );
}

int TextureLayer::tileRowCount( int level ) const
{
    return d->m_layerDecorator.tileRowCount( level );
}

qint64 TextureLayer::volatileCacheLimit() const
{
    return d->m_tileLoader.volatileCacheLimit();
}

int TextureLayer::preferredRadiusCeil( int radius ) const
{
    if (!d->m_layerDecorator.hasTextureLayer()) {
        return radius;
    }
    const int tileWidth = d->m_layerDecorator.tileSize().width();
    const int levelZeroColumns = d->m_layerDecorator.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    const int tileLevel = qCeil( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

int TextureLayer::preferredRadiusFloor( int radius ) const
{
    if (!d->m_layerDecorator.hasTextureLayer()) {
        return radius;
    }
    const int tileWidth = d->m_layerDecorator.tileSize().width();
    const int levelZeroColumns = d->m_layerDecorator.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    const int tileLevel = qFloor( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

RenderState TextureLayer::renderState() const
{
    return d->m_renderState;
}

QString TextureLayer::addTextureLayer(GeoSceneTextureTileDataset* texture)
{
    if (!texture)
        return QString(); //Not a sane call

    QString sourceDir = texture->sourceDir();
    if (!d->m_customTextures.contains(sourceDir))
    {   // Add if not present. For update, remove the old texture first.
        d->m_customTextures.insert(sourceDir, texture);
        d->m_textures.append(texture);
        d->updateTextureLayers();
    }
    return sourceDir;
}

void TextureLayer::removeTextureLayer(const QString &key)
{
    if (d->m_customTextures.contains(key))
    {
        GeoSceneTextureTileDataset *texture = d->m_customTextures.value(key);
        d->m_customTextures.remove(key);
        d->m_textures.remove(d->m_textures.indexOf(texture));
        delete texture;
        d->updateTextureLayers();
    }
}

}

#include "moc_TextureLayer.cpp"
