//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// local
#include"TileScalingTextureMapper.h"

// posix
#include <cmath>

// Qt
#include <QtCore/qmath.h>
#include <QtGui/QImage>
#include <QtGui/QPainter>

// Marble
#include "GeoPainter.h"
#include "StackedTileLoader.h"
#include "ViewParams.h"
#include "TextureColorizer.h"
#include "TileLoaderHelper.h"
#include "StackedTile.h"
#include "MathHelper.h"

using namespace Marble;

TileScalingTextureMapper::TileScalingTextureMapper( StackedTileLoader *tileLoader,
                                                    QCache<TileId, QPixmap> *cache,
                                                    QObject *parent )
    : AbstractScanlineTextureMapper( tileLoader, parent ),
      m_cache( cache ),
      m_repaintNeeded( true ),
      m_oldRadius( 0 )
{
    connect( m_tileLoader, SIGNAL( tileUpdateAvailable( const TileId & ) ),
             this, SLOT( updateTile( const TileId & ) ) );
    connect( m_tileLoader, SIGNAL( tileUpdatesAvailable() ),
             this, SLOT( updateTiles() ) );
}

void TileScalingTextureMapper::mapTexture( GeoPainter *painter,
                                           ViewParams *viewParams,
                                           const QRect &dirtyRect,
                                           TextureColorizer *texColorizer )
{
    if ( texColorizer || m_oldRadius != viewParams->radius() ) {
        if ( m_repaintNeeded ) {
            mapTexture( painter, viewParams, texColorizer );

            m_repaintNeeded = false;
        }

        painter->drawImage( dirtyRect, *viewParams->canvasImage(), dirtyRect );
    } else {
        mapTexture( painter, viewParams, texColorizer );
    }
}

void TileScalingTextureMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void TileScalingTextureMapper::mapTexture( GeoPainter *geoPainter, ViewParams *viewParams, TextureColorizer *texColorizer )
{
    if ( viewParams->radius() <= 0 )
        return;

    QSharedPointer<QImage> canvasImage = viewParams->canvasImagePtr();
    const int imageHeight = canvasImage->height();
    const int imageWidth  = canvasImage->width();
    const qint64  radius      = viewParams->radius();

    const bool highQuality  = ( viewParams->mapQuality() == HighQuality
                || viewParams->mapQuality() == PrintQuality );

    // Reset backend
    m_tileLoader->resetTilehash();
    setRadius( viewParams->radius() );

    // Calculate translation of center point
    qreal centerLon, centerLat;

    viewParams->centerCoordinates( centerLon, centerLat );

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

    if ( texColorizer || m_oldRadius != radius ) {
        m_cache->clear();

        QPainter painter( canvasImage.data() );
        painter.setRenderHint( QPainter::SmoothPixmapTransform, highQuality );

        for ( int tileY = minTileY; tileY <= maxTileY; ++tileY ) {
            for ( int tileX = minTileX; tileX <= maxTileX; ++tileX ) {
                const qreal xLeft   = ( 4.0 * radius ) * ( ( tileX     ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal xRight  = ( 4.0 * radius ) * ( ( tileX + 1 ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal yTop    = ( 4.0 * radius ) * ( ( tileY     ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );
                const qreal yBottom = ( 4.0 * radius ) * ( ( tileY + 1 ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );

                const QRectF rect = QRectF( QPointF( xLeft, yTop ), QPointF( xRight, yBottom ) );
                const TileId stackedId = TileId( 0, tileZoomLevel(), ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );
                StackedTile *const tile = m_tileLoader->loadTile( stackedId );
                tile->setUsed( true );

                painter.drawImage( rect, *tile->resultTile() );
            }
        }

        if ( texColorizer ) {
            texColorizer->colorize( viewParams );
        }
    } else {
        QPainter painter( geoPainter->device() );
        painter.setRenderHint( QPainter::SmoothPixmapTransform, highQuality );

        for ( int tileY = minTileY; tileY <= maxTileY; ++tileY ) {
            for ( int tileX = minTileX; tileX <= maxTileX; ++tileX ) {
                const qreal xLeft   = ( 4.0 * radius ) * ( ( tileX     ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal xRight  = ( 4.0 * radius ) * ( ( tileX + 1 ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
                const qreal yTop    = ( 4.0 * radius ) * ( ( tileY     ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );
                const qreal yBottom = ( 4.0 * radius ) * ( ( tileY + 1 ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );

                const QRectF rect = QRectF( QPointF( xLeft, yTop ), QPointF( xRight, yBottom ) );
                const TileId stackedId = TileId( 0, tileZoomLevel(), ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );
                StackedTile *const tile = m_tileLoader->loadTile( stackedId );
                tile->setUsed( true );

                const QSize size = QSize( qRound( rect.right() - rect.left() ), qRound( rect.bottom() - rect.top() ) );
                const int cacheHash = 2 * ( size.width() % 2 ) + ( size.height() % 2 );
                const TileId cacheId = TileId( cacheHash, stackedId.zoomLevel(), stackedId.x(), stackedId.y() );
                QPixmap *im = (*m_cache)[cacheId];
                if ( im == 0 ) {
                    im = new QPixmap( QPixmap::fromImage( tile->resultTile()->scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) ) );
                    m_cache->insert( cacheId, im );
                }

                painter.drawPixmap( rect.topLeft(), *im );
            }
        }
    }

    m_tileLoader->cleanupTilehash();

    m_oldRadius = radius;
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
