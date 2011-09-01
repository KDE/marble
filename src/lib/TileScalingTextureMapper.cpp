//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// local
#include"TileScalingTextureMapper.h"

// posix
#include <cmath>

// Qt
#include <QtCore/qmath.h>
#include <QtGui/QImage>

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

TileScalingTextureMapper::TileScalingTextureMapper( StackedTileLoader *tileLoader,
                                                    QCache<TileId, QPixmap> *cache,
                                                    QObject *parent )
    : TextureMapperInterface( parent ),
      m_tileLoader( tileLoader ),
      m_cache( cache ),
      m_repaintNeeded( true ),
      m_radius( 0 )
{
    connect( m_tileLoader, SIGNAL( tileUpdateAvailable( const TileId & ) ),
             this, SLOT( updateTile( const TileId & ) ) );
    connect( m_tileLoader, SIGNAL( tileUpdatesAvailable() ),
             this, SLOT( updateTiles() ) );
}

void TileScalingTextureMapper::mapTexture( GeoPainter *painter,
                                           const ViewportParams *viewport,
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
            mapTexture( painter, viewport, texColorizer );

            m_radius = viewport->radius();
            m_repaintNeeded = false;
        }

        painter->drawImage( dirtyRect, m_canvasImage, dirtyRect );
    } else {
        mapTexture( painter, viewport, texColorizer );

        m_radius = viewport->radius();
    }
}

void TileScalingTextureMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void TileScalingTextureMapper::mapTexture( GeoPainter *painter, const ViewportParams *viewport, TextureColorizer *texColorizer )
{
    const int imageHeight = viewport->height();
    const int imageWidth  = viewport->width();
    const qint64  radius      = viewport->radius();

    const bool highQuality  = ( painter->mapQuality() == HighQuality
                             || painter->mapQuality() == PrintQuality );

    // Reset backend
    m_tileLoader->resetTilehash();

    // Calculate translation of center point
    qreal centerLon, centerLat;

    viewport->centerCoordinates( centerLon, centerLat );

    const int numTilesX = m_tileLoader->tileRowCount( tileZoomLevel() );
    const int numTilesY = m_tileLoader->tileColumnCount( tileZoomLevel() );
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

    if ( texColorizer || m_radius != radius ) {
        m_cache->clear();

        QPainter imagePainter( &m_canvasImage );
        imagePainter.setRenderHint( QPainter::SmoothPixmapTransform, highQuality );

        for ( int tileY = minTileY; tileY <= maxTileY; ++tileY ) {
            for ( int tileX = minTileX; tileX <= maxTileX; ++tileX ) {
                const qreal xLeft   = ( 4.0 * radius ) * ( ( tileX     ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal xRight  = ( 4.0 * radius ) * ( ( tileX + 1 ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal yTop    = ( 4.0 * radius ) * ( ( tileY     ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );
                const qreal yBottom = ( 4.0 * radius ) * ( ( tileY + 1 ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );

                const QRectF rect = QRectF( QPointF( xLeft, yTop ), QPointF( xRight, yBottom ) );
                const TileId stackedId = TileId( 0, tileZoomLevel(), ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );
                const StackedTile *const tile = m_tileLoader->loadTile( stackedId );

                const QImage *const toScale = tile->resultTile();
                const int deltaLevel = stackedId.zoomLevel() - tile->id().zoomLevel();
                const int restTileX = stackedId.x() % ( 1 << deltaLevel );
                const int restTileY = stackedId.y() % ( 1 << deltaLevel );
                const int partWidth = toScale->width() >> deltaLevel;
                const int partHeight = toScale->height() >> deltaLevel;
                const int startX = restTileX * partWidth;
                const int startY = restTileY * partHeight;
                QImage const part = toScale->copy( startX, startY, partWidth, partHeight ).scaled( toScale->size() );

                imagePainter.drawImage( rect, part );
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
                const TileId stackedId = TileId( 0, tileZoomLevel(), ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );
                const StackedTile *const tile = m_tileLoader->loadTile( stackedId );

                const QSize size = QSize( qRound( rect.right() - rect.left() ), qRound( rect.bottom() - rect.top() ) );
                const int cacheHash = 2 * ( size.width() % 2 ) + ( size.height() % 2 );
                const TileId cacheId = TileId( cacheHash, stackedId.zoomLevel(), stackedId.x(), stackedId.y() );

                QPixmap *im_cached = (*m_cache)[cacheId];
                QPixmap *im = im_cached;
                if ( im == 0 ) {
                    const QImage *const toScale = tile->resultTile();
                    const int deltaLevel = stackedId.zoomLevel() - tile->id().zoomLevel();
                    const int restTileX = stackedId.x() % ( 1 << deltaLevel );
                    const int restTileY = stackedId.y() % ( 1 << deltaLevel );
                    const int partWidth = toScale->width() >> deltaLevel;
                    const int partHeight = toScale->height() >> deltaLevel;
                    const int startX = restTileX * partWidth;
                    const int startY = restTileY * partHeight;
                    QImage const part = toScale->copy( startX, startY, partWidth, partHeight ).scaled( toScale->size() );

                    im = new QPixmap( QPixmap::fromImage( part.scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) ) );
                }
                painter->drawPixmap( rect.topLeft(), *im );

                if (im != im_cached)
                    m_cache->insert( cacheId, im );
            }
        }

        painter->restore();
    }

    m_tileLoader->cleanupTilehash();
}

void TileScalingTextureMapper::updateTile( const TileId &stackedId )
{
    for ( int i = 0; i < 4; ++i ) {
        const TileId id = TileId( i, stackedId.zoomLevel(), stackedId.x(), stackedId.y() );

        m_cache->remove( id );
    }

    emit tileUpdatesAvailable();
}

void TileScalingTextureMapper::updateTiles()
{
    m_cache->clear();

    emit tileUpdatesAvailable();
}

#include "TileScalingTextureMapper.moc"
