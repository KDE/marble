//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MARBLETEXTURELAYER_H
#define MARBLE_MARBLETEXTURELAYER_H

#include "LayerInterface.h"
#include <QtCore/QObject>

#include "global.h"
#include "GeoSceneTexture.h"

#include <QtCore/QSize>

class QImage;
class QRegion;
class QRect;

namespace Marble
{

class GeoPainter;
class GeoSceneGroup;
class HttpDownloadManager;
class MapThemeManager;
class SunLocator;
class TextureColorizer;
class ViewportParams;

class TextureLayer : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    TextureLayer( const MapThemeManager *mapThemeManager,
                  HttpDownloadManager *downloadManager,
                  const SunLocator *sunLocator );

    ~TextureLayer();

    QStringList renderPosition() const;

    bool showSunShading() const;
    bool showCityLights() const;

    /**
     * @brief Return the current tile zoom level. For example for OpenStreetMap
     *        possible values are 1..18, for BlueMarble 0..6.
     */
    int tileZoomLevel() const;

    QSize tileSize() const;

    GeoSceneTexture::Projection tileProjection() const;

    int tileColumnCount( int level ) const;
    int tileRowCount( int level ) const;

    qint64 volatileCacheLimit() const;

    int preferredRadiusCeil( int radius ) const;
    int preferredRadiusFloor( int radius ) const;

 public Q_SLOTS:
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos = "NONE", GeoSceneLayer *layer = 0 );

    void setShowSunShading( bool show );

    void setShowCityLights( bool show );

    void setShowTileId( bool show );

    void setTextureColorizer( TextureColorizer *texcolorizer );

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void setupTextureMapper( Projection projection );

    void setNeedsUpdate();

    void setMapTheme( const QVector<const GeoSceneTexture *> &textures, GeoSceneGroup *textureLayerSettings );

    void setVolatileCacheLimit( quint64 kilobytes );

    void update();

    void reload();

    void downloadTile( const TileId &tileId );

 Q_SIGNALS:
    void tileLevelChanged( int );
    void repaintNeeded();

 private:
    Q_PRIVATE_SLOT( d, void mapChanged() )
    Q_PRIVATE_SLOT( d, void updateTextureLayers() )
    Q_PRIVATE_SLOT( d, void updateTile( const TileId &tileId, const QImage &tileImage ) )

 private:
    class Private;
    Private *const d;
};

}

#endif
