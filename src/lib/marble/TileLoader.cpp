/*
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
 * Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TileLoader.h"

#include <QDateTime>
#include <QFileInfo>
#include <QMetaType>
#include <QImage>
#include <QUrl>

#include "GeoSceneTextureTileDataset.h"
#include "GeoSceneTileDataset.h"
#include "GeoSceneTypes.h"
#include "GeoSceneVectorTileDataset.h"
#include "GeoDataDocument.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "TileId.h"
#include "TileLoaderHelper.h"
#include "ParseRunnerPlugin.h"
#include "ParsingRunner.h"

Q_DECLARE_METATYPE( Marble::DownloadUsage )

namespace Marble
{

TileLoader::TileLoader(HttpDownloadManager * const downloadManager, const PluginManager *pluginManager) :
    m_pluginManager(pluginManager)
{
    qRegisterMetaType<DownloadUsage>( "DownloadUsage" );
    connect( this, SIGNAL(downloadTile(QUrl,QString,QString,DownloadUsage)),
             downloadManager, SLOT(addJob(QUrl,QString,QString,DownloadUsage)));
    connect( downloadManager, SIGNAL(downloadComplete(QString,QString)),
             SLOT(updateTile(QString,QString)));
    connect( downloadManager, SIGNAL(downloadComplete(QByteArray,QString)),
             SLOT(updateTile(QByteArray,QString)));
}

TileLoader::~TileLoader()
{
    // nothing to do
}

// If the tile image file is locally available:
//     - if not expired: create ImageTile, set state to "uptodate", return it => done
//     - if expired: create TextureTile, state is set to Expired by default, trigger dl,
QImage TileLoader::loadTileImage( GeoSceneTextureTileDataset const *textureLayer, TileId const & tileId, DownloadUsage const usage )
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


GeoDataDocument *TileLoader::loadTileVectorData( GeoSceneVectorTileDataset const *textureLayer, TileId const & tileId, DownloadUsage const usage )
{
    // FIXME: textureLayer->fileFormat() could be used in the future for use just that parser, instead of all available parsers

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

        QFile file ( fileName );
        if ( file.exists() ) {

            // File is ready, so parse and return the vector data in any case
            GeoDataDocument* document = openVectorFile(fileName);
            if (document) {
                return document;
            }
        }
    }

    // tile was not locally available => trigger download
    triggerDownload( textureLayer, tileId, usage );
    return nullptr;
}

// This method triggers a download of the given tile (without checking
// expiration). It is called by upper layer (StackedTileLoader) when the tile
// that should be reloaded is currently loaded in memory.
//
// post condition
//     - download is triggered
void TileLoader::downloadTile( GeoSceneTileDataset const *tileData, TileId const &tileId, DownloadUsage const usage )
{
    triggerDownload( tileData, tileId, usage );
}

int TileLoader::maximumTileLevel( GeoSceneTileDataset const & tileData )
{
    // if maximum tile level is configured in the DGML files,
    // then use it, otherwise use old detection code.
    if ( tileData.maximumTileLevel() >= 0 ) {
        return tileData.maximumTileLevel();
    }

    int maximumTileLevel = -1;
    const QFileInfo themeStr( tileData.themeStr() );
    const QString tilepath = themeStr.isAbsolute() ? themeStr.absoluteFilePath() : MarbleDirs::path( tileData.themeStr() );
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

bool TileLoader::baseTilesAvailable( GeoSceneTileDataset const & tileData )
{
    const int  levelZeroColumns = tileData.levelZeroColumns();
    const int  levelZeroRows    = tileData.levelZeroRows();

    bool result = true;

    // Check whether the tiles from the lowest texture level are available
    //
    for ( int column = 0; result && column < levelZeroColumns; ++column ) {
        for ( int row = 0; result && row < levelZeroRows; ++row ) {
            const TileId id( 0, 0, column, row );
            const QString tilepath = tileFileName( &tileData, id );
            result &= QFile::exists( tilepath );
            if (!result) {
                mDebug() << "Base tile " << tileData.relativeTileFileName( id ) << " is missing for source dir " << tileData.sourceDir();
            }
        }
    }

    return result;
}

TileLoader::TileStatus TileLoader::tileStatus( GeoSceneTileDataset const *tileData, const TileId &tileId )
{
    QString const fileName = tileFileName( tileData, tileId );
    QFileInfo fileInfo( fileName );
    if ( !fileInfo.exists() ) {
        return Missing;
    }

    const QDateTime lastModified = fileInfo.lastModified();
    const int expireSecs = tileData->expire();
    const bool isExpired = lastModified.secsTo( QDateTime::currentDateTime() ) >= expireSecs;
    return isExpired ? Expired : Available;
}

void TileLoader::updateTile( QByteArray const & data, QString const & idStr )
{
    QStringList const components = idStr.split(QLatin1Char(':'), QString::SkipEmptyParts);
    Q_ASSERT( components.size() == 5 );

    QString const origin = components[0];
    QString const sourceDir = components[ 1 ];
    int const zoomLevel = components[ 2 ].toInt();
    int const tileX = components[ 3 ].toInt();
    int const tileY = components[ 4 ].toInt();

    TileId const id = TileId( sourceDir, zoomLevel, tileX, tileY );

    if (origin == GeoSceneTypes::GeoSceneTextureTileType) {
        QImage const tileImage = QImage::fromData( data );
        if ( tileImage.isNull() )
            return;

        emit tileCompleted( id, tileImage );
    }
}

void TileLoader::updateTile(const QString &fileName, const QString &idStr)
{
    QStringList const components = idStr.split(QLatin1Char(':'), QString::SkipEmptyParts);
    Q_ASSERT( components.size() == 5 );

    QString const origin = components[0];
    QString const sourceDir = components[ 1 ];
    int const zoomLevel = components[ 2 ].toInt();
    int const tileX = components[ 3 ].toInt();
    int const tileY = components[ 4 ].toInt();

    TileId const id = TileId( sourceDir, zoomLevel, tileX, tileY );
    if (origin == GeoSceneTypes::GeoSceneVectorTileType) {
        GeoDataDocument* document = openVectorFile(MarbleDirs::path(fileName));
        if (document) {
            emit tileCompleted(id,  document);
        }
    }
}

QString TileLoader::tileFileName( GeoSceneTileDataset const * tileData, TileId const & tileId )
{
    QString const fileName = tileData->relativeTileFileName( tileId );
    QFileInfo const dirInfo( fileName );
    return dirInfo.isAbsolute() ? fileName : MarbleDirs::path( fileName );
}

void TileLoader::triggerDownload( GeoSceneTileDataset const *tileData, TileId const &id, DownloadUsage const usage )
{
    if (id.zoomLevel() > 0) {
        int minValue = tileData->maximumTileLevel() == -1 ? id.zoomLevel() : qMin( id.zoomLevel(), tileData->maximumTileLevel() );
        if (id.zoomLevel() != qMax(tileData->minimumTileLevel(), minValue) ) {
            // Download only level 0 tiles and tiles between minimum and maximum tile level
            return;
        }
    }

    QUrl const sourceUrl = tileData->downloadUrl( id );
    QString const destFileName = tileData->relativeTileFileName( id );
    QString const idStr = QString( "%1:%2:%3:%4:%5" ).arg( tileData->nodeType()).arg( tileData->sourceDir() ).arg( id.zoomLevel() ).arg( id.x() ).arg( id.y() );
    emit downloadTile( sourceUrl, destFileName, idStr, usage );
}

QImage TileLoader::scaledLowerLevelTile( const GeoSceneTextureTileDataset * textureData, TileId const & id )
{
    mDebug() << Q_FUNC_INFO << id;

    int const minimumLevel = textureData->minimumTileLevel();
    for ( int level = qMax<int>( 0, id.zoomLevel() - 1 ); level >= 0; --level ) {
        if (level > 0 && level < minimumLevel) {
            continue;
        }
        int const deltaLevel = id.zoomLevel() - level;

        TileId const replacementTileId( id.mapThemeIdHash(), level,
                                        id.x() >> deltaLevel, id.y() >> deltaLevel );
        QString const fileName = tileFileName( textureData, replacementTileId );
        mDebug() << "TileLoader::scaledLowerLevelTile" << "trying" << fileName;
        QImage toScale = QFile::exists(fileName) ? QImage(fileName) : QImage();

        if ( level == 0 && toScale.isNull() ) {
            mDebug() << "No level zero tile installed in map theme dir. Falling back to a transparent image for now.";
            QSize tileSize = textureData->tileSize();
            Q_ASSERT( !tileSize.isEmpty() ); // assured by textureLayer
            toScale = QImage( tileSize, QImage::Format_ARGB32_Premultiplied );
            toScale.fill( qRgba( 0, 0, 0, 0 ) );
        }

        if ( !toScale.isNull() ) {
            // which rect to scale?
            int const restTileX = id.x() % ( 1 << deltaLevel );
            int const restTileY = id.y() % ( 1 << deltaLevel );
            int const partWidth = qMax(1, toScale.width() >> deltaLevel);
            int const partHeight = qMax(1, toScale.height() >> deltaLevel);
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

GeoDataDocument *TileLoader::openVectorFile(const QString &fileName) const
{
    QList<const ParseRunnerPlugin*> plugins = m_pluginManager->parsingRunnerPlugins();
    const QFileInfo fileInfo( fileName );
    const QString suffix = fileInfo.suffix().toLower();
    const QString completeSuffix = fileInfo.completeSuffix().toLower();

    foreach( const ParseRunnerPlugin *plugin, plugins ) {
        QStringList const extensions = plugin->fileExtensions();
        if ( extensions.contains( suffix ) || extensions.contains( completeSuffix ) ) {
            ParsingRunner* runner = plugin->newRunner();
            QString error;
            GeoDataDocument* document = runner->parseFile(fileName, UserDocument, error);
            if (!document && !error.isEmpty()) {
                mDebug() << QString("Failed to open vector tile %1: %2").arg(fileName).arg(error);
            }
            delete runner;
            return document;
        }
    }

    mDebug() << "Unable to open vector tile " << fileName << ": No suitable plugin registered to parse this file format";
    return nullptr;
}

}

#include "moc_TileLoader.cpp"
