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
#include "TextureLayer.moc"

#include <QtCore/QTimer>

#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneFilter.h"
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoScenePalette.h"
#include "GeoSceneSettings.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MergedLayerDecorator.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "VectorComposer.h"
#include "ViewParams.h"

namespace Marble
{

class TextureLayer::Private
{
public:
    Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator, TextureLayer *parent );

    GeoSceneTexture *textureLayer() const;
    static GeoSceneGroup *textureLayerProperties( GeoSceneDocument *mapTheme );

public:
    TextureColorizer     m_texcolorizer;
    StackedTileLoader    m_tileLoader;
    MergedLayerDecorator m_layerDecorator;
    VectorComposer       m_veccomposer;
    AbstractScanlineTextureMapper *m_texmapper;
    GeoSceneDocument              *m_mapTheme;
};

TextureLayer::Private::Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator, TextureLayer *parent )
    : m_texcolorizer()
    , m_tileLoader( mapThemeManager, downloadManager, parent )
    , m_layerDecorator( &m_tileLoader, sunLocator )
    , m_veccomposer( parent )
    , m_texmapper( 0 )
    , m_mapTheme( 0 )
{
}

GeoSceneTexture* TextureLayer::Private::textureLayer() const
{
    if ( !m_mapTheme )
        return 0;
    if ( !m_mapTheme->map()->hasTextureLayers() )
        return 0;

    // As long as we don't have an Layer Management Class we just lookup
    // the name of the layer that has the same name as the theme Id
    const QString themeId = m_mapTheme->head()->theme();
    GeoSceneLayer * const layer = static_cast<GeoSceneLayer*>( m_mapTheme->map()->layer( themeId ));
    if ( !layer )
        return 0;

    return static_cast<GeoSceneTexture*>( layer->groundDataset() );
}

GeoSceneGroup * TextureLayer::Private::textureLayerProperties( GeoSceneDocument *mapTheme )
{
    if ( !mapTheme )
        return 0;

    GeoSceneSettings * const settings = mapTheme->settings();
    if ( !settings )
        return 0;

    return settings->group( "Texture Layers" );
}



TextureLayer::TextureLayer( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator )
    : QObject()
    , d( new Private( mapThemeManager, downloadManager, sunLocator, this ) )
{
    connect( &d->m_layerDecorator, SIGNAL( repaintMap() ), SIGNAL( modelChanged() ) );
    connect( &d->m_veccomposer, SIGNAL( datasetLoaded() ), SIGNAL( modelChanged() ) );
    connect( &d->m_tileLoader, SIGNAL( tileUpdateAvailable() ), SIGNAL( modelChanged() ) );
}

TextureLayer::~TextureLayer()
{
    delete d;
}

void TextureLayer::paintGlobe( GeoPainter *painter,
                               ViewParams *viewParams,
                               bool redrawBackground,
                               const QRect& dirtyRect )
{
    if ( !d->m_mapTheme )
        return;

    if ( redrawBackground ) {
        if ( d->m_mapTheme->map()->hasTextureLayers() ) {
            // FIXME: Remove this once the LMC is there:
            QString themeID = d->m_mapTheme->head()->theme();

            GeoSceneLayer *layer =
                static_cast<GeoSceneLayer*>( d->m_mapTheme->map()->layer( themeID ) );

            // Create the height map image a.k.a viewParams->d->m_canvasImage.
            d->m_texmapper->mapTexture( viewParams );

            if ( !viewParams->showElevationModel()
                && layer->role() == "dem"
                && !d->m_mapTheme->map()->filters().isEmpty() ) {

                GeoSceneFilter *filter= d->m_mapTheme->map()->filters().first();
                viewParams->coastImage()->fill( Qt::transparent );
                // Create VectorMap
                d->m_veccomposer.drawTextureMap( viewParams );

                // Colorize using settings from when the map was loaded
                // there's no need to check the palette because it's set with the map theme
                if( filter->type() == "colorize" ) {
                    d->m_texcolorizer.colorize( viewParams );
                }
            } //else { mDebug() << "No filters to act on..."; }
        }
    }

    // Paint the map on the Widget
//    QTime t;
//    t.start();
    int radius = (int)(1.05 * (qreal)(viewParams->radius()));

    if ( d->m_mapTheme->map()->hasTextureLayers() ) {
        if ( viewParams->projection() == Spherical ) {
            QRect rect( viewParams->width() / 2 - radius, viewParams->height() / 2 - radius,
                        2 * radius, 2 * radius);
            rect = rect.intersect( dirtyRect );
            painter->drawImage( rect, *viewParams->canvasImage(), rect );
        }
        else {
            painter->drawImage( dirtyRect, *viewParams->canvasImage(), dirtyRect );
        }
    }

//    qDebug( "Painted in %ims", t.elapsed() );
}

void TextureLayer::setShowTileId( bool show )
{
    d->m_layerDecorator.setShowTileId( show );
}

void TextureLayer::setupTextureMapper( Projection projection )
{
    if ( !d->m_mapTheme || !d->m_mapTheme->map()->hasTextureLayers() )
        return;
  // FIXME: replace this with an approach based on the factory method pattern.
    delete d->m_texmapper;

    switch( projection ) {
        case Spherical:
            d->m_texmapper = new SphericalScanlineTextureMapper( d->textureLayer(), &d->m_tileLoader,
                                                                 this );
            break;
        case Equirectangular:
            d->m_texmapper = new EquirectScanlineTextureMapper( d->textureLayer(), &d->m_tileLoader,
                                                                this );
            break;
        case Mercator:
            d->m_texmapper = new MercatorScanlineTextureMapper( d->textureLayer(), &d->m_tileLoader,
                                                                this );
            break;
        default:
            d->m_texmapper = 0;
    }
    Q_ASSERT( d->m_texmapper );
    connect( d->m_texmapper, SIGNAL( tileLevelChanged( int )), SIGNAL( tileLevelChanged( int )));
    connect( d->m_texmapper, SIGNAL( mapChanged() ), SIGNAL( modelChanged() ) );
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
    QList<TileId> displayed = d->m_tileLoader.tilesOnDisplay();
    QList<TileId>::const_iterator pos = displayed.constBegin();
    QList<TileId>::const_iterator const end = displayed.constEnd();
    for (; pos != end; ++pos ) {
        // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
        // but since "reload" or "refresh" seems to be a common action of a browser and it
        // allows for more connections (in our model), use "DownloadBrowse"
        d->m_tileLoader.reloadTile( *pos, DownloadBrowse );
    }
}

void TextureLayer::downloadTile( const TileId &tileId )
{
    d->m_tileLoader.downloadTile( tileId );
}

void TextureLayer::setMapTheme(GeoSceneDocument* mapTheme)
{
    d->m_mapTheme = mapTheme;

    d->m_tileLoader.setTextureLayerSettings( d->textureLayerProperties( mapTheme ) );
    d->m_tileLoader.flush();

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

            d->m_texcolorizer.setSeaFileLandFile( seafile, landfile );
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
    return d->textureLayer()->tileSize();
}

GeoSceneTexture::Projection TextureLayer::tileProjection() const
{
    return d->textureLayer()->projection();
}

int TextureLayer::levelZeroColumns() const
{
    return d->textureLayer()->levelZeroColumns();
}

int TextureLayer::levelZeroRows() const
{
    return d->textureLayer()->levelZeroRows();
}

qint64 TextureLayer::volatileCacheLimit() const
{
    return d->m_tileLoader.volatileCacheLimit();
}

void TextureLayer::paintTile( StackedTile* tile, const GeoSceneTexture *textureLayer )
{
//    mDebug() << "MarbleModel::paintTile: " << "x: " << x << "y:" << y << "level: " << level
//             << "requestTileUpdate" << requestTileUpdate;

    d->m_layerDecorator.setInfo( tile->id() );
    d->m_layerDecorator.setTile( tile->resultTile() );

    d->m_layerDecorator.paint( "maps/" + textureLayer->sourceDir(), d->m_mapTheme );
}

}
