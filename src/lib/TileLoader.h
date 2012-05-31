// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
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

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtGui/QImage>

#include "TileId.h"
#include "MarbleGlobal.h"

class QByteArray;
class QImage;
class QUrl;

namespace Marble
{
class HttpDownloadManager;
class GeoSceneTexture;

class TileLoader: public QObject
{
    Q_OBJECT

 public:
    enum TileStatus {
        Missing,
        Expired,
        Available
    };

    explicit TileLoader( HttpDownloadManager * const );

    void setTextureLayers( const QVector<GeoSceneTexture const *> &textureLayers );

    QImage loadTile( TileId const & tileId, DownloadUsage const );
    void reloadTile( TileId const &tileId, DownloadUsage const );
    void downloadTile( TileId const & tileId );

    static int maximumTileLevel( GeoSceneTexture const & texture );

    /**
     * Returns whether the mandatory most basic tile level is fully available for
     * the given @p texture layer.
     */
    static bool baseTilesAvailable( GeoSceneTexture const & texture );

    /**
      * Returns the status of the downloaded tile file:
      * - Missing when it has not been downloaded
      * - Expired when it has been downloaded, but is too old (as per .dgml expiration time)
      * - Available when it has been downloaded and is not expired
      */
    TileStatus tileStatus( const TileId &tileId ) const;

 public Q_SLOTS:
    void updateTile( QByteArray const & imageData, QString const & tileId );

 Q_SIGNALS:
    void downloadTile( QUrl const & sourceUrl, QString const & destinationFileName,
                       QString const & id, DownloadUsage );

    void tileCompleted( TileId const & tileId, QImage const & tileImage );

 private:
    GeoSceneTexture const * findTextureLayer( TileId const & ) const;
    static QString tileFileName( GeoSceneTexture const * textureLayer, TileId const & );
    void triggerDownload( TileId const &, DownloadUsage const );
    QImage scaledLowerLevelTile( TileId const & ) const;

    // TODO: comment about uint hash key
    QHash<uint, GeoSceneTexture const *> m_textureLayers;
};

}

#endif
