//
// This file is part of the Marble Virtual Globe.
//
// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_TILELOADER_H
#define MARBLE_TILELOADER_H

#include <QObject>
#include <QString>
#include <QImage>

#include "GeoSceneTiled.h"

class QByteArray;
class QImage;
class QUrl;

namespace Marble
{
class HttpDownloadManager;
class GeoDataDocument;
class GeoSceneTextureTile;
class GeoSceneVectorTile;
class PluginManager;

class TileLoader: public QObject
{
    Q_OBJECT

 public:
    enum TileStatus {
        Missing,
        Expired,
        Available
    };

    explicit TileLoader( const GeoSceneTextureTile *layer, HttpDownloadManager *downloadManager );
    explicit TileLoader( const GeoSceneVectorTile *layer, HttpDownloadManager *downloadManager, const PluginManager * );

    const GeoSceneTiled *layer() const;

    QSize tileSize() const;

    GeoSceneTiled::Projection projection() const;

    QString name() const;

    QString fileFormat() const;

    QString blending() const;

    int levelZeroColumns() const;
    int levelZeroRows() const;

    QString sourceDir() const;

    QImage loadTileImage( TileId const & tileId, DownloadUsage const );
    GeoDataDocument* loadTileVectorData( TileId const & tileId, DownloadUsage const usage );
    void downloadTile( TileId const &, DownloadUsage const );

    static int maximumTileLevel( GeoSceneTiled const & texture );

    int maximumTileLevel() const;
    bool hasMaximumTileLevel() const;

    /**
     * Returns whether the mandatory most basic tile level is fully available for
     * the given @p texture layer.
     */
    static bool baseTilesAvailable( GeoSceneTiled const & texture );

    /**
      * Returns the status of the downloaded tile file:
      * - Missing when it has not been downloaded
      * - Expired when it has been downloaded, but is too old (as per .dgml expiration time)
      * - Available when it has been downloaded and is not expired
      */
    static TileStatus tileStatus( GeoSceneTiled const *textureLayer, const TileId &tileId );

    TileStatus tileStatus( const TileId &tileId ) const;

 public Q_SLOTS:
    void updateTile( QByteArray const & imageData, QString const & tileId );

 Q_SIGNALS:
    void downloadTile( QUrl const & sourceUrl, QString const & destinationFileName,
                       QString const & id, DownloadUsage );

    void tileCompleted( TileId const & tileId, QImage const & tileImage );

    void tileCompleted( TileId const & tileId, GeoDataDocument * document, QString const & format );

 private:
    static QString tileFileName( const GeoSceneTiled *textureLayer, TileId const & );
    void triggerDownload( TileId const &, DownloadUsage const );
    QImage scaledLowerLevelTile( TileId const & ) const;

    const GeoSceneTiled *const m_layer;
    HttpDownloadManager *const m_downloadManager;
    const PluginManager *const m_pluginManager; // For vectorTile parsing
};

}

#endif
