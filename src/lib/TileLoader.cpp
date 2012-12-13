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
#include <QtCore/QMetaType>
#include <QtGui/QImage>

#include "GeoSceneTiled.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "TileLoaderHelper.h"

Q_DECLARE_METATYPE( Marble::DownloadUsage )

namespace Marble
{

TileLoader::TileLoader( HttpDownloadManager * const downloadManager )
{
    qRegisterMetaType<DownloadUsage>( "DownloadUsage" );
    connect( this, SIGNAL( downloadTile( QUrl, QString, QString, DownloadUsage )),
             downloadManager, SLOT( addJob( QUrl, QString, QString, DownloadUsage )));
    connect( downloadManager, SIGNAL( downloadComplete( QByteArray, QString )),
             SLOT( updateTile( QByteArray, QString )));
}

// If the tile is locally available:
//     - if not expired: create TextureTile, set state to "uptodate", return it => done
//     - if expired: create TextureTile, state is set to Expired by default, trigger dl,

QImage TileLoader::loadTile( GeoSceneTiled const *textureLayer, TileId const & tileId, DownloadUsage const usage )
{
    QString const fileName = tileFileName( textureLayer, tileId );
    TileStatus status = tileStatus( textureLayer, tileId );
    if ( status != Missing ) {
        // check if an update should be triggered

        if ( status == Available ) {
            mDebug() << Q_FUNC_INFO << tileId << "StateUptodate";
        } else {
            Q_ASSERT( status == Expired );
            mDebug() << Q_FUNC_INFO << tileId << "StateExpired";
            triggerDownload( textureLayer, tileId, usage );
        }

        QImage const image( fileName );
        if ( !image.isNull() ) {
            // file is there, so create and return a tile object in any case
            return image;
        }
    }

    // tile was not locally available => trigger download and look for tiles in other levels
    // for scaling
    QImage replacementTile = scaledLowerLevelTile( textureLayer, tileId );
    Q_ASSERT( !replacementTile.isNull() );

    triggerDownload( textureLayer, tileId, usage );

    return replacementTile;
}

// This method triggers a download of the given tile (without checking
// expiration). It is called by upper layer (StackedTileLoader) when the tile
// that should be reloaded is currently loaded in memory.
//
// post condition
//     - download is triggered
void TileLoader::downloadTile( GeoSceneTiled const *textureLayer, TileId const &tileId, DownloadUsage const usage )
{
    triggerDownload( textureLayer, tileId, usage );
}

int TileLoader::maximumTileLevel( GeoSceneTiled const & texture )
{
    // if maximum tile level is configured in the DGML files,
    // then use it, otherwise use old detection code.
    if ( texture.maximumTileLevel() >= 0 ) {
        return texture.maximumTileLevel();
    }

    int maximumTileLevel = -1;
    const QFileInfo themeStr( texture.themeStr() );
    const QString tilepath = themeStr.isAbsolute() ? themeStr.absoluteFilePath() : MarbleDirs::path( texture.themeStr() );
    //    mDebug() << "StackedTileLoader::maxPartialTileLevel tilepath" << tilepath;
    QStringList leveldirs = QDir( tilepath ).entryList( QDir::AllDirs | QDir::NoSymLinks
                                                        | QDir::NoDotAndDotDot );

    QStringList::const_iterator it = leveldirs.constBegin();
    QStringList::const_iterator const end = leveldirs.constEnd();
    for (; it != end; ++it ) {
        bool ok = true;
        const int value = (*it).toInt( &ok, 10 );

        if ( ok && value > maximumTileLevel )
            maximumTileLevel = value;
    }

    //    mDebug() << "Detected maximum tile level that contains data: "
    //             << maxtilelevel;
    return maximumTileLevel + 1;
}

bool TileLoader::baseTilesAvailable( GeoSceneTiled const & texture )
{
    const int  levelZeroColumns = texture.levelZeroColumns();
    const int  levelZeroRows    = texture.levelZeroRows();

    bool result = true;

    // Check whether the tiles from the lowest texture level are available
    //
    for ( int column = 0; result && column < levelZeroColumns; ++column ) {
        for ( int row = 0; result && row < levelZeroRows; ++row ) {
            const TileId id( 0, 0, column, row );
            const QString tilepath = tileFileName( &texture, id );
            result &= QFile::exists( tilepath );
            if (!result) {
                mDebug() << "Base tile " << texture.relativeTileFileName( id ) << " is missing for source dir " << texture.sourceDir();
            }
        }
    }

    return result;
}

TileLoader::TileStatus TileLoader::tileStatus( GeoSceneTiled const *textureLayer, const TileId &tileId )
{
    QString const fileName = tileFileName( textureLayer, tileId );
    QFileInfo fileInfo( fileName );
    if ( !fileInfo.exists() ) {
        return Missing;
    }

    const QDateTime lastModified = fileInfo.lastModified();
    const int expireSecs = textureLayer->expire();
    const bool isExpired = lastModified.secsTo( QDateTime::currentDateTime() ) >= expireSecs;
    return isExpired ? Expired : Available;
}

void TileLoader::updateTile( QByteArray const & data, QString const & idStr )
{
    QStringList const components = idStr.split( ':', QString::SkipEmptyParts );
    Q_ASSERT( components.size() == 4 );

    QString const sourceDir = components[ 0 ];
    int const zoomLevel = components[ 1 ].toInt();
    int const tileX = components[ 2 ].toInt();
    int const tileY = components[ 3 ].toInt();

    TileId const id = TileId( sourceDir, zoomLevel, tileX, tileY );

    QImage const tileImage = QImage::fromData( data );
    if ( tileImage.isNull() )
        return;

    emit tileCompleted( id, tileImage );
}

QString TileLoader::tileFileName( GeoSceneTiled const * textureLayer, TileId const & tileId )
{
    QString const fileName = textureLayer->relativeTileFileName( tileId );
    QFileInfo const dirInfo( fileName );
    return dirInfo.isAbsolute() ? fileName : MarbleDirs::path( fileName );
}

void TileLoader::triggerDownload( GeoSceneTiled const *textureLayer, TileId const &id, DownloadUsage const usage )
{
    QUrl const sourceUrl = textureLayer->downloadUrl( id );
    QString const destFileName = textureLayer->relativeTileFileName( id );
    QString const idStr = QString( "%1:%2:%3:%4" ).arg( textureLayer->sourceDir() ).arg( id.zoomLevel() ).arg( id.x() ).arg( id.y() );
    emit downloadTile( sourceUrl, destFileName, idStr, usage );
}

QImage TileLoader::scaledLowerLevelTile( GeoSceneTiled const *textureLayer, TileId const & id ) const
{
    mDebug() << Q_FUNC_INFO << id;

    for ( int level = qMax<int>( 0, id.zoomLevel() - 1 ); level >= 0; --level ) {
        int const deltaLevel = id.zoomLevel() - level;
        TileId const replacementTileId( id.mapThemeIdHash(), level,
                                        id.x() >> deltaLevel, id.y() >> deltaLevel );
        QString const fileName = tileFileName( textureLayer, replacementTileId );
        mDebug() << "TileLoader::scaledLowerLevelTile" << "trying" << fileName;
        QImage toScale( fileName );

        if ( level == 0 && toScale.isNull() ) {
            mDebug() << "No level zero tile installed in map theme dir. Falling back to a transparent image for now.";
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
            return part.scaled( toScale.size() );
        }
    }

    Q_ASSERT_X( false, "scaled image", "level zero image missing" ); // not reached
    return QImage();
}

}

#include "TileLoader.moc"
