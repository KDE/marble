//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// local
#include "TileScalingTextureMapper.h"

// posix
#include <cmath>

// Qt
#include <qmath.h>
#include <QImage>

// Marble
#include "GeoPainter.h"
#include "ScanlineTextureMapperContext.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "TileLoaderHelper.h"
#include "StackedTile.h"
#include "MathHelper.h"
#include "ViewportParams.h"

using namespace Marble;

TileScalingTextureMapper::TileScalingTextureMapper( StackedTileLoader *tileLoader, QObject *parent )
    : QObject( parent ),
      TextureMapperInterface(),
      m_tileLoader( tileLoader ),
      m_cache( 100 ),
      m_radius( 0 )
{
    connect( tileLoader, SIGNAL(tileLoaded(TileId)),
             this,       SLOT(removePixmap(TileId)) );
    connect( tileLoader, SIGNAL(cleared()),
             this,       SLOT(clearPixmaps()) );
}

void TileScalingTextureMapper::mapTexture( GeoPainter *painter,
                                           const ViewportParams *viewport,
                                           int tileZoomLevel,
                                           const QRect &dirtyRect,
                                           TextureColorizer *texColorizer )
{
    if ( viewport->radius() <= 0 )
        return;

    if ( texColorizer || m_radius != viewport->radius() ) {
        if ( m_canvasImage.size() != viewport->size() || m_radius != viewport->radius() ) {
            const QImage::Format optimalFormat = ScanlineTextureMapperContext::optimalCanvasImageFormat( viewport );

            if ( m_canvasImage.size() != viewport->size() || m_canvasImage.format() != optimalFormat ) {
                m_canvasImage = QImage( viewport->size(), optimalFormat );
            }

            if ( !viewport->mapCoversViewport() ) {
                m_canvasImage.fill( 0 );
            }

            m_repaintNeeded = true;
        }

        if ( m_repaintNeeded ) {
            mapTexture( painter, viewport, tileZoomLevel, texColorizer );

            m_radius = viewport->radius();
            m_repaintNeeded = false;
        }

        painter->drawImage( dirtyRect, m_canvasImage, dirtyRect );
    } else {
        mapTexture( painter, viewport, tileZoomLevel, texColorizer );

        m_radius = viewport->radius();
    }
}

void TileScalingTextureMapper::mapTexture( GeoPainter *painter, const ViewportParams *viewport, int tileZoomLevel, TextureColorizer *texColorizer )
{
    const int imageHeight = viewport->height();
    const int imageWidth  = viewport->width();
    const qint64  radius  = viewport->radius();

    const bool highQuality  = ( painter->mapQuality() == HighQuality
                                || painter->mapQuality() == PrintQuality );

    // Reset backend
    m_tileLoader->resetTilehash();

    // Calculate translation of center point
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    const int numTilesX = m_tileLoader->tileRowCount( tileZoomLevel );
    const int numTilesY = m_tileLoader->tileColumnCount( tileZoomLevel );
    Q_ASSERT( numTilesX > 0 );
    Q_ASSERT( numTilesY > 0 );

    const qreal xNormalizedCenter = 0.5 + 0.5 * centerLon / M_PI;
    const int minTileX = qFloor( numTilesX * ( xNormalizedCenter - imageWidth/( 8.0 * radius ) ) );
    const int maxTileX = numTilesX * ( xNormalizedCenter + imageWidth/( 8.0 * radius ) );

    const qreal yNormalizedCenter = 0.5 - 0.5 * asinh( tan( centerLat ) ) / M_PI;
    const int minTileY = qMax( qreal( numTilesY * ( yNormalizedCenter - imageHeight/( 8.0 * radius ) ) ),
                               qreal( 0.0 ) );
    const int maxTileY = qMin( qreal( numTilesY * ( yNormalizedCenter + imageHeight/( 8.0 * radius ) ) ),
                               qreal( numTilesY - 1.0 ) );

    if ( m_radius != radius ) {
        m_cache.clear();
    }

    if ( texColorizer || m_radius != radius ) {
        QPainter imagePainter( &m_canvasImage );
        imagePainter.setRenderHint( QPainter::SmoothPixmapTransform, highQuality );

        for ( int tileY = minTileY; tileY <= maxTileY; ++tileY ) {
            for ( int tileX = minTileX; tileX <= maxTileX; ++tileX ) {
                const qreal xLeft   = ( 4.0 * radius ) * ( ( tileX     ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal xRight  = ( 4.0 * radius ) * ( ( tileX + 1 ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal yTop    = ( 4.0 * radius ) * ( ( tileY     ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );
                const qreal yBottom = ( 4.0 * radius ) * ( ( tileY + 1 ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );

                const QRectF rect = QRectF( QPointF( xLeft, yTop ), QPointF( xRight, yBottom ) );
                const TileId stackedId = TileId( 0, tileZoomLevel, ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );

                const StackedTile *const tile = m_tileLoader->loadTile( stackedId );

                imagePainter.drawImage( rect, *tile->resultImage() );
            }
        }

        if ( texColorizer ) {
            texColorizer->colorize( &m_canvasImage, viewport, painter->mapQuality() );
        }
    } else {
        painter->save();
        painter->setRenderHint( QPainter::SmoothPixmapTransform, highQuality );

        for ( int tileY = minTileY; tileY <= maxTileY; ++tileY ) {
            for ( int tileX = minTileX; tileX <= maxTileX; ++tileX ) {
                const qreal xLeft   = ( 4.0 * radius ) * ( ( tileX     ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal xRight  = ( 4.0 * radius ) * ( ( tileX + 1 ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal yTop    = ( 4.0 * radius ) * ( ( tileY     ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );
                const qreal yBottom = ( 4.0 * radius ) * ( ( tileY + 1 ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );

                const QRectF rect = QRectF( QPointF( xLeft, yTop ), QPointF( xRight, yBottom ) );
                const TileId stackedId = TileId( 0, tileZoomLevel, ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );
                const StackedTile *const tile = m_tileLoader->loadTile( stackedId ); // load tile here for every frame, otherwise cleanupTilehash() clears all visible tiles

                const QSize size = QSize( qCeil( rect.right() - rect.left() ), qCeil( rect.bottom() - rect.top() ) );
                const int cacheHash = 2 * ( size.width() % 2 ) + ( size.height() % 2 );
                const TileId cacheId = TileId( cacheHash, stackedId.zoomLevel(), stackedId.x(), stackedId.y() );

                const QPixmap *const im_cached = m_cache[cacheId];
                const QPixmap *im = im_cached;
                if ( im == 0 ) {
                    im = new QPixmap( QPixmap::fromImage( tile->resultImage()->scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) ) );
                }
                painter->drawPixmap( rect.topLeft(), *im );

                if (im != im_cached)
                    m_cache.insert( cacheId, im );
            }
        }

        painter->restore();
    }

    m_tileLoader->cleanupTilehash();
}

void TileScalingTextureMapper::removePixmap( const TileId &tileId )
{
    const TileId stackedTileId( 0, tileId.zoomLevel(), tileId.x(), tileId.y() );
    for ( int i = 0; i < 4; ++i ) {
        const TileId id = TileId( i, stackedTileId.zoomLevel(), stackedTileId.x(), stackedTileId.y() );

        m_cache.remove( id );
    }
}

void TileScalingTextureMapper::clearPixmaps()
{
    m_cache.clear();
}

#include "moc_TileScalingTextureMapper.cpp"
