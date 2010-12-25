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
#include "StackedTileLoader.h"
#include "ViewParams.h"
#include "TileLoaderHelper.h"
#include "StackedTile.h"
#include "MathHelper.h"

using namespace Marble;

TileScalingTextureMapper::TileScalingTextureMapper( StackedTileLoader *tileLoader,
                                                    QObject *parent )
    : AbstractScanlineTextureMapper( tileLoader, parent ),
      m_oldYPaintedTop( 0 )
{
}


void TileScalingTextureMapper::mapTexture( ViewParams *viewParams )
{
    if ( viewParams->radius() <= 0 )
        return;

    QImage       *canvasImage = viewParams->canvasImage();
    const int imageHeight = canvasImage->height();
    const int imageWidth  = canvasImage->width();
    const qint64  radius      = viewParams->radius();

    const bool highQuality  = ( viewParams->mapQuality() == HighQuality
                || viewParams->mapQuality() == PrintQuality );

    m_tilePosX = 65535;
    m_tilePosY = 65535;
    m_toTileCoordinatesLon = (qreal)(globalWidth() / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (qreal)(globalHeight() / 2 - m_tilePosY);

    // Reset backend
    m_tileLoader->resetTilehash();
    selectTileLevel( viewParams );

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
    const int minTileY = qMax( numTilesY * ( yNormalizedCenter - imageHeight/( 8.0 * radius ) ), 0.0 );
    const int maxTileY = qMin( numTilesY * ( yNormalizedCenter + imageHeight/( 8.0 * radius ) ), numTilesY - 1.0 );

    QPainter painter( canvasImage );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, highQuality );

    for ( int tileY = minTileY; tileY <= maxTileY; ++tileY ) {
        for ( int tileX = minTileX; tileX <= maxTileX; ++tileX ) {
            const qreal xLeft   = ( 4.0 * radius ) * ( ( tileX     ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
            const qreal xRight  = ( 4.0 * radius ) * ( ( tileX + 1 ) / (qreal)numTilesX - xNormalizedCenter ) + ( imageWidth / 2.0 );
            const qreal yTop    = ( 4.0 * radius ) * ( ( tileY     ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );
            const qreal yBottom = ( 4.0 * radius ) * ( ( tileY + 1 ) / (qreal)numTilesY - yNormalizedCenter ) + ( imageHeight / 2.0 );

            const QRectF rect = QRectF( QPointF( xLeft, yTop ), QPointF( xRight, yBottom ) );
            const TileId id = TileId( 0, tileZoomLevel(), ( ( tileX % numTilesX ) + numTilesX ) % numTilesX, tileY );
            StackedTile *const tile = m_tileLoader->loadTile( id, DownloadBrowse );
            tile->setUsed( true );

            painter.drawImage( rect, *tile->resultTile() );
        }
    }

    painter.end();

    int yTop;
    int yPaintedTop;
    int yPaintedBottom;

    // Calculate y-range the represented by the center point, yTop and
    // what actually can be painted
    yPaintedTop = yTop = (     - yNormalizedCenter ) * ( 4 * radius ) + imageHeight / 2;
    yPaintedBottom    = ( 1.0 - yNormalizedCenter ) * ( 4 * radius ) + imageHeight / 2;

    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > imageHeight)    yPaintedTop = imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > imageHeight) yPaintedBottom = imageHeight;

    // Remove unused lines
    const int clearStart = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? yPaintedBottom : 0;
    const int clearStop  = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? imageHeight  : yTop;

    QRgb * const clearBegin = (QRgb*)( canvasImage->scanLine( clearStart ) );
    QRgb * const clearEnd = (QRgb*)( canvasImage->scanLine( clearStop ) );

    QRgb * it = clearBegin;

    for ( ; it < clearEnd; ++it ) {
        *(it) = 0;
    }

    m_oldYPaintedTop = yPaintedTop;

    m_tileLoader->cleanupTilehash();
}


#include "TileScalingTextureMapper.moc"
