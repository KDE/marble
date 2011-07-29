/**
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>
 * Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jensmh@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "StackedTileLoader.h"

#include "blendings/BlendingFactory.h"
#include "GeoSceneTexture.h"
#include "MarbleDebug.h"
#include "MergedLayerDecorator.h"
#include "StackedTile.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "global.h"

#include <QtCore/QCache>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QReadWriteLock>
#include <QtCore/QSet>
#include <QtGui/QImage>


namespace Marble
{

class StackedTileLoaderPrivate
{
public:
    StackedTileLoaderPrivate( TileLoader *tileLoader,
                              SunLocator * const sunLocator )
        : m_tileLoader( tileLoader ),
          m_blendingFactory( sunLocator ),
          m_layerDecorator( m_tileLoader, sunLocator ),
          m_maxTileLevel( 0 )
    {
        m_tileCache.setMaxCost( 20000 * 1024 ); // Cache size measured in bytes
    }

    const StackedTile *createTile( TileId const &stackedTileId );

    void detectMaxTileLevel();
    QVector<GeoSceneTexture const *>
        findRelevantTextureLayers( TileId const & stackedTileId ) const;

    TileLoader *const m_tileLoader;
    BlendingFactory m_blendingFactory;
    MergedLayerDecorator m_layerDecorator;
    int         m_maxTileLevel;
    QVector<GeoSceneTexture const *> m_textureLayers;
    QHash <TileId, const StackedTile*> m_tilesOnDisplay;
    QHash <TileId, const StackedTile*> m_levelZeroHash;
    QCache <TileId, const StackedTile>  m_tileCache;
    QReadWriteLock m_cacheLock;
};

StackedTileLoader::StackedTileLoader( TileLoader *tileLoader,
                                      SunLocator * const sunLocator )
    : d( new StackedTileLoaderPrivate( tileLoader, sunLocator ) )
{
}

StackedTileLoader::~StackedTileLoader()
{
    qDeleteAll( d->m_levelZeroHash );
    delete d;
}

void StackedTileLoader::setTextureLayers( QVector<GeoSceneTexture const *> & textureLayers )
{
    mDebug() << "StackedTileLoader::setTextureLayers";

    d->m_textureLayers = textureLayers;

    if ( !d->m_textureLayers.isEmpty() ) {
        const GeoSceneTexture *const firstTexture = d->m_textureLayers.at( 0 );
        d->m_layerDecorator.setLevelZeroLayout( firstTexture->levelZeroColumns(), firstTexture->levelZeroRows() );
        d->m_layerDecorator.setThemeId( "maps/" + d->m_textureLayers.at( 0 )->sourceDir() );
    }

    clear();

    d->detectMaxTileLevel();
}

void StackedTileLoader::setShowSunShading( bool show )
{
    d->m_layerDecorator.setShowSunShading( show );
}

bool StackedTileLoader::showSunShading() const
{
    return d->m_layerDecorator.showSunShading();
}

void StackedTileLoader::setShowCityLights( bool show )
{
    d->m_layerDecorator.setShowCityLights( show );
}

bool StackedTileLoader::showCityLights() const
{
    return d->m_layerDecorator.showCityLights();
}

void StackedTileLoader::setShowTileId( bool show )
{
    d->m_layerDecorator.setShowTileId( show );
}

int StackedTileLoader::tileColumnCount( int level ) const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    const int levelZeroColumns = d->m_textureLayers.at( 0 )->levelZeroColumns();

    return TileLoaderHelper::levelToColumn( levelZeroColumns, level );
}

int StackedTileLoader::tileRowCount( int level ) const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    const int levelZeroRows = d->m_textureLayers.at( 0 )->levelZeroRows();

    return TileLoaderHelper::levelToRow( levelZeroRows, level );
}

GeoSceneTexture::Projection StackedTileLoader::tileProjection() const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    return d->m_textureLayers.at( 0 )->projection();
}

QSize StackedTileLoader::tileSize() const
{
    Q_ASSERT( !d->m_textureLayers.isEmpty() );

    return d->m_textureLayers.at( 0 )->tileSize();
}

void StackedTileLoader::resetTilehash()
{
    d->m_tilesOnDisplay.clear();
}

void StackedTileLoader::cleanupTilehash()
{
}

const StackedTile* StackedTileLoader::loadTile( TileId const & stackedTileId )
{
    // check if the tile is in the hash
    d->m_cacheLock.lockForRead();
    const StackedTile *stackedTile = d->m_tilesOnDisplay.value( stackedTileId );
    d->m_cacheLock.unlock();
    if ( stackedTile )
        return stackedTile;

    d->m_cacheLock.lockForWrite();

    // has another thread loaded our tile due to a race condition?
    stackedTile = d->m_tilesOnDisplay.value( stackedTileId, 0 );
    if ( d->m_tilesOnDisplay.contains( stackedTileId ) ) {
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    // the tile was not in the hash so check if it is in the cache
    stackedTile = d->m_tileCache.object( stackedTileId );
    if ( stackedTile ) {
        d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    for ( int tileLevel = stackedTileId.zoomLevel() - 1; tileLevel > 0; --tileLevel ) {
        const int deltaLevel = stackedTileId.zoomLevel() - tileLevel;
        const TileId id = TileId( 0, tileLevel, stackedTileId.x() >> deltaLevel, stackedTileId.y() >> deltaLevel );
        const StackedTile *const stackedTile = d->m_tileCache.object( id );
        if ( stackedTile ) {
            d->m_tilesOnDisplay.insert( stackedTileId, stackedTile );
            d->m_cacheLock.unlock();
            return stackedTile;
        }
    }

    stackedTile = d->m_levelZeroHash.value( TileId( 0, 0, stackedTileId.x() >> stackedTileId.zoomLevel(), stackedTileId.y() >> stackedTileId.zoomLevel() ), 0 );
    d->m_tilesOnDisplay.insert( stackedTileId, stackedTile );
    d->m_cacheLock.unlock();

    Q_ASSERT( stackedTile );

    return stackedTile;
}

const StackedTile *StackedTileLoaderPrivate::createTile( TileId const & stackedTileId )
{
    QVector<QSharedPointer<TextureTile> > tiles;
    QVector<GeoSceneTexture const *> const textureLayers = findRelevantTextureLayers( stackedTileId );
    QVector<GeoSceneTexture const *>::const_iterator pos = textureLayers.constBegin();
    QVector<GeoSceneTexture const *>::const_iterator const end = textureLayers.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneTexture const * const textureLayer = *pos;
        TileId const tileId( textureLayer->sourceDir(), stackedTileId.zoomLevel(),
                             stackedTileId.x(), stackedTileId.y() );
        mDebug() << "StackedTileLoader::loadTile: tile" << textureLayer->sourceDir()
                 << tileId.toString() << textureLayer->tileSize();
        const QImage tileImage = m_tileLoader->loadTile( tileId, DownloadBrowse );
        const Blending *blending = m_blendingFactory.findBlending( textureLayer->blending() );
        if ( blending == 0 && !textureLayer->blending().isEmpty() ) {
            mDebug() << Q_FUNC_INFO << "could not find blending" << textureLayer->blending();
        }
        QSharedPointer<TextureTile> tile( new TextureTile( tileId, tileImage, blending ) );
        tiles.append( tile );
    }
    Q_ASSERT( !tiles.isEmpty() );

    const QImage resultImage = m_layerDecorator.merge( stackedTileId, tiles );
    const StackedTile *const stackedTile = new StackedTile( stackedTileId, resultImage, tiles );

    return stackedTile;
}

void StackedTileLoader::downloadTile( TileId const & stackedTileId )
{
    QVector<GeoSceneTexture const *> const textureLayers = d->findRelevantTextureLayers( stackedTileId );
    QVector<GeoSceneTexture const *>::const_iterator pos = textureLayers.constBegin();
    QVector<GeoSceneTexture const *>::const_iterator const end = textureLayers.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneTexture const * const textureLayer = *pos;
        TileId const tileId( textureLayer->sourceDir(), stackedTileId.zoomLevel(),
                             stackedTileId.x(), stackedTileId.y() );
        d->m_tileLoader->downloadTile( tileId );
    }
}

quint64 StackedTileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.maxCost() / 1024;
}

void StackedTileLoader::reloadVisibleTiles()
{
    foreach ( TileId const & id, d->m_tilesOnDisplay.keys() ) {
        if ( id.zoomLevel() != 0 && !d->m_tileCache.contains( id ) ) {
            const StackedTile * tile = d->createTile( id );
            d->m_tileCache.insert( id, tile, tile->numBytes() );
        }
        const StackedTile *const displayedTile = ( id.zoomLevel() == 0 ? d->m_levelZeroHash.value( id )
                                                                       : d->m_tileCache.object( id ) );
        foreach ( QSharedPointer<TextureTile> const & tile, displayedTile->tiles() ) {
            // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
            // but since "reload" or "refresh" seems to be a common action of a browser and it
            // allows for more connections (in our model), use "DownloadBrowse"
            d->m_tileLoader->reloadTile( tile->id(), DownloadBrowse );
        }
    }

    emit tileUpdatesAvailable();
}

int StackedTileLoader::maximumTileLevel() const
{
    return d->m_maxTileLevel;
}

void StackedTileLoaderPrivate::detectMaxTileLevel()
{
    if ( m_textureLayers.isEmpty() ) {
        m_maxTileLevel = -1;
        return;
    }

    m_maxTileLevel = TileLoader::maximumTileLevel( *m_textureLayers.at( 0 ) );
}


void StackedTileLoader::setVolatileCacheLimit( quint64 kiloBytes )
{
    mDebug() << QString("Setting tile cache to %1 kilobytes.").arg( kiloBytes );
    d->m_tileCache.setMaxCost( kiloBytes * 1024 );
}

void StackedTileLoader::update()
{
    QSet<TileId> requiredTiles;

    foreach ( TileId const & stackedTileId, d->m_tilesOnDisplay.keys() ) {
        for ( int tileLevel = stackedTileId.zoomLevel(); tileLevel > 0; --tileLevel ) {
            const int levDiff = stackedTileId.zoomLevel() - tileLevel;
            const TileId id = TileId( 0, tileLevel, stackedTileId.x() >> levDiff, stackedTileId.y() >> levDiff );
            requiredTiles.insert( id );
        }
    }

    bool needsUpdate = false;
    foreach ( TileId const & id, requiredTiles ) {
            if ( !d->m_tileCache.contains( id ) ) {
                const StackedTile *const tile = d->createTile( id );
                d->m_tileCache.insert( id , tile, tile->numBytes() );
                needsUpdate = true;
            }
    }

    if ( needsUpdate ) {
        emit tileUpdatesAvailable();
    }
}

void StackedTileLoader::updateTile( TileId const &tileId, QImage const &tileImage )
{
    Q_ASSERT( !tileImage.isNull() );

    d->detectMaxTileLevel();

    const TileId stackedTileId( 0, tileId.zoomLevel(), tileId.x(), tileId.y() );

    if ( d->m_tilesOnDisplay.contains( stackedTileId ) ) {
        const StackedTile *displayedTile = ( stackedTileId.zoomLevel() == 0 ? d->m_levelZeroHash.take( stackedTileId )
                                                                            : d->m_tileCache.take( stackedTileId ) );

        QVector<QSharedPointer<TextureTile> > tiles = ( displayedTile != 0 ? displayedTile->tiles()
                                                                           : QVector<QSharedPointer<TextureTile> >() );
        delete displayedTile;
        displayedTile = 0;

        if ( !tiles.isEmpty() ) {
            for ( int i = 0; i < tiles.count(); ++ i) {
                if ( tiles[i]->id() == tileId ) {
                    const Blending *blending = tiles[i]->blending();
                    tiles[i] = QSharedPointer<TextureTile>( new TextureTile( tileId, tileImage, blending ) );
                }
            }

            const QImage resultImage = d->m_layerDecorator.merge( stackedTileId, tiles );
            displayedTile = new StackedTile( stackedTileId, resultImage, tiles );
        } else {
            displayedTile = d->createTile( stackedTileId );
        }

        if ( stackedTileId.zoomLevel() == 0 ) {
            d->m_levelZeroHash.insert( stackedTileId, displayedTile );
        } else {
            d->m_tileCache.insert( stackedTileId, displayedTile, displayedTile->numBytes() );
        }

        emit tileUpdateAvailable( stackedTileId );
    } else {
        d->m_tileCache.remove( stackedTileId );
    }
}

void StackedTileLoader::clear()
{
    mDebug() << "StackedTileLoader::clear()";

    d->m_tilesOnDisplay.clear();
    qDeleteAll( d->m_levelZeroHash );
    d->m_levelZeroHash.clear();
    d->m_tileCache.clear();

    for ( int row = 0; row < tileRowCount( 0 ); ++row ) {
        for ( int column = 0; column < tileColumnCount( 0 ); ++column ) {
            TileId const id = TileId( 0, 0, column, row );
            const StackedTile *const levelZeroTile = d->createTile( id );
            d->m_levelZeroHash.insert( id, levelZeroTile );
        }
    }
}

// 
QVector<GeoSceneTexture const *>
StackedTileLoaderPrivate::findRelevantTextureLayers( TileId const & stackedTileId ) const
{
    QVector<GeoSceneTexture const *> result;
    QVector<GeoSceneTexture const *>::const_iterator pos = m_textureLayers.constBegin();
    QVector<GeoSceneTexture const *>::const_iterator const end = m_textureLayers.constEnd();
    for (; pos != end; ++pos ) {
        GeoSceneTexture const * const candidate = dynamic_cast<GeoSceneTexture const *>( *pos );
        // check if layer is enabled. A layer is considered to be enabled if one of the
        // following conditions is true:
        // 1) it is the first layer
        // 2) there are no settings available (group "Texture Layers" not defined in DGML)
        // 3) the layer is configured and enabled in the settings
        // also check, if layer provides tiles for the current level
        if ( candidate
             && ( !candidate->hasMaximumTileLevel()
                  || stackedTileId.zoomLevel() <= candidate->maximumTileLevel() ))
            result.append( candidate );
    }
    return result;
}

}

#include "StackedTileLoader.moc"
