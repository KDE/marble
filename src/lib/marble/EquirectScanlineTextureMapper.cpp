//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// local
#include"EquirectScanlineTextureMapper.h"

// posix
#include <cmath>

// Qt
#include <QRunnable>

// Marble
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "ScanlineTextureMapperContext.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "ViewportParams.h"

using namespace Marble;

class EquirectScanlineTextureMapper::RenderJob : public QRunnable
{
public:
    RenderJob( StackedTileLoader *tileLoader, int tileLevel, QImage *canvasImage, const ViewportParams *viewportParams, MapQuality mapQuality, int yTop, int yBottom );

    virtual void run();

private:
    StackedTileLoader *const m_tileLoader;
    const int m_tileLevel;
    QImage *const m_canvasImage;
    const ViewportParams *const m_viewport;
    const MapQuality m_mapQuality;
    const int m_yPaintedTop;
    const int m_yPaintedBottom;
};

EquirectScanlineTextureMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, QImage *canvasImage, const ViewportParams *viewport, MapQuality mapQuality, int yTop, int yBottom )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_canvasImage( canvasImage ),
      m_viewport( viewport ),
      m_mapQuality( mapQuality ),
      m_yPaintedTop( yTop ),
      m_yPaintedBottom( yBottom )
{
}


EquirectScanlineTextureMapper::EquirectScanlineTextureMapper( StackedTileLoader *tileLoader )
    : TextureMapperInterface(),
      m_tileLoader( tileLoader ),
      m_radius( 0 ),
      m_oldYPaintedTop( 0 )
{
}

void EquirectScanlineTextureMapper::mapTexture( GeoPainter *painter,
                                                const ViewportParams *viewport,
                                                int tileZoomLevel,
                                                const QRect &dirtyRect,
                                                TextureColorizer *texColorizer )
{
    if ( m_canvasImage.size() != viewport->size() || m_radius != viewport->radius() ) {
        const QImage::Format optimalFormat = ScanlineTextureMapperContext::optimalCanvasImageFormat( viewport );

        if ( m_canvasImage.size() != viewport->size() || m_canvasImage.format() != optimalFormat ) {
            m_canvasImage = QImage( viewport->size(), optimalFormat );
        }

        if ( !viewport->mapCoversViewport() ) {
            m_canvasImage.fill( 0 );
        }

        m_radius = viewport->radius();
        m_repaintNeeded = true;
    }

    if ( m_repaintNeeded ) {
        mapTexture( viewport, tileZoomLevel, painter->mapQuality() );

        if ( texColorizer ) {
            texColorizer->colorize( &m_canvasImage, viewport, painter->mapQuality() );
        }

        m_repaintNeeded = false;
    }

    painter->drawImage( dirtyRect, m_canvasImage, dirtyRect );
}

void EquirectScanlineTextureMapper::mapTexture( const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality )
{
    // Reset backend
    m_tileLoader->resetTilehash();

    // Initialize needed constants:

    const int imageHeight = m_canvasImage.height();
    const qint64  radius      = viewport->radius();
    // Calculate how many degrees are being represented per pixel.
    const float rad2Pixel = (float)( 2 * radius ) / M_PI;

    // Calculate translation of center point
    const qreal centerLat = viewport->centerLatitude();

    int yCenterOffset = (int)( centerLat * rad2Pixel );

    // Calculate y-range the represented by the center point, yTop and
    // what actually can be painted
    const int yTop     = imageHeight / 2 - radius + yCenterOffset;;
    int yPaintedTop    = imageHeight / 2 - radius + yCenterOffset;
    int yPaintedBottom = imageHeight / 2 + radius + yCenterOffset;
 
    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > imageHeight)    yPaintedTop = imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > imageHeight) yPaintedBottom = imageHeight;

    const int numThreads = m_threadPool.maxThreadCount();
    const int yStep = ( yPaintedBottom - yPaintedTop ) / numThreads;
    for ( int i = 0; i < numThreads; ++i ) {
        const int yStart = yPaintedTop +  i      * yStep;
        const int yEnd   = yPaintedTop + (i + 1) * yStep;
        QRunnable *const job = new RenderJob( m_tileLoader, tileZoomLevel, &m_canvasImage, viewport, mapQuality, yStart, yEnd );
        m_threadPool.start( job );
    }

    // Remove unused lines
    const int clearStart = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? yPaintedBottom : 0;
    const int clearStop  = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? imageHeight  : yTop;

    QRgb * const itClearBegin = (QRgb*)( m_canvasImage.scanLine( clearStart ) );
    QRgb * const itClearEnd = (QRgb*)( m_canvasImage.scanLine( clearStop ) );

    for ( QRgb * it = itClearBegin; it < itClearEnd; ++it ) {
        *(it) = 0;
    }

    m_threadPool.waitForDone();

    m_oldYPaintedTop = yPaintedTop;

    m_tileLoader->cleanupTilehash();
}

void EquirectScanlineTextureMapper::RenderJob::run()
{
    // Scanline based algorithm to do texture mapping

    const int imageHeight = m_canvasImage->height();
    const int imageWidth  = m_canvasImage->width();
    const qint64  radius  = m_viewport->radius();
    // Calculate how many degrees are being represented per pixel.
    const qreal rad2Pixel = (qreal)( 2 * radius ) / M_PI;
    const float pixel2Rad = 1.0/rad2Pixel;  // FIXME chainging to qreal may crash Marble when the equator is visible

    const bool interlaced   = ( m_mapQuality == LowQuality );
    const bool highQuality  = ( m_mapQuality == HighQuality
                             || m_mapQuality == PrintQuality );
    const bool printQuality = ( m_mapQuality == PrintQuality );

    // Evaluate the degree of interpolation
    const int n = ScanlineTextureMapperContext::interpolationStep( m_viewport, m_mapQuality );

    // Calculate translation of center point
    const qreal centerLon = m_viewport->centerLongitude();
    const qreal centerLat = m_viewport->centerLatitude();

    const int yCenterOffset = (int)( centerLat * rad2Pixel );

    const int yTop = imageHeight / 2 - radius + yCenterOffset;

    qreal leftLon = + centerLon - ( imageWidth / 2 * pixel2Rad );
    while ( leftLon < -M_PI ) leftLon += 2 * M_PI;
    while ( leftLon >  M_PI ) leftLon -= 2 * M_PI;

    const int maxInterpolationPointX = n * (int)( imageWidth / n - 1 ) + 1;


    // initialize needed variables that are modified during texture mapping:

    ScanlineTextureMapperContext context( m_tileLoader, m_tileLevel );


    // Scanline based algorithm to do texture mapping

    for ( int y = m_yPaintedTop; y < m_yPaintedBottom; ++y ) {

        QRgb * scanLine = (QRgb*)( m_canvasImage->scanLine( y ) );

        qreal lon = leftLon;
        const qreal lat = M_PI/2 - (y - yTop )* pixel2Rad;

        for ( int x = 0; x < imageWidth; ++x ) {

            // Prepare for interpolation
            bool interpolate = false;
            if ( x > 0 && x <= maxInterpolationPointX ) {
                x += n - 1;
                lon += (n - 1) * pixel2Rad;
                interpolate = !printQuality;
            }
            else {
                interpolate = false;
            }

            if ( lon < -M_PI ) lon += 2 * M_PI;
            if ( lon >  M_PI ) lon -= 2 * M_PI;

            if ( interpolate ) {
                if (highQuality)
                    context.pixelValueApproxF( lon, lat, scanLine, n );
                else
                    context.pixelValueApprox( lon, lat, scanLine, n );

                scanLine += ( n - 1 );
            }

            if ( x < imageWidth ) {
                if ( highQuality )
                    context.pixelValueF( lon, lat, scanLine );
                else
                    context.pixelValue( lon, lat, scanLine );
            }

            ++scanLine;
            lon += pixel2Rad;
        }

        // copy scanline to improve performance
        if ( interlaced && y + 1 < m_yPaintedBottom ) { 

            const int pixelByteSize = m_canvasImage->bytesPerLine() / imageWidth;

            memcpy( m_canvasImage->scanLine( y + 1 ),
                    m_canvasImage->scanLine( y     ),
                    imageWidth * pixelByteSize );
            ++y;
        }
    }
}
