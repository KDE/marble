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
#include <QObject>

#include "MarbleGlobal.h"
#include "GeoSceneTileDataset.h"

class QAbstractItemModel;
class QImage;
class QSize;

namespace Marble
{

class GeoPainter;
class GeoDataDocument;
class GeoSceneGroup;
class GeoSceneTextureTileDataset;
class HttpDownloadManager;
class SunLocator;
class ViewportParams;
class PluginManager;

class MARBLE_EXPORT TextureLayer : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    TextureLayer( HttpDownloadManager *downloadManager,
                  PluginManager* pluginManager,
                  const SunLocator *sunLocator,
                  QAbstractItemModel *groundOverlayModel );

    ~TextureLayer();

    QStringList renderPosition() const;

    void addSeaDocument( const GeoDataDocument *seaDocument );

    void addLandDocument( const GeoDataDocument *landDocument );

    int textureLayerCount() const;

    /**
     * @brief Adds texture sublayer, taking ownership of the object's memory
     *        Does nothing if a texture with the same source directory was already
     *        added with this method.
     * @return returned string is the key for the texture that can be later used to remove it
     */
    QString addTextureLayer(GeoSceneTextureTileDataset *texture);

    /**
     * @brief Removes texture sublayer identified by a key.
     *        Deletes the texture object. Does nothing if key is not found.
     * @param A key to identify the texture, returned from addTextureLayer
     */
    void removeTextureLayer(const QString &key);

    bool showSunShading() const;
    bool showCityLights() const;

    /**
     * @brief Return the current tile zoom level. For example for OpenStreetMap
     *        possible values are 1..18, for BlueMarble 0..6.
     */
    int tileZoomLevel() const;

    QSize tileSize() const;

    GeoSceneAbstractTileProjection::Type tileProjectionType() const;

    int tileColumnCount( int level ) const;
    int tileRowCount( int level ) const;

    qint64 volatileCacheLimit() const;

    int preferredRadiusCeil( int radius ) const;
    int preferredRadiusFloor( int radius ) const;

    RenderState renderState() const;

    virtual QString runtimeTrace() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString &renderPos = QLatin1String("NONE"),
                         GeoSceneLayer *layer = 0 );

public Q_SLOTS:
    void setShowRelief( bool show );

    void setShowSunShading( bool show );

    void setShowCityLights( bool show );

    void setShowTileId( bool show );

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void setProjection( Projection projection );

    void setNeedsUpdate();

    void setMapTheme( const QVector<const GeoSceneTextureTileDataset *> &textures, const GeoSceneGroup *textureLayerSettings, const QString &seaFile, const QString &landFile );

    void setVolatileCacheLimit( quint64 kilobytes );

    void reset();

    void reload();

    void downloadStackedTile( const TileId &stackedTileId );

 Q_SIGNALS:
    void tileLevelChanged( int );
    void repaintNeeded();

 private:
    Q_PRIVATE_SLOT( d, void requestDelayedRepaint() )
    Q_PRIVATE_SLOT( d, void updateTextureLayers() )
    Q_PRIVATE_SLOT( d, void updateTile( const TileId &tileId, const QImage &tileImage ) )
    Q_PRIVATE_SLOT( d, void addGroundOverlays( const QModelIndex& parent, int first, int last ) )
    Q_PRIVATE_SLOT( d, void removeGroundOverlays( const QModelIndex& parent, int first, int last ) )
    Q_PRIVATE_SLOT( d, void resetGroundOverlaysCache() )

 private:
    class Private;
    Private *const d;
};

}

#endif
