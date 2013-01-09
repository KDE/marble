//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn     <tackat@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "SphericalScanlineTextureMapper.h"

#include <cmath>

#include <QtCore/QRunnable>

#include "MarbleGlobal.h"
#include "GeoPainter.h"
#include "GeoDataPolygon.h"
#include "GeoDataDocument.h"
#include "MarbleDebug.h"
#include "Quaternion.h"
#include "ScanlineTextureMapperContext.h"
#include "StackedTileLoader.h"
#include "StackedTile.h"
#include "TextureColorizer.h"
#include "ViewportParams.h"
#include "MathHelper.h"


using namespace Marble;

class SphericalScanlineTextureMapper::RenderJob : public QRunnable
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
    int const m_yTop;
    int const m_yBottom;
};

SphericalScanlineTextureMapper::RenderJob::RenderJob( StackedTileLoader *tileLoader, int tileLevel, QImage *canvasImage, const ViewportParams *viewport, MapQuality mapQuality, int yTop, int yBottom )
    : m_tileLoader( tileLoader ),
      m_tileLevel( tileLevel ),
      m_canvasImage( canvasImage ),
      m_viewport( viewport ),
      m_mapQuality( mapQuality ),
      m_yTop( yTop ),
      m_yBottom( yBottom )
{
}

SphericalScanlineTextureMapper::SphericalScanlineTextureMapper( StackedTileLoader *tileLoader )
    : TextureMapperInterface()
    , m_tileLoader( tileLoader )
    , m_repaintNeeded( true )
    , m_radius( 0 )
    , m_threadPool()
{
}

void SphericalScanlineTextureMapper::mapTexture( GeoPainter *painter,
                                                 const ViewportParams *viewport,
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
        mapTexture( viewport, painter->mapQuality() );

        if ( texColorizer ) {
            texColorizer->colorize( &m_canvasImage, viewport, painter->mapQuality() );
        }

        m_repaintNeeded = false;
    }

    const int radius = (int)(1.05 * (qreal)(viewport->radius()));

    QRect rect( viewport->width() / 2 - radius, viewport->height() / 2 - radius,
                2 * radius, 2 * radius);
    rect = rect.intersect( dirtyRect );
    painter->drawImage( rect, m_canvasImage, rect );
}

void SphericalScanlineTextureMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void SphericalScanlineTextureMapper::mapTexture( const ViewportParams *viewport, MapQuality mapQuality )
{
    // Reset backend
    m_tileLoader->resetTilehash();

    // Initialize needed constants:

    const int imageHeight = m_canvasImage.height();
    const qint64  radius      = viewport->radius();

    // Calculate the actual y-range of the map on the screen 
    const int skip = ( mapQuality == LowQuality ) ? 1 : 0;
    const int yTop = ( ( imageHeight / 2 - radius < 0 )
                       ? 0 : imageHeight / 2 - radius );
    const int yBottom = ( (yTop == 0)
                          ? imageHeight - skip
                          : yTop + radius + radius - skip );

    const int numThreads = m_threadPool.maxThreadCount();
    const int yStep = ( yBottom - yTop ) / numThreads;
    for ( int i = 0; i < numThreads; ++i ) {
        const int yStart = yTop +  i      * yStep;
        const int yEnd   = yTop + (i + 1) * yStep;
        QRunnable *const job = new RenderJob( m_tileLoader, tileZoomLevel(), &m_canvasImage, viewport, mapQuality, yStart, yEnd );
        m_threadPool.start( job );
    }

    m_threadPool.waitForDone();

    m_tileLoader->cleanupTilehash();
}

void SphericalScanlineTextureMapper::RenderJob::run()
{
    const int imageHeight = m_canvasImage->height();
    const int imageWidth  = m_canvasImage->width();
    const qint64  radius  = m_viewport->radius();
    const qreal  inverseRadius = 1.0 / (qreal)(radius);

    const bool interlaced   = ( m_mapQuality == LowQuality );
    const bool highQuality  = ( m_mapQuality == HighQuality
                             || m_mapQuality == PrintQuality );
    const bool printQuality = ( m_mapQuality == PrintQuality );

    // Evaluate the degree of interpolation
    const int n = ScanlineTextureMapperContext::interpolationStep( m_viewport, m_mapQuality );

    // Calculate north pole position to decrease pole distortion later on
    Quaternion northPole = Quaternion::fromSpherical( 0.0, M_PI * 0.5 );
    northPole.rotateAroundAxis( m_viewport->planetAxis().inverse() );
    const int northPoleX = imageWidth / 2 + (int)( radius * northPole.v[Q_X] );
    const int northPoleY = imageHeight / 2 - (int)( radius * northPole.v[Q_Y] );

    // Calculate axis matrix to represent the planet's rotation.
    matrix  planetAxisMatrix;
    m_viewport->planetAxis().toMatrix( planetAxisMatrix );

    // initialize needed variables that are modified during texture mapping:

    ScanlineTextureMapperContext context( m_tileLoader, m_tileLevel );
    qreal  lon = 0.0;
    qreal  lat = 0.0;

    // Scanline based algorithm to texture map a sphere
    for ( int y = m_yTop; y < m_yBottom ; ++y ) {

        // Evaluate coordinates for the 3D position vector of the current pixel
        const qreal qy = inverseRadius * (qreal)( imageHeight / 2 - y );
        const qreal qr = 1.0 - qy * qy;

        // rx is the radius component in x direction
        const int rx = (int)sqrt( (qreal)( radius * radius
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

        const int xLeft  = ( ( imageWidth / 2 - rx > 0 )
                             ? imageWidth / 2 - rx : 0 ); 
        const int xRight = ( ( imageWidth / 2 - rx > 0 )
                             ? xLeft + rx + rx : imageWidth );

        QRgb * scanLine = (QRgb*)( m_canvasImage->scanLine( y ) ) + xLeft;

        const int xIpLeft  = ( imageWidth / 2 - rx > 0 ) ? n * (int)( xLeft / n + 1 )
                                                         : 1;
        const int xIpRight = ( imageWidth / 2 - rx > 0 ) ? n * (int)( xRight / n - 1 )
                                                         : n * (int)( xRight / n - 1 ) + 1; 

        // Decrease pole distortion due to linear approximation ( y-axis )
        bool crossingPoleArea = false;
        if ( northPole.v[Q_Z] > 0
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
//                mDebug() << QString("NorthPole X: %1, LeftInterval: %2").arg( northPoleX ).arg( leftInterval );
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

            // Evaluate more coordinates for the 3D position vector of
            // the current pixel.
            const qreal qx = (qreal)( x - imageWidth / 2 ) * inverseRadius;
            const qreal qr2z = qr - qx * qx;
            const qreal qz = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;

            // Create Quaternion from vector coordinates and rotate it
            // around globe axis
            Quaternion qpos( 0.0, qx, qy, qz );
            qpos.rotateAroundAxis( planetAxisMatrix );

            qpos.getSpherical( lon, lat );
//            mDebug() << QString("lon: %1 lat: %2").arg(lon).arg(lat);
            // Approx for n-1 out of n pixels within the boundary of
            // xIpLeft to xIpRight

            if ( interpolate ) {
                if (highQuality)
                    context.pixelValueApproxF( lon, lat, scanLine, n );
                else
                    context.pixelValueApprox( lon, lat, scanLine, n );

                scanLine += ( n - 1 );
            }

//          Comment out the pixelValue line and run Marble if you want
//          to understand the interpolation:

//          Uncomment the crossingPoleArea line to check precise 
//          rendering around north pole:

//            if ( !crossingPoleArea )
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
                    m_canvasImage->scanLine( y ) + xLeft * pixelByteSize, 
                    ( xRight - xLeft ) * pixelByteSize );
            ++y;
        }
    }
}
