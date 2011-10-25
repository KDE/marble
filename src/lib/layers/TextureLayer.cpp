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
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>//

#include "TextureLayer.h"

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
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "TileLoader.h"
#include "ViewportParams.h"

namespace Marble
{

const int REPAINT_SCHEDULING_INTERVAL = 1000;

class TextureLayer::Private
{
public:
    Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, const SunLocator *sunLocator, TextureLayer *parent );

    void mapChanged();
    void updateTextureLayers();
    void updateTile( const TileId &tileId, const QImage &tileImage );

public:
    TextureLayer  *const m_parent;
    const SunLocator *const m_sunLocator;
    TileLoader m_loader;
    StackedTileLoader    m_tileLoader;
    QCache<TileId, QPixmap> m_pixmapCache;
    TextureMapperInterface *m_texmapper;
    QPointer<TextureColorizer> m_texcolorizer;
    QVector<const GeoSceneTexture *> m_textures;
    GeoSceneGroup *m_textureLayerSettings;

    // For scheduling repaints
    QTimer           m_repaintTimer;
};

TextureLayer::Private::Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, const SunLocator *sunLocator, TextureLayer *parent )
    : m_parent( parent )
    , m_sunLocator( sunLocator )
    , m_loader( downloadManager, mapThemeManager )
    , m_tileLoader( &m_loader, sunLocator )
    , m_pixmapCache( 100 )
    , m_texmapper( 0 )
    , m_texcolorizer( 0 )
    , m_textureLayerSettings( 0 )
    , m_repaintTimer()
{
}

void TextureLayer::Private::mapChanged()
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
    QVector<GeoSceneTexture const *> result;

    foreach ( const GeoSceneTexture *candidate, m_textures ) {
        bool enabled = true;
        if ( m_textureLayerSettings ) {
            const bool propertyExists = m_textureLayerSettings->propertyValue( candidate->name(), enabled );
            enabled |= !propertyExists; // if property doesn't exist, enable texture nevertheless
        }
        if ( enabled )
            result.append( candidate );
    }

    m_tileLoader.setTextureLayers( result );
    m_pixmapCache.clear();
}

void TextureLayer::Private::updateTile( const TileId &tileId, const QImage &tileImage )
{
    if ( tileImage.isNull() )
        return; // keep tiles in cache to improve performance

    const TileId stackedTileId( 0, tileId.zoomLevel(), tileId.x(), tileId.y() );
    m_pixmapCache.remove( stackedTileId );

    m_tileLoader.updateTile( tileId, tileImage );
}



TextureLayer::TextureLayer( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, const SunLocator *sunLocator )
    : QObject()
    , d( new Private( mapThemeManager, downloadManager, sunLocator, this ) )
{
    connect( &d->m_loader, SIGNAL( tileCompleted( const TileId &, const QImage & ) ),
             this, SLOT( updateTile( const TileId &, const QImage & ) ) );

    // Repaint timer
    d->m_repaintTimer.setSingleShot( true );
    d->m_repaintTimer.setInterval( REPAINT_SCHEDULING_INTERVAL );
    connect( &d->m_repaintTimer, SIGNAL( timeout() ),
             this, SIGNAL( repaintNeeded() ) );
}

TextureLayer::~TextureLayer()
{
    delete d;
}

QStringList TextureLayer::renderPosition() const
{
    return QStringList() << "SURFACE";
}

bool TextureLayer::showSunShading() const
{
    return d->m_tileLoader.showSunShading();
}

bool TextureLayer::showCityLights() const
{
    return d->m_tileLoader.showCityLights();
}

bool TextureLayer::render( GeoPainter *painter, ViewportParams *viewport,
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
    const int levelZeroMinDimension = ( levelZeroWidth < levelZeroHight ) ? levelZeroWidth : levelZeroHight;

    qreal linearLevel = ( 4.0 * (qreal)( viewport->radius() ) / (qreal)( levelZeroMinDimension ) );

    if ( linearLevel < 1.0 )
        linearLevel = 1.0; // Dirty fix for invalid entry linearLevel

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)

    qreal tileLevelF = log( linearLevel ) / log( 2.0 );
    int tileLevel = (int)( tileLevelF );

//    mDebug() << "tileLevelF: " << tileLevelF << " tileLevel: " << tileLevel;

    if ( tileLevel > d->m_tileLoader.maximumTileLevel() )
        tileLevel = d->m_tileLoader.maximumTileLevel();

    const bool changedTileLevel = tileLevel != d->m_texmapper->tileZoomLevel();

    //    mDebug() << "Texture Level was set to: " << tileLevel;
    d->m_texmapper->setTileLevel( tileLevel );

    if ( changedTileLevel ) {
        emit tileLevelChanged( tileLevel );
    }

    const QRect dirtyRect = QRect( QPoint( 0, 0), viewport->size() );
    d->m_texmapper->mapTexture( painter, viewport, dirtyRect, d->m_texcolorizer );

    return true;
}

void TextureLayer::setShowSunShading( bool show )
{
    disconnect( d->m_sunLocator, SIGNAL( positionChanged( qreal, qreal ) ),
                this, SLOT( update() ) );

    if ( show ) {
        connect( d->m_sunLocator, SIGNAL( positionChanged( qreal, qreal ) ),
                 this,       SLOT( update() ) );
    }

    d->m_tileLoader.setShowSunShading( show );

    update();
}

void TextureLayer::setShowCityLights( bool show )
{
    d->m_tileLoader.setShowCityLights( show );

    update();
}

void TextureLayer::setShowTileId( bool show )
{
    d->m_tileLoader.setShowTileId( show );

    update();
}

void TextureLayer::setTextureColorizer( TextureColorizer *texcolorizer )
{
    if ( d->m_texcolorizer ) {
        disconnect( d->m_texcolorizer, 0, this, 0 );
    }

    d->m_texcolorizer = texcolorizer;

    if ( d->m_texcolorizer ) {
        connect( d->m_texcolorizer, SIGNAL( datasetLoaded() ), SLOT( mapChanged() ) );
    }
}

void TextureLayer::setupTextureMapper( Projection projection )
{
    if ( d->m_textures.isEmpty() )
        return;

  // FIXME: replace this with an approach based on the factory method pattern.
    delete d->m_texmapper;

    switch( projection ) {
        case Spherical:
            d->m_texmapper = new SphericalScanlineTextureMapper( &d->m_tileLoader, this );
            break;
        case Equirectangular:
            d->m_texmapper = new EquirectScanlineTextureMapper( &d->m_tileLoader, this );
            break;
        case Mercator:
            if ( d->m_tileLoader.tileProjection() == GeoSceneTexture::Mercator ) {
                d->m_texmapper = new TileScalingTextureMapper( &d->m_tileLoader, &d->m_pixmapCache, this );
            } else {
                d->m_texmapper = new MercatorScanlineTextureMapper( &d->m_tileLoader, this );
            }
            break;
        default:
            d->m_texmapper = 0;
    }
    Q_ASSERT( d->m_texmapper );
    connect( d->m_texmapper, SIGNAL( tileUpdatesAvailable() ), SLOT( mapChanged() ) );
}

void TextureLayer::setNeedsUpdate()
{
    if ( d->m_texmapper ) {
        d->m_texmapper->setRepaintNeeded();
    }
}

void TextureLayer::setVolatileCacheLimit( quint64 kilobytes )
{
    d->m_tileLoader.setVolatileCacheLimit( kilobytes );
}

void TextureLayer::update()
{
    mDebug() << "TextureLayer::update()";
    d->m_tileLoader.clear();
    d->mapChanged();
}

void TextureLayer::reload()
{
    d->m_tileLoader.reloadVisibleTiles();
}

void TextureLayer::downloadTile( const TileId &tileId )
{
    d->m_tileLoader.downloadTile( tileId );
}

void TextureLayer::setMapTheme( const QVector<const GeoSceneTexture *> &textures, GeoSceneGroup *textureLayerSettings )
{
    if ( d->m_textureLayerSettings ) {
        disconnect( d->m_textureLayerSettings, SIGNAL( valueChanged( QString, bool ) ),
                    this,                      SLOT( updateTextureLayers() ) );
    }

    d->m_textures = textures;
    d->m_textureLayerSettings = textureLayerSettings;

    if ( d->m_textureLayerSettings ) {
        connect( d->m_textureLayerSettings, SIGNAL( valueChanged( QString, bool ) ),
                 this,                      SLOT( updateTextureLayers() ) );
    }

    d->updateTextureLayers();
}

int TextureLayer::tileZoomLevel() const
{
    if (!d->m_texmapper)
        return -1;

    return d->m_texmapper->tileZoomLevel();
}

QSize TextureLayer::tileSize() const
{
    return d->m_tileLoader.tileSize();
}

GeoSceneTexture::Projection TextureLayer::tileProjection() const
{
    return d->m_tileLoader.tileProjection();
}

int TextureLayer::tileColumnCount( int level ) const
{
    return d->m_tileLoader.tileColumnCount( level );
}

int TextureLayer::tileRowCount( int level ) const
{
    return d->m_tileLoader.tileRowCount( level );
}

qint64 TextureLayer::volatileCacheLimit() const
{
    return d->m_tileLoader.volatileCacheLimit();
}

int TextureLayer::preferredRadiusCeil( int radius ) const
{
    const int tileWidth = d->m_tileLoader.tileSize().width();
    const int levelZeroColumns = d->m_tileLoader.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = log( linearLevel ) / log( 2.0 );
    const int tileLevel = qCeil( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

int TextureLayer::preferredRadiusFloor( int radius ) const
{
    const int tileWidth = d->m_tileLoader.tileSize().width();
    const int levelZeroColumns = d->m_tileLoader.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = log( linearLevel ) / log( 2.0 );
    const int tileLevel = qFloor( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

}

#include "TextureLayer.moc"
