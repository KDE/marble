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

// Qt
#include <QtGui/QImage>

// Marble
#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "AbstractProjection.h"
#include "MathHelper.h"

using namespace Marble;

MercatorScanlineTextureMapper::MercatorScanlineTextureMapper( StackedTileLoader *tileLoader,
                                                              QObject *parent )
    : AbstractScanlineTextureMapper( tileLoader, parent ),
      m_repaintNeeded( true ),
      m_oldCenterLon( 0.0 ),
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
        mapTexture( viewParams, texColorizer );

        m_repaintNeeded = false;
    }

    painter->drawImage( dirtyRect, *viewParams->canvasImage(), dirtyRect );
}

void MercatorScanlineTextureMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void MercatorScanlineTextureMapper::mapTexture( ViewParams *viewParams, TextureColorizer *texColorizer )
{
    QImage       *canvasImage = viewParams->canvasImage();
    const int imageHeight = canvasImage->height();
    const int imageWidth  = canvasImage->width();
    const qint64  radius      = viewParams->radius();

    const bool highQuality  = ( viewParams->mapQuality() == HighQuality
                || viewParams->mapQuality() == PrintQuality );
    const bool printQuality = ( viewParams->mapQuality() == PrintQuality );

    //mDebug() << "m_maxGlobalX: " << m_maxGlobalX;
    //mDebug() << "radius      : " << radius << endl;
    // Scanline based algorithm to do texture mapping

    // Initialize needed variables:
    qreal  lon = 0.0;
    qreal  lat = 0.0;

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    // Calculate how many degrees are being represented per pixel.
    const float rad2Pixel = (float)( 2 * radius ) / M_PI;

    // Reset backend
    m_tileLoader->resetTilehash();
    setRadius( viewParams->radius() );

    // Evaluate the degree of interpolation
    const int n = interpolationStep( viewParams );
    bool interpolate = false;

    bool interlaced = ( m_interlaced 
            || viewParams->mapQuality() == LowQuality );

    // Calculate translation of center point
    qreal centerLon, centerLat;

    viewParams->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );

    int yTop;
    int yPaintedTop;
    int yPaintedBottom;

    // Calculate y-range the represented by the center point, yTop and
    // what actually can be painted
    yPaintedTop    = yTop = imageHeight / 2 - 2 * radius + yCenterOffset;
    yPaintedBottom        = imageHeight / 2 + 2 * radius + yCenterOffset;
 
    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > imageHeight)    yPaintedTop = imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > imageHeight) yPaintedBottom = imageHeight;

    const qreal pixel2Rad = 1.0/rad2Pixel;

    qreal leftLon = + centerLon - ( imageWidth / 2 * pixel2Rad );
    while ( leftLon < -M_PI ) leftLon += 2 * M_PI;
    while ( leftLon >  M_PI ) leftLon -= 2 * M_PI;

    // Paint the map.
    for ( int y = yPaintedTop; y < yPaintedBottom; ++y ) {

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

        const int xLeft  = 0; 
        const int xRight = canvasImage->width();

        QRgb * scanLine = (QRgb*)( canvasImage->scanLine( y ) ) + xLeft;

        int  xIpLeft  = 1;
        int  xIpRight = n * (int)( xRight / n - 1 ) + 1; 

        lon = leftLon;
        lat = atan( sinh( ( (imageHeight / 2 + yCenterOffset) - y )
                    * pixel2Rad ) );

        for ( int x = xLeft; x < xRight; ++x ) {

            // Prepare for interpolation
            if ( x >= xIpLeft && x <= xIpRight ) {
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
                    pixelValueApproxF( lon, lat, scanLine, n );
                else
                    pixelValueApprox( lon, lat, scanLine, n );

                scanLine += ( n - 1 );
            }

            if ( x < imageWidth ) {
                if ( highQuality )
                    pixelValueF( lon, lat, scanLine );
                else
                    pixelValue( lon, lat, scanLine );
            }
            m_prevLon = lon;
            m_prevLat = lat; // preparing for interpolation

            ++scanLine;
            lon += pixel2Rad;
        }

        // copy scanline to improve performance
        if ( interlaced && y + 1 < yPaintedBottom ) { 

            int pixelByteSize = canvasImage->bytesPerLine() / imageWidth;

            memcpy( canvasImage->scanLine( y + 1 ) + xLeft * pixelByteSize, 
                    canvasImage->scanLine( y ) + xLeft * pixelByteSize, 
                    ( xRight - xLeft ) * pixelByteSize );
            ++y;
        }
    }

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

    if ( texColorizer ) {
        texColorizer->colorize( viewParams );
    }
}


#include "MercatorScanlineTextureMapper.moc"
