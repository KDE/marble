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

#include "GeoSceneTexture.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoaderHelper.h"

namespace Marble
{

TileLoader::TileLoader( MapThemeManager const * const mapThemeManager,
                        HttpDownloadManager * const downloadManager )
{
    connect( this, SIGNAL( downloadTile( QUrl, QString, QString, DownloadUsage )),
             downloadManager, SLOT( addJob( QUrl, QString, QString, DownloadUsage )));
    connect( downloadManager, SIGNAL( downloadComplete( QByteArray, QString )),
             SLOT( updateTile( QByteArray, QString )));
}

// If the tile is locally available:
//     - if not expired: create TextureTile, set state to "uptodate", return it => done
//     - if expired: create TextureTile, state is set to Expired by default, trigger dl,

TextureTile * TileLoader::loadTile( TileId const & composedTileId, TileId const & baseTileId )
{
    QString const fileName = tileFileName( baseTileId );
    QFileInfo const fileInfo( fileName );
    if ( fileInfo.exists() ) {
        // file is there, so create and return a tile object in any case,
        // but check if an update should be triggered
        GeoSceneTexture const * const textureLayer = findTextureLayer( baseTileId );
        TextureTile * const tile = new TextureTile( baseTileId, fileName );
        tile->setComposedTileId( composedTileId );
        tile->setLastModified( fileInfo.lastModified() );
        tile->setExpireSecs( textureLayer->expire() );

        if ( !tile->expired() ) {
            mDebug() << "TileLoader::loadTile" << baseTileId.toString() << "StateUptodate";
            tile->setState( TextureTile::StateUptodate );
        } else {
            mDebug() << "TileLoader::loadTile" << baseTileId.toString() << "StateExpired";
            m_waitingForUpdate.insert( baseTileId, tile );
            triggerDownload( baseTileId );
        }
        return tile;
    }

    // tile was not locally available => trigger download and look for tiles in other levels
    // for scaling
    TextureTile * const tile = new TextureTile( baseTileId );
    tile->setComposedTileId( composedTileId );
    m_waitingForUpdate.insert( baseTileId, tile );
    triggerDownload( baseTileId );
    QImage * const replacementTile = scaledLowerLevelTile( baseTileId );
    if ( replacementTile ) {
        mDebug() << "TileLoader::loadTile" << baseTileId.toString() << "StateScaled";
        tile->setImage( replacementTile );
        tile->setState( TextureTile::StateScaled );
    } else {
        mDebug() << "TileLoader::loadTile" << baseTileId.toString() << "No tiles found";
    }
    return tile;
}

void TileLoader::updateTile( QByteArray const & data, QString const & tileId )
{
    TileId const id = TileId::fromString( tileId );
    TextureTile * const tile = m_waitingForUpdate.value( id, 0 );
    Q_ASSERT( tile );
    m_waitingForUpdate.remove( id );
    tile->setImage( data );
    tile->setState( TextureTile::StateUptodate );
    tile->setLastModified( QDateTime::currentDateTime() );
    emit tileCompleted( tile->composedTileId(), id );
}

inline GeoSceneTexture const * TileLoader::findTextureLayer( TileId const & id ) const
{
    GeoSceneTexture const * const textureLayer = m_textureLayers.value( id.mapThemeIdHash(), 0 );
    Q_ASSERT( textureLayer );
    return textureLayer;
}

inline GeoSceneTexture * TileLoader::findTextureLayer( TileId const & id )
{
    GeoSceneTexture * const textureLayer = m_textureLayers.value( id.mapThemeIdHash(), 0 );
    Q_ASSERT( textureLayer );
    return textureLayer;
}

QString TileLoader::tileFileName( TileId const & tileId ) const
{
    GeoSceneTexture const * const textureLayer = findTextureLayer( tileId );
    return MarbleDirs::path( TileLoaderHelper::relativeTileFileName
                             ( textureLayer, tileId.zoomLevel(), tileId.x(), tileId.y() ));
}

void TileLoader::triggerDownload( TileId const & id )
{
    GeoSceneTexture * const textureLayer = findTextureLayer( id );
    QUrl const sourceUrl = TileLoaderHelper::downloadUrl( textureLayer, id.zoomLevel(), id.x(),
                                                          id.y() );
    QString const destFileName = TileLoaderHelper::relativeTileFileName( textureLayer, id.zoomLevel(),
                                                                         id.x(), id.y() );
    emit downloadTile( sourceUrl, destFileName, id.toString(), DownloadBrowse );
}

    // TODO: get lastModified time stamp into the TextureTile
QImage * TileLoader::scaledLowerLevelTile( TileId const & id )
{
    mDebug() << "TileLoader::scaledLowerLevelTile" << id.toString();
    QImage * result = 0;
    int level = id.zoomLevel() - 1;
    while ( !result && level >= 0 ) {
        int const deltaLevel = id.zoomLevel() - level;
        TileId const replacementTileId( id.mapThemeIdHash(), level,
                                        id.x() >> deltaLevel, id.y() >> deltaLevel );
        mDebug() << "TileLoader::scaledLowerLevelTile" << "trying" << replacementTileId.toString();
        QString const fileName = tileFileName( replacementTileId );
        QFileInfo const fileInfo( fileName );
        if ( fileInfo.exists() ) {
            QImage const toScale( fileName );
            // which rect to scale?
            QSize const size = toScale.size();
            int const restTileX = id.x() % ( 1 << deltaLevel );
            int const restTileY = id.y() % ( 1 << deltaLevel );
            int const partWidth = toScale.width() >> deltaLevel;
            int const partHeight = toScale.height() >> deltaLevel;
            int const startX = restTileX * partWidth;
            int const startY = restTileY * partHeight;
            mDebug() << "QImage::copy:" << startX << startY << partWidth << partHeight;
            QImage const part = toScale.copy( startX, startY, partWidth, partHeight );
            mDebug() << "QImage::scaled:" << toScale.size();
            result = new QImage ( part.scaled( toScale.size() ));
        }
        --level;
    }
    return result;
}

}

#include "TileLoader.moc"
