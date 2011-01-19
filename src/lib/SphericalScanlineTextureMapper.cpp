//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn     <tackat@kde.org>
//


#include "SphericalScanlineTextureMapper.h"

#include <cmath>

#include <QtGui/QImage>

#include "global.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "Quaternion.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "MathHelper.h"


using namespace Marble;

SphericalScanlineTextureMapper::SphericalScanlineTextureMapper( StackedTileLoader *tileLoader,
                                                                QObject *parent )
    : AbstractScanlineTextureMapper( tileLoader, parent )
    , m_repaintNeeded( true )
{
    m_interlaced = false;
}

void SphericalScanlineTextureMapper::mapTexture( GeoPainter *painter,
                                                 ViewParams *viewParams,
                                                 const QRect &dirtyRect,
                                                 TextureColorizer *texColorizer )
{
    if ( m_repaintNeeded ) {
        mapTexture( viewParams, texColorizer );

        m_repaintNeeded = false;
    }

    const int radius = (int)(1.05 * (qreal)(viewParams->radius()));

    QRect rect( viewParams->width() / 2 - radius, viewParams->height() / 2 - radius,
                2 * radius, 2 * radius);
    rect = rect.intersect( dirtyRect );
    painter->drawImage( rect, *viewParams->canvasImage(), rect );
}

void SphericalScanlineTextureMapper::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

void SphericalScanlineTextureMapper::mapTexture( ViewParams *viewParams, TextureColorizer *texColorizer )
{
    QImage       *canvasImage = viewParams->canvasImage();
    const int imageHeight = canvasImage->height();
    const int imageWidth  = canvasImage->width();
    const qint64  radius      = viewParams->radius();

    const bool highQuality  = ( viewParams->mapQuality() == HighQuality
				|| viewParams->mapQuality() == PrintQuality );
    const bool printQuality = ( viewParams->mapQuality() == PrintQuality );

    // Scanline based algorithm to texture map a sphere

    // Initialize needed variables:
    qreal  lon = 0.0;
    qreal  lat = 0.0;

    const qreal  inverseRadius = 1.0 / (qreal)(radius);

    m_tilePosX = 65535;
    m_tilePosY = 65535;
    m_toTileCoordinatesLon = (qreal)(globalWidth() / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (qreal)(globalHeight() / 2 - m_tilePosY);

    // Reset backend
    m_tileLoader->resetTilehash();
    setRadius( viewParams->radius() );

    // Evaluate the degree of interpolation
    const int n = interpolationStep( viewParams );
    bool interpolate = false;

    // Calculate north pole position to decrease pole distortion later on
    Quaternion northPole( 0.0, (qreal)( M_PI * 0.5 ) );

    northPole.rotateAroundAxis( viewParams->planetAxis().inverse() );

    // Calculate axis matrix to represent the planet's rotation.
    matrix  planetAxisMatrix;
    viewParams->planetAxis().toMatrix( planetAxisMatrix );

    bool interlaced = ( m_interlaced 
            || viewParams->mapQuality() == LowQuality );

    int skip = interlaced ? 1 : 0;

    // Calculate the actual y-range of the map on the screen 
    const int yTop = ( ( imageHeight / 2 - radius < 0 )
                       ? 0 : imageHeight / 2 - radius );
    const int yBottom = ( (yTop == 0)
                          ? imageHeight - skip
                          : yTop + radius + radius - skip );

    for ( int y = yTop; y < yBottom ; ++y ) {

        // Evaluate coordinates for the 3D position vector of the current pixel
        const qreal qy = inverseRadius * (qreal)( imageHeight / 2 - y );
        const qreal qr = 1.0 - qy * qy;

        // rx is the radius component in x direction
        int rx = (int)sqrt( (qreal)( radius * radius 
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
                             ? xLeft + rx + rx : canvasImage->width() );

        QRgb * scanLine = (QRgb*)( canvasImage->scanLine( y ) ) + xLeft;

        int  xIpLeft  = 1;
        int  xIpRight = n * (int)( xRight / n - 1 ) + 1; 

        if ( imageWidth / 2 - rx > 0 ) {
            xIpLeft  = n * (int)( xLeft  / n + 1 );
            xIpRight = n * (int)( xRight / n - 1 );
        }

        // Decrease pole distortion due to linear approximation ( y-axis )
        bool crossingPoleArea = false;
        int northPoleY = imageHeight / 2 - (int)( radius * northPole.v[Q_Y] );
        if ( northPole.v[Q_Z] > 0
             && northPoleY - ( n * 0.75 ) <= y
             && northPoleY + ( n * 0.75 ) >= y ) 
        {
            crossingPoleArea = true;
        }

        int ncount = 0;


        for ( int x = xLeft; x < xRight; ++x ) {
            // Prepare for interpolation

            int  leftInterval = xIpLeft + ncount * n;

            if ( x >= xIpLeft && x <= xIpRight ) {

                // Decrease pole distortion due to linear approximation ( x-axis )
                int northPoleX = imageWidth / 2 + (int)( radius * northPole.v[Q_X] );

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
                    pixelValueApproxF( lon, lat, scanLine, n );
                else
                    pixelValueApprox( lon, lat, scanLine, n );

                scanLine += ( n - 1 );
            }

//          Comment out the pixelValue line and run Marble if you want
//          to understand the interpolation:

//          Uncomment the crossingPoleArea line to check precise 
//          rendering around north pole:

//            if ( !crossingPoleArea )
            if ( x < imageWidth ) {
                if ( highQuality )
                    pixelValueF( lon, lat, scanLine );
                else
                    pixelValue( lon, lat, scanLine );
            }
            m_prevLon = lon;
            m_prevLat = lat; // preparing for interpolation

            ++scanLine;
        }

        // copy scanline to improve performance
        if ( interlaced && y + 1 < yBottom ) { 

            int pixelByteSize = canvasImage->bytesPerLine() / imageWidth;

            memcpy( canvasImage->scanLine( y + 1 ) + xLeft * pixelByteSize, 
                    canvasImage->scanLine( y ) + xLeft * pixelByteSize, 
                    ( xRight - xLeft ) * pixelByteSize );
            ++y;
        }
    }

    m_tileLoader->cleanupTilehash();

    if ( texColorizer ) {
        texColorizer->colorize( viewParams );
    }
}


bool SphericalScanlineTextureMapper::needsFilter( const QRgb& rgb, int& oldR, int& oldB, int &oldG  )
{
    int red = qRed( rgb );
    int green = qGreen( rgb );
    int blue = qBlue( rgb );

    bool differs = false;

    if ( abs( red - oldR ) > 0 || abs( blue - oldB ) > 0 || abs( green - oldG ) > 0 ) {
        differs = true;
    }

    oldR = red;
    oldG = green;
    oldB = blue;

    return differs;
}
