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

#include "TileLoader.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtGui/QImage>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoSceneTexture.h"
#include "HttpDownloadManager.h"
#include "MapThemeManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoaderHelper.h"

namespace Marble
{

TileLoader::TileLoader( HttpDownloadManager * const downloadManager, MapThemeManager const * mapThemeManager )
    : m_mapThemeManager( mapThemeManager )
{
    connect( this, SIGNAL( downloadTile( QUrl, QString, QString, DownloadUsage )),
             downloadManager, SLOT( addJob( QUrl, QString, QString, DownloadUsage )));
    connect( downloadManager, SIGNAL( downloadComplete( QByteArray, QString )),
             SLOT( updateTile( QByteArray, QString )));
    connect( m_mapThemeManager, SIGNAL( themesChanged() ),
             this, SLOT( updateTextureLayers() ) );
    updateTextureLayers();
}

// If the tile is locally available:
//     - if not expired: create TextureTile, set state to "uptodate", return it => done
//     - if expired: create TextureTile, state is set to Expired by default, trigger dl,

QSharedPointer<TextureTile> TileLoader::loadTile( TileId const & tileId, DownloadUsage const usage )
{
    GeoSceneTexture const * const textureLayer = findTextureLayer( tileId );
    QString const fileName = tileFileName( tileId );
    QImage const image( fileName );
    if ( !image.isNull() ) {
        // file is there, so create and return a tile object in any case,
        // but check if an update should be triggered
        QSharedPointer<TextureTile> const tile( new TextureTile( tileId, new QImage( image ), textureLayer->blending() ));

        const QDateTime lastModified = QFileInfo( fileName ).lastModified();
        const int expireSecs = textureLayer->expire();
        const bool isExpired = lastModified.secsTo( QDateTime::currentDateTime() ) >= expireSecs;

        if ( !isExpired ) {
            mDebug() << "TileLoader::loadTile" << tileId.toString() << "StateUptodate";
        } else {
            mDebug() << "TileLoader::loadTile" << tileId.toString() << "StateExpired";
            m_waitingForUpdate.insert( tileId );
            triggerDownload( tileId, usage );
        }
        return tile;
    }

    // tile was not locally available => trigger download and look for tiles in other levels
    // for scaling
    QImage * replacementTile = scaledLowerLevelTile( tileId );
    Q_ASSERT( replacementTile && !replacementTile->isNull() );

    QSharedPointer<TextureTile> const tile( new TextureTile( tileId, replacementTile, textureLayer->blending() ));

    m_waitingForUpdate.insert( tileId );
    triggerDownload( tileId, usage );

    return tile;
}

// This method triggers a download of the given tile (without checking
// expiration). It is called by upper layer (StackedTileLoader) when the tile
// that should be reloaded is currently loaded in memory.
//
// post condition
//     - download is triggered, but only if not in progress (indicated by
//       m_waitingForUpdate)
void TileLoader::reloadTile( TileId const &tileId, DownloadUsage const usage )
{
    if ( m_waitingForUpdate.contains( tileId ) )
        return;
    m_waitingForUpdate.insert( tileId );
    triggerDownload( tileId, usage );
}

void TileLoader::downloadTile( TileId const & tileId )
{
    triggerDownload( tileId, DownloadBulk );
}

void TileLoader::updateTile( QByteArray const & data, QString const & tileId )
{
    Q_UNUSED( data ); // image data has been written to disk by HttpDownloadManager,
                      // so it can be loaded using loadTile()

    TileId const id = TileId::fromString( tileId );

    // preliminary fix for reload map crash
    // TODO: fix properly
    if ( !m_waitingForUpdate.contains( id ) )
        return;

    m_waitingForUpdate.remove( id );

    emit tileCompleted( id );
}

void TileLoader::updateTextureLayers()
{
    m_textureLayers.clear();

    QHash<uint, GeoSceneLayer const *> sceneLayers;

    QList<GeoSceneDocument const *> const & mapThemes = m_mapThemeManager->mapThemes();
    QList<GeoSceneDocument const *>::const_iterator pos = mapThemes.constBegin();
    QList<GeoSceneDocument const *>::const_iterator const end = mapThemes.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneHead const * head = (*pos)->head();
        Q_ASSERT( head );
        const QString mapThemeId = head->target() + '/' + head->theme();
        mDebug() << "TileLoader::updateTextureLayers" << mapThemeId;

        GeoSceneMap const * map = (*pos)->map();
        Q_ASSERT( map );
        GeoSceneLayer const * sceneLayer = map->layer( head->theme() );
        if ( !sceneLayer ) {
            mDebug() << "ignoring, has no GeoSceneLayer for" << head->theme();
            continue;
        }

        uint const mapThemeIdHash = qHash( mapThemeId );
        if ( sceneLayers.contains( mapThemeIdHash ) ) {
            mDebug() << "TileLoader::updateTextureLayers:"
                     << mapThemeIdHash << mapThemeId
                     << "already exists";
            continue;
        }

        sceneLayers.insert( mapThemeIdHash, sceneLayer );

        // find all texture layers
        QVector<GeoSceneAbstractDataset *> layers = sceneLayer->datasets();
        QVector<GeoSceneAbstractDataset *>::const_iterator pos = layers.constBegin();
        QVector<GeoSceneAbstractDataset *>::const_iterator const end = layers.constEnd();
        for (; pos != end; ++pos ) {
            GeoSceneTexture const * const textureLayer = dynamic_cast<GeoSceneTexture *>( *pos );
            if ( !textureLayer ) {
                mDebug() << "ignoring dataset, is not a texture layer";
                continue;
            }
            m_textureLayers.insert( qHash( textureLayer->sourceDir() ), textureLayer );
            mDebug() << "TileLoader::updateTextureLayers" << "added texture layer:"
                     << qHash( textureLayer->sourceDir() ) << textureLayer->sourceDir();
        }
    }
}

inline GeoSceneTexture const * TileLoader::findTextureLayer( TileId const & id ) const
{
    GeoSceneTexture const * const textureLayer = m_textureLayers.value( id.mapThemeIdHash(), 0 );
    Q_ASSERT( textureLayer );
    return textureLayer;
}

QString TileLoader::tileFileName( TileId const & tileId ) const
{
    GeoSceneTexture const * const textureLayer = findTextureLayer( tileId );
    QString const fileName = textureLayer->relativeTileFileName( tileId );
    QFileInfo const dirInfo( fileName );
    return dirInfo.isAbsolute() ? fileName : MarbleDirs::path( fileName );
}

void TileLoader::triggerDownload( TileId const & id, DownloadUsage const usage )
{
    GeoSceneTexture const * const textureLayer = findTextureLayer( id );
    QUrl const sourceUrl = textureLayer->downloadUrl( id );
    QString const destFileName = textureLayer->relativeTileFileName( id );
    emit downloadTile( sourceUrl, destFileName, id.toString(), usage );
}

    // TODO: get lastModified time stamp into the TextureTile
QImage * TileLoader::scaledLowerLevelTile( TileId const & id )
{
    mDebug() << "TileLoader::scaledLowerLevelTile" << id.toString();

    for ( int level = qMax<int>( 0, id.zoomLevel() - 1 ); level >= 0; --level ) {
        int const deltaLevel = id.zoomLevel() - level;
        TileId const replacementTileId( id.mapThemeIdHash(), level,
                                        id.x() >> deltaLevel, id.y() >> deltaLevel );
        QString const fileName = tileFileName( replacementTileId );
        mDebug() << "TileLoader::scaledLowerLevelTile" << "trying" << fileName;
        QImage toScale( fileName );

        if ( level == 0 && toScale.isNull() ) {
            mDebug() << "No level zero tile installed in map theme dir. Falling back to a transparent image for now.";
            GeoSceneTexture const * const textureLayer = findTextureLayer( replacementTileId );
            QSize tileSize = textureLayer->tileSize();
            Q_ASSERT( !tileSize.isEmpty() ); // assured by textureLayer
            toScale = QImage( tileSize, QImage::Format_ARGB32_Premultiplied );
            toScale.fill( qRgba( 0, 0, 0, 0 ) );
        }

        if ( !toScale.isNull() ) {
            // which rect to scale?
            int const restTileX = id.x() % ( 1 << deltaLevel );
            int const restTileY = id.y() % ( 1 << deltaLevel );
            int const partWidth = toScale.width() >> deltaLevel;
            int const partHeight = toScale.height() >> deltaLevel;
            int const startX = restTileX * partWidth;
            int const startY = restTileY * partHeight;
            mDebug() << "QImage::copy:" << startX << startY << partWidth << partHeight;
            QImage const part = toScale.copy( startX, startY, partWidth, partHeight );
            mDebug() << "QImage::scaled:" << toScale.size();
            return new QImage( part.scaled( toScale.size() ) );
        }
    }

    Q_ASSERT_X( false, "scaled image", "level zero image missing" ); // not reached
    return 0;
}

}

#include "TileLoader.moc"
