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

#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "GeoSceneTiled.h"
#include "GeoDataDocument.h"

#include <QtCore/QSize>

class QImage;
class QRegion;
class QRect;

namespace Marble
{

class GeoPainter;
class GeoSceneGroup;
class HttpDownloadManager;
class SunLocator;
class VectorComposer;
class ViewportParams;

class TextureLayer : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    TextureLayer( HttpDownloadManager *downloadManager,
                  const SunLocator *sunLocator,
                  VectorComposer *veccomposer,
                  const PluginManager *pluginManager );

    ~TextureLayer();

    QStringList renderPosition() const;

    void setCoastDocument( GeoDataDocument* coastDocument );

    void setGlacierDocument( GeoDataDocument* glacierDocument );

    void setLakeDocument( GeoDataDocument* lakeDocument );


    bool showSunShading() const;
    bool showCityLights() const;

    /**
     * @brief Return the current tile zoom level. For example for OpenStreetMap
     *        possible values are 1..18, for BlueMarble 0..6.
     */
    int tileZoomLevel() const;

    QSize tileSize() const;

    GeoSceneTiled::Projection tileProjection() const;

    int tileColumnCount( int level ) const;
    int tileRowCount( int level ) const;

    qint64 volatileCacheLimit() const;

    int preferredRadiusCeil( int radius ) const;
    int preferredRadiusFloor( int radius ) const;

    virtual QString runtimeTrace() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos = "NONE", GeoSceneLayer *layer = 0 );

public Q_SLOTS:
    void setShowRelief( bool show );

    void setShowSunShading( bool show );

    void setShowCityLights( bool show );

    void setShowTileId( bool show );

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void setupTextureMapper( Projection projection );

    void setNeedsUpdate();

    void setMapTheme( const QVector<const GeoSceneTiled *> &textures, const GeoSceneGroup *textureLayerSettings, const QString &seaFile, const QString &landFile );

    void setVolatileCacheLimit( quint64 kilobytes );

    void reset();

    void reload();

    void downloadStackedTile( const TileId &stackedTileId );

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
