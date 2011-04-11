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
// Copyright 2010      Bernhard Beschow  <bbeschow@cs.tu-berlin.de>
//

#include "TextureLayer.h"

#include <QtCore/QCache>
#include <QtCore/QTimer>
#include <QtOpenGL/QGLContext>

#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "TileScalingTextureMapper.h"
#include "GLTextureMapper.h"
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneFilter.h"
#include "GeoSceneGroup.h"
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneSettings.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "TileLoader.h"
#include "ViewParams.h"

namespace Marble
{

class TextureLayer::Private
{
public:
    Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator, TextureLayer *parent );

    void mapChanged();
    void updateTextureLayers();

    const GeoSceneLayer *sceneLayer() const;
    GeoSceneGroup *textureLayerSettings() const;

public:
    TextureLayer  *const m_parent;
    TileLoader           m_loader;
    StackedTileLoader    m_tileLoader;
    QCache<TileId, QPixmap> m_pixmapCache;
    AbstractScanlineTextureMapper *m_texmapper;
    TextureColorizer              *m_texcolorizer;
    GeoSceneDocument              *m_mapTheme;
};

TextureLayer::Private::Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator, TextureLayer *parent )
    : m_parent( parent )
    , m_loader( downloadManager, mapThemeManager )
    , m_tileLoader( &m_loader, sunLocator )
    , m_pixmapCache( 100 )
    , m_texmapper( 0 )
    , m_texcolorizer( 0 )
    , m_mapTheme( 0 )
{
}

void TextureLayer::Private::mapChanged()
{
    if ( m_texmapper ) {
        m_texmapper->setRepaintNeeded();
    }

    emit m_parent->repaintNeeded( QRegion() );
}

void TextureLayer::Private::updateTextureLayers()
{
    QVector<GeoSceneTexture const *> result;

    foreach ( const GeoSceneAbstractDataset *pos, sceneLayer()->datasets() ) {
        GeoSceneTexture const * const candidate = dynamic_cast<GeoSceneTexture const *>( pos );
        if ( !candidate )
            continue;
        bool enabled = true;
        if ( textureLayerSettings() ) {
            const bool propertyExists = textureLayerSettings()->propertyValue( candidate->name(), enabled );
            enabled |= !propertyExists; // if property doesn't exist, enable texture nevertheless
        }
        if ( enabled )
            result.append( candidate );
    }

    m_tileLoader.setTextureLayers( result );
    m_pixmapCache.clear();
}

const GeoSceneLayer *TextureLayer::Private::sceneLayer() const
{
    if ( !m_mapTheme )
        return 0;

    GeoSceneHead const * head = m_mapTheme->head();
    if ( !head )
        return 0;

    GeoSceneMap const * map = m_mapTheme->map();
    if ( !map )
        return 0;

    const QString mapThemeId = head->target() + '/' + head->theme();
    mDebug() << "StackedTileLoader::updateTextureLayers" << mapThemeId;

    return map->layer( head->theme() );
}

GeoSceneGroup* TextureLayer::Private::textureLayerSettings() const
{
    if ( !m_mapTheme )
        return 0;

    if ( !m_mapTheme->settings() )
        return 0;

    return m_mapTheme->settings()->group( "Texture Layers" );
}




TextureLayer::TextureLayer( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator )
    : QObject()
    , d( new Private( mapThemeManager, downloadManager, sunLocator, this ) )
{
}

TextureLayer::~TextureLayer()
{
    delete d;
}

void TextureLayer::paintGlobe( GeoPainter *painter,
                               ViewParams *viewParams,
                               const QRect& dirtyRect )
{
    if ( !d->m_mapTheme )
        return;

    if ( !d->m_mapTheme->map()->hasTextureLayers() )
        return;

    if ( !d->m_texmapper )
        return;

    d->m_texmapper->setViewport( viewParams->viewport() );
    d->m_texmapper->mapTexture( painter, viewParams, dirtyRect, d->m_texcolorizer );
}

void TextureLayer::setShowTileId( bool show )
{
    d->m_tileLoader.setShowTileId( show );
}

void TextureLayer::setupTextureMapper( Projection projection )
{
    if ( !d->m_mapTheme || !d->m_mapTheme->map()->hasTextureLayers() )
        return;
  // FIXME: replace this with an approach based on the factory method pattern.
    delete d->m_texmapper;

    QGLContext *glContext = const_cast<QGLContext*>( QGLContext::currentContext() );

    switch( projection ) {
        case Spherical:
            if ( glContext ) {
                d->m_texmapper = new GLTextureMapper( &d->m_tileLoader, glContext, &d->m_loader, this );
            } else {
                d->m_texmapper = new SphericalScanlineTextureMapper( &d->m_tileLoader, this );
            }
            break;
        case Equirectangular:
            if ( glContext ) {
                d->m_texmapper = new GLTextureMapper( &d->m_tileLoader, glContext, &d->m_loader, this );
            } else {
                d->m_texmapper = new EquirectScanlineTextureMapper( &d->m_tileLoader, this );
            }
            break;
        case Mercator:
            if ( glContext ) {
                d->m_texmapper = new GLTextureMapper( &d->m_tileLoader, glContext, &d->m_loader, this );
            } else if ( d->m_tileLoader.tileProjection() == GeoSceneTexture::Mercator ) {
                d->m_texmapper = new TileScalingTextureMapper( &d->m_tileLoader, &d->m_pixmapCache, this );
            } else {
                d->m_texmapper = new MercatorScanlineTextureMapper( &d->m_tileLoader, this );
            }
            break;
        default:
            d->m_texmapper = 0;
    }
    Q_ASSERT( d->m_texmapper );
    connect( d->m_texmapper, SIGNAL( tileLevelChanged( int )), SIGNAL( tileLevelChanged( int )));
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
    QTimer::singleShot( 0, &d->m_tileLoader, SLOT( update() ) );
}

void TextureLayer::reload()
{
    d->m_tileLoader.reloadVisibleTiles();
}

void TextureLayer::downloadTile( const TileId &tileId )
{
    d->m_tileLoader.downloadTile( tileId );
}

void TextureLayer::setMapTheme(GeoSceneDocument* mapTheme)
{
    if ( d->textureLayerSettings() ) {
        disconnect( d->textureLayerSettings(), SIGNAL( valueChanged( QString, bool ) ),
                    this,                      SLOT( updateTextureLayers() ) );
    }

    d->m_mapTheme = mapTheme;

    if ( d->textureLayerSettings() ) {
        connect( d->textureLayerSettings(), SIGNAL( valueChanged( QString, bool )),
                 this,                      SLOT( updateTextureLayers() ) );
    }
    d->updateTextureLayers();

    delete d->m_texcolorizer;
    d->m_texcolorizer = 0;
    if( !d->m_mapTheme->map()->filters().isEmpty() ) {
        GeoSceneFilter *filter= d->m_mapTheme->map()->filters().first();

        if( filter->type() == "colorize" ) {
             //no need to look up with MarbleDirs twice so they are left null for now
            QString seafile, landfile;
            QList<GeoScenePalette*> palette = filter->palette();
            foreach ( GeoScenePalette *curPalette, palette ) {
                if( curPalette->type() == "sea" ) {
                    seafile = MarbleDirs::path( curPalette->file() );
                } else if( curPalette->type() == "land" ) {
                    landfile = MarbleDirs::path( curPalette->file() );
                }
            }
            //look up locations if they are empty
            if(seafile.isEmpty())
                seafile = MarbleDirs::path( "seacolors.leg" );
            if(landfile.isEmpty())
                landfile = MarbleDirs::path( "landcolors.leg" );

            d->m_texcolorizer = new TextureColorizer( seafile, landfile, this );
            connect( d->m_texcolorizer, SIGNAL( datasetLoaded() ), SLOT( mapChanged() ) );
        }
    }
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

}

#include "TextureLayer.moc"
