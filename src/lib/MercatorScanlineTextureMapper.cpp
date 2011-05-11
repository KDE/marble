//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2008      Inge Wallin      <inge@lysator.liu.se>
//


// local
#include"MercatorScanlineTextureMapper.h"

// posix
#include <cmath>

// Marble
#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "ScanlineTextureMapperContext.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "ViewParams.h"
#include "MathHelper.h"

using namespace Marble;

MercatorScanlineTextureMapper::MercatorScanlineTextureMapper( StackedTileLoader *tileLoader,
                                                              QObject *parent )
    : TextureMapperInterface( parent ),
      m_tileLoader( tileLoader ),
      m_repaintNeeded( true ),
      m_oldYPaintedTop( 0 )
{
    connect( m_tileLoader, SIGNAL( tileUpdateAvailable( const TileId & ) ),
             this, SIGNAL( tileUpdatesAvailable() ) );
    connect( m_tileLoader, SIGNAL( tileUpdatesAvailable() ),
             this, SIGNAL( tileUpdatesAvailable() ) );
}

void MercatorScanlineTextureMapper::mapTexture( GeoPainter *painter,
                                                ViewParams *viewParams,
                                                const QRect &dirtyRect,
                                                TextureColorizer *texColorizer )
{
    if ( m_repaintNeeded ) {
        mapTexture( viewParams );

        if ( texColorizer ) {
            texColorizer->colorize( viewParams );
        }

        m_repaintNeeded = false;
    }

    painter->drawImage( dirtyRect, *viewParams->canvasImage(), dirtyRect );
}

void MercatorScanlineTextureMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void MercatorScanlineTextureMapper::mapTexture( ViewParams *viewParams )
{
    // Reset backend
    m_tileLoader->resetTilehash();

    QSharedPointer<QImage> canvasImage = viewParams->canvasImagePtr();

    // Initialize needed constants:

    const int imageHeight = canvasImage->height();
    const int imageWidth  = canvasImage->width();
    const qint64  radius      = viewParams->radius();
    // Calculate how many degrees are being represented per pixel.
    const float rad2Pixel = (float)( 2 * radius ) / M_PI;
    const qreal pixel2Rad = 1.0/rad2Pixel;

    const bool interlaced   = ( viewParams->mapQuality() == LowQuality );
    const bool highQuality  = ( viewParams->mapQuality() == HighQuality
                             || viewParams->mapQuality() == PrintQuality );
    const bool printQuality = ( viewParams->mapQuality() == PrintQuality );

    // Evaluate the degree of interpolation
    const int n = ScanlineTextureMapperContext::interpolationStep( viewParams );

    //mDebug() << "m_maxGlobalX: " << m_maxGlobalX;
    //mDebug() << "radius      : " << radius << endl;

    // Calculate translation of center point
    qreal centerLon, centerLat;
    viewParams->centerCoordinates( centerLon, centerLat );

    const int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );

    // Calculate y-range the represented by the center point, yTop and
    // what actually can be painted
    const int yTop     = imageHeight / 2 - 2 * radius + yCenterOffset;
    int yPaintedTop    = imageHeight / 2 - 2 * radius + yCenterOffset;
    int yPaintedBottom = imageHeight / 2 + 2 * radius + yCenterOffset;
 
    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > imageHeight)    yPaintedTop = imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > imageHeight) yPaintedBottom = imageHeight;

    qreal leftLon = + centerLon - ( imageWidth / 2 * pixel2Rad );
    while ( leftLon < -M_PI ) leftLon += 2 * M_PI;
    while ( leftLon >  M_PI ) leftLon -= 2 * M_PI;

    const int maxInterpolationPointX = n * (int)( imageWidth / n - 1 ) + 1;


    // initialize needed variables that are modified during texture mapping:

    ScanlineTextureMapperContext context( m_tileLoader, tileZoomLevel() );


    // Scanline based algorithm to do texture mapping

    for ( int y = yPaintedTop; y < yPaintedBottom; ++y ) {

        QRgb * scanLine = (QRgb*)( canvasImage->scanLine( y ) );

        qreal lon = leftLon;
        const qreal lat = atan( sinh( ( (imageHeight / 2 + yCenterOffset) - y )
                    * pixel2Rad ) );

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
        if ( interlaced && y + 1 < yPaintedBottom ) { 

            const int pixelByteSize = canvasImage->bytesPerLine() / imageWidth;

            memcpy( canvasImage->scanLine( y + 1 ),
                    canvasImage->scanLine( y     ),
                    imageWidth * pixelByteSize );
            ++y;
        }
    }

    // Remove unused lines
    const int clearStart = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? yPaintedBottom : 0;
    const int clearStop  = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? imageHeight  : yTop;

    QRgb * const clearBegin = (QRgb*)( canvasImage->scanLine( clearStart ) );
    QRgb * const clearEnd   = (QRgb*)( canvasImage->scanLine( clearStop ) );

    for ( QRgb * it = clearBegin; it < clearEnd; ++it ) {
        *(it) = 0;
    }

    m_oldYPaintedTop = yPaintedTop;

    m_tileLoader->cleanupTilehash();
}


#include "MercatorScanlineTextureMapper.moc"
