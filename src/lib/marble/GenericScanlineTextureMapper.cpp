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
#include "GenericScanlineTextureMapper.h"

// Qt
#include <qmath.h>
#include <QRunnable>
#include <QImage>

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
#include "AbstractProjection.h"

using namespace Marble;

class GenericScanlineTextureMapper::RenderJob : public QRunnable
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
    const int m_yTop;
    const int m_yBottom;
};

GenericScanlineTextureMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, QImage *canvasImage, const ViewportParams *viewport, MapQuality mapQuality, int yTop, int yBottom )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_canvasImage( canvasImage ),
      m_viewport( viewport ),
      m_mapQuality( mapQuality ),
      m_yTop( yTop ),
      m_yBottom( yBottom )
{
}


GenericScanlineTextureMapper::GenericScanlineTextureMapper( StackedTileLoader *tileLoader )
    : TextureMapperInterface()
    , m_tileLoader( tileLoader )
    , m_radius( 0 )
    , m_threadPool()
{
}

void GenericScanlineTextureMapper::mapTexture( GeoPainter *painter,
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

    const int radius = viewport->radius() * viewport->currentProjection()->clippingRadius();

    QRect rect( viewport->width() / 2 - radius, viewport->height() / 2 - radius,
                2 * radius, 2 * radius);
    rect = rect.intersected( dirtyRect );
    painter->drawImage( rect, m_canvasImage, rect );
}

void GenericScanlineTextureMapper::mapTexture( const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality )
{
    // Reset backend
    m_tileLoader->resetTilehash();

    const int imageHeight = viewport->height();
    const qint64  radius      = viewport->radius() * viewport->currentProjection()->clippingRadius();

    // Calculate the actual y-range of the map on the screen
    const int skip = ( mapQuality == LowQuality ) ? 1
                                                  : 0;
    const int yTop = ( imageHeight / 2 - radius >= 0 ) ? imageHeight / 2 - radius
                                                       : 0;
    const int yBottom = ( yTop == 0 ) ? imageHeight - skip
                                      : yTop + radius + radius - skip;

    const int numThreads = m_threadPool.maxThreadCount();
    const int yStep = qCeil(qreal( yBottom - yTop ) / qreal(numThreads));
    for ( int i = 0; i < numThreads; ++i ) {
        const int yStart = yTop +  i      * yStep;
        const int yEnd   = qMin(yBottom, yTop + (i + 1) * yStep);
        QRunnable *const job = new RenderJob( m_tileLoader, tileZoomLevel, &m_canvasImage, viewport, mapQuality, yStart, yEnd );
        m_threadPool.start( job );
    }

    m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();
}

void GenericScanlineTextureMapper::RenderJob::run()
{
    const int imageWidth  = m_canvasImage->width();
    const int imageHeight  = m_canvasImage->height();
    const qint64  radius  = m_viewport->radius();

    const bool interlaced   = ( m_mapQuality == LowQuality );
    const bool highQuality  = ( m_mapQuality == HighQuality
                             || m_mapQuality == PrintQuality );
    const bool printQuality = ( m_mapQuality == PrintQuality );

    // Evaluate the degree of interpolation
    const int n = ScanlineTextureMapperContext::interpolationStep( m_viewport, m_mapQuality );

    // Calculate north pole position to decrease pole distortion later on
    qreal northPoleX, northPoleY;
    bool globeHidesNorthPole;
    GeoDataCoordinates northPole(0, m_viewport->currentProjection()->maxLat(), 0);
    m_viewport->screenCoordinates(northPole, northPoleX, northPoleY, globeHidesNorthPole );

    // initialize needed variables that are modified during texture mapping:

    ScanlineTextureMapperContext context( m_tileLoader, m_tileLevel );

    qreal clipRadius = radius * m_viewport->currentProjection()->clippingRadius();


    // Paint the map.
    for ( int y = m_yTop; y < m_yBottom; ++y ) {

        // rx is the radius component in x direction
        const int rx = (int)sqrt( (qreal)( clipRadius * clipRadius
                                      - ( ( y - imageHeight / 2 )
                                          * ( y - imageHeight / 2 ) ) ) );

        // Calculate the actual x-range of the map within the current scanline.
        //
        // If the circular border of the earth disk is still visible then xLeft
        // equals the scanline position of the most left pixel that gets covered
        // by the earth disk. In terms of math this equals the half image width minus
        // the radius component on the current scanline in x direction ("rx").
        //
        // If the zoom factor is high enough then the whole screen gets covered
        // by the earth and the border of the earth disk isn't visible anymore.
        // In that situation xLeft equals zero.
        // For xRight the situation is similar.

        const int xLeft  = ( imageWidth / 2 - rx > 0 ) ? imageWidth / 2 - rx
                                                       : 0;
        const int xRight = ( imageWidth / 2 - rx > 0 ) ? xLeft + rx + rx
                                                       : imageWidth;

        QRgb * scanLine = (QRgb*)( m_canvasImage->scanLine( y ) ) + xLeft;

        const int xIpLeft  = ( imageWidth / 2 - rx > 0 ) ? n * (int)( xLeft / n + 1 )
                                                         : 1;
        const int xIpRight = ( imageWidth / 2 - rx > 0 ) ? n * (int)( xRight / n - 1 )
                                                         : n * (int)( xRight / n - 1 ) + 1;

        // Decrease pole distortion due to linear approximation ( y-axis )
        bool crossingPoleArea = false;
        if ( !globeHidesNorthPole
             && northPoleY - ( n * 0.75 ) <= y
             && northPoleY + ( n * 0.75 ) >= y )
        {
            crossingPoleArea = true;
        }

        int ncount = 0;


        for ( int x = xLeft; x < xRight; ++x ) {

            // Prepare for interpolation
            const int leftInterval = xIpLeft + ncount * n;

            bool interpolate = false;

            if ( x >= xIpLeft && x <= xIpRight ) {

                // Decrease pole distortion due to linear approximation ( x-axis )
                if ( crossingPoleArea
                     && northPoleX >= leftInterval + n
                     && northPoleX < leftInterval + 2 * n
                     && x < leftInterval + 3 * n )
                {
                    interpolate = false;
                }
                else {
                    x += n - 1;
                    interpolate = !printQuality;
                    ++ncount;
                }
            }
            else
                interpolate = false;

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
        }

        // copy scanline to improve performance
        if ( interlaced && y + 1 < m_yBottom ) {

            const int pixelByteSize = m_canvasImage->bytesPerLine() / imageWidth;

            memcpy( m_canvasImage->scanLine( y + 1 ) + xLeft * pixelByteSize,
                    m_canvasImage->scanLine( y     ) + xLeft * pixelByteSize,
                    ( xRight - xLeft ) * pixelByteSize );
            ++y;
        }
    }
}
