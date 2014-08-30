//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2008      Inge Wallin      <inge@lysator.liu.se>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


// local
#include "GnomonicScanlineTextureMapper.h"

// Qt
#include <QtCore/qmath.h>
#include <QtCore/QRunnable>
#include <QtGui/QImage>

// Marble
#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "ScanlineTextureMapperContext.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "MathHelper.h"

using namespace Marble;

class GnomonicScanlineTextureMapper::RenderJob : public QRunnable
{
public:
    RenderJob( StackedTileLoader *tileLoader, int tileLevel, QImage *canvasImage, const ViewportParams *viewport, MapQuality mapQuality, int yTop, int yBottom );

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

GnomonicScanlineTextureMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, QImage *canvasImage, const ViewportParams *viewport, MapQuality mapQuality, int yTop, int yBottom )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_canvasImage( canvasImage ),
      m_viewport( viewport ),
      m_mapQuality( mapQuality ),
      m_yPaintedTop( yTop ),
      m_yPaintedBottom( yBottom )
{
}


GnomonicScanlineTextureMapper::GnomonicScanlineTextureMapper( StackedTileLoader *tileLoader )
    : TextureMapperInterface()
    , m_tileLoader( tileLoader )
    , m_radius( 0 )
{
}


void GnomonicScanlineTextureMapper::mapTexture( GeoPainter *painter,
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

void GnomonicScanlineTextureMapper::mapTexture( const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality )
{
    // Reset backend
    m_tileLoader->resetTilehash();

    const int imageHeight = viewport->height();

    const int numThreads = m_threadPool.maxThreadCount();
    const int yStep = imageHeight / numThreads;
    for ( int i = 0; i < numThreads; ++i ) {
        const int yStart =  i      * yStep;
        const int yEnd   = (i + 1) * yStep;
        QRunnable *const job = new RenderJob( m_tileLoader, tileZoomLevel, &m_canvasImage, viewport, mapQuality, yStart, yEnd );
        m_threadPool.start( job );
    }

    m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();
}

void GnomonicScanlineTextureMapper::RenderJob::run()
{
    const int imageWidth  = m_canvasImage->width();
    const qint64  radius  = m_viewport->radius();
    // Calculate how many degrees are being represented per pixel.
    const qreal rad2Pixel = ( 2 * radius ) / M_PI;
    const qreal pixel2Rad = 1.0/rad2Pixel;

    const bool interlaced   = ( m_mapQuality == LowQuality );
    const bool highQuality  = ( m_mapQuality == HighQuality
                             || m_mapQuality == PrintQuality );
    const bool printQuality = ( m_mapQuality == PrintQuality );

    // Evaluate the degree of interpolation
    const int n = ScanlineTextureMapperContext::interpolationStep( m_viewport, m_mapQuality );
    const int maxInterpolationPointX = n * (int)( imageWidth / n - 1 ) + 1;

    // initialize needed variables that are modified during texture mapping:

    ScanlineTextureMapperContext context( m_tileLoader, m_tileLevel );


    // Paint the map.
    for ( int y = m_yPaintedTop; y < m_yPaintedBottom; ++y ) {

        QRgb * scanLine = (QRgb*)m_canvasImage->scanLine( y );

        for ( int x = 0; x < imageWidth; ++x ) {

            // Prepare for interpolation
            bool interpolate = false;
            if ( x >= 1 && x <= maxInterpolationPointX ) {
                x += n - 1;
                interpolate = !printQuality;
            }
            else {
                interpolate = false;
            }

            qreal lon;
            qreal lat;
            m_viewport->geoCoordinates(x,y, lon, lat, GeoDataCoordinates::Radian);

            if ( interpolate ) {
                if ( highQuality )
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
