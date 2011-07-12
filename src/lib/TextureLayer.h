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
class GeoSceneDocument;
class HttpDownloadManager;
class MapThemeManager;
class SunLocator;
class TextureColorizer;
class ViewParams;

class TextureLayer : public QObject
{
    Q_OBJECT

 public:
    TextureLayer( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator );
    ~TextureLayer();

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
    void paintGlobe( GeoPainter *painter,
                     ViewParams *viewParams,
                     const QRect& dirtyRect );

    void setShowTileId( bool show );

    void setTextureColorizer( TextureColorizer *texcolorizer );

    /**
     * @brief  Set the Projection used for the map
     * @param  projection projection type (e.g. Spherical, Equirectangular, Mercator)
     */
    void setupTextureMapper( Projection projection );

    void setNeedsUpdate();

    void setMapTheme( GeoSceneDocument* mapTheme );

    void setVolatileCacheLimit( quint64 kilobytes );

    void update();

    void reload();

    void downloadTile( const TileId &tileId );

 Q_SIGNALS:
    void tileLevelChanged( int );
    void repaintNeeded( const QRegion & );

 private:
    Q_PRIVATE_SLOT( d, void mapChanged() )
    Q_PRIVATE_SLOT( d, void updateTextureLayers() )

 private:
    class Private;
    Private *const d;
};

}

#endif
