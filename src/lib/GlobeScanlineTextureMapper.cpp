//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn     <tackat@kde.org>"
//


#include "GlobeScanlineTextureMapper.h"

#include <cmath>

#include <QtCore/QDebug>

#include "GeoDataPoint.h"
#include "GeoPolygon.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoader.h"


// Defining INTERLACE will make sure that for two subsequent scanlines
// every second scanline will be a deep copy of the first scanline.
// This results in a more coarse resolution and in a speedup for the 
// texture mapping of approx. 25%.

// #define INTERLACE

GlobeScanlineTextureMapper::GlobeScanlineTextureMapper( TileLoader *tileLoader, QObject * parent  ) 
    : AbstractScanlineTextureMapper( tileLoader, parent )
{
    m_interpolate  = false;
    m_nBest = 0;

    m_n = 0;
    m_nInverse = 0.0;

    m_x = 0;
    m_y = 0;

    m_qr = 0.0; 
    m_qx = 0.0;
    m_qy = 0.0;
    m_qz = 0.0;

    m_interlaced = false;
}


GlobeScanlineTextureMapper::~GlobeScanlineTextureMapper()
{
}


void GlobeScanlineTextureMapper::resizeMap(int width, int height)
{
    AbstractScanlineTextureMapper::resizeMap( width, height );

    // Find the optimal interpolation interval m_n for the 
    // current image canvas width
    m_nBest = 2;

    int  nEvalMin = m_imageWidth - 1;
    for ( int it = 1; it < 48; ++it ) {
        int nEval = ( m_imageWidth - 1 ) / it + ( m_imageWidth - 1 ) % it;
        if ( nEval < nEvalMin ) {
            nEvalMin = nEval;
            m_nBest = it; 
        }
    }
    // qDebug() << QString( "Optimized n = %1, remainder: %2" )
    //.arg(m_nBest).arg( ( m_imageWidth ) % m_nBest );
}


void GlobeScanlineTextureMapper::mapTexture( ViewParams *viewParams )
{
    QImage       *canvasImage = viewParams->m_canvasImage;
    const qint64  radius      = viewParams->radius();

    // Scanline based algorithm to texture map a sphere

    // Initialize needed variables:
    double  lon = 0.0;
    double  lat = 0.0;

    const double  inverseRadius = 1.0 / (double)(radius);

    m_tilePosX = 65535;
    m_tilePosY = 65535;
    m_toTileCoordinatesLon = (double)(m_tileLoader->globalWidth( m_tileLevel ) 
                             / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (double)(m_tileLoader->globalHeight( m_tileLevel ) 
                             / 2 - m_tilePosY);

    // Reset backend
    m_tileLoader->resetTilehash();
    selectTileLevel( viewParams );

    // Evaluate the degree of interpolation
    m_n        = ( m_imageRadius < radius * radius ) ? m_nBest : 8;
    m_nInverse = 1.0 / (double)(m_n);

    // Calculate north pole position to decrease pole distortion later on
    Quaternion  northPole = GeoDataPoint( 0.0, (double)( M_PI * 0.5 ) ).quaternion();

    northPole.rotateAroundAxis( viewParams->planetAxis().inverse() );

    // Calculate axis matrix to represent the planet's rotation.
    matrix  planetAxisMatrix;
    viewParams->planetAxis().toMatrix( planetAxisMatrix );

    int skip = ( m_interlaced == true ) ? 1 : 0;

    // Calculate the actual y-range of the map on the screen 
    const int yTop = ( ( m_imageHeight / 2 - radius < 0 )
                       ? 0 : m_imageHeight / 2 - radius );
    const int yBottom = ( (yTop == 0)
                          ? m_imageHeight - skip
                          : yTop + radius + radius - skip );

    bool interlaced = ( m_interlaced ||
                        viewParams->mapQuality() == Marble::Low );

    for ( m_y = yTop; m_y < yBottom ; ++m_y ) {

        // Evaluate coordinates for the 3D position vector of the current pixel
        m_qy = inverseRadius * (double)( m_imageHeight / 2 - m_y );
        m_qr = 1.0 - m_qy * m_qy;

        // rx is the radius component in x direction
        int rx = (int)sqrt( (double)( radius * radius 
                                      - ( ( m_y - m_imageHeight / 2 )
                                          * ( m_y - m_imageHeight / 2 ) ) ) );

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

        const int xLeft  = ( ( m_imageWidth / 2 - rx > 0 )
                             ? m_imageWidth / 2 - rx : 0 ); 
        const int xRight = ( ( m_imageWidth / 2 - rx > 0 )
                             ? xLeft + rx + rx : canvasImage -> width() );

        m_scanLine = (QRgb*)( canvasImage->scanLine( m_y ) ) + xLeft;

        int  xIpLeft  = 1;
        int  xIpRight = m_n * (int)( xRight / m_n - 1) + 1; 

        if ( m_imageWidth / 2 - rx > 0 ) {
            xIpLeft  = m_n * (int)( xLeft  / m_n + 1 );
            xIpRight = m_n * (int)( xRight / m_n - 1 );
        }

        // Decrease pole distortion due to linear approximation ( y-axis )
        bool crossingPoleArea = false;
        int northPoleY = m_imageHeight / 2 - (int)( radius * northPole.v[Q_Y] );
        if ( northPole.v[Q_Z] > 0
             && northPoleY - ( m_n * 0.75 ) <= m_y
             && northPoleY + ( m_n * 0.75 ) >= m_y ) 
        {
            crossingPoleArea = true;
        }

        int ncount = 0;


        for ( int m_x = xLeft; m_x < xRight; ++m_x ) {
            // Prepare for interpolation

            int  leftInterval = xIpLeft + ncount * m_n;

            if ( m_x >= xIpLeft && m_x <= xIpRight ) {

                // Decrease pole distortion due to linear approximation ( x-axis )
                int northPoleX = m_imageWidth / 2 + (int)( radius * northPole.v[Q_X] );

//                qDebug() << QString("NorthPole X: %1, LeftInterval: %2").arg( northPoleX ).arg( leftInterval );
                if ( crossingPoleArea == true
                     && northPoleX >= leftInterval + m_n
                     && northPoleX < leftInterval + 2 * m_n
                     && m_x < leftInterval + 3 * m_n )
                {
                    m_interpolate = false;
                }
                else {
                    m_x += m_n - 1;

                    m_interpolate = true;
                    ++ncount;
                } 
            }
            else
                m_interpolate = false;

            // Evaluate more coordinates for the 3D position vector of
            // the current pixel.
            m_qx = (double)( m_x - m_imageWidth / 2 ) * inverseRadius;

            double qr2z = m_qr - m_qx * m_qx;
            m_qz = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;

            // Create Quaternion from vector coordinates and rotate it
            // around globe axis
            m_qpos.set( 0.0, m_qx, m_qy, m_qz );
            m_qpos.rotateAroundAxis( planetAxisMatrix );

            m_qpos.getSpherical( lon, lat );
//            qDebug() << QString("lon: %1 lat: %2").arg(lon).arg(lat);
            // Approx for m_n-1 out of n pixels within the boundary of
            // xIpLeft to xIpRight

            if ( m_interpolate ) {
                pixelValueApprox( lon, lat, m_scanLine );

                m_scanLine += ( m_n - 1 );
            }

//          Comment out the pixelValue line and run Marble if you want
//          to understand the interpolation:

//          Uncomment the crossingPoleArea line to check precise 
//          rendering around north pole:

//            if ( crossingPoleArea == false )
            if ( m_x < m_imageWidth ) 
                pixelValue( lon, lat, m_scanLine );

            m_prevLon = lon;
            m_prevLat = lat; // preparing for interpolation

            ++m_scanLine;
        }

        if ( interlaced == true ) { // copy scanline to improve performance

            int pixelByteSize = canvasImage->bytesPerLine() / m_imageWidth;

            memcpy( canvasImage->scanLine( m_y + 1 ) + xLeft * pixelByteSize, 
                    canvasImage->scanLine( m_y ) + xLeft * pixelByteSize, 
                    ( xRight - xLeft ) * pixelByteSize );
            ++m_y;
        }
    }

    m_tileLoader->cleanupTilehash();
}


// This method interpolates color values for skipped pixels in a scanline.
// 
// While moving along the scanline we don't move from pixel to pixel but
// leave out m_n pixels each time and calculate the exact position and 
// color value for the new pixel. The pixel values in between get 
// approximated through linear interpolation across the direct connecting 
// line on the original tiles directly.
// This method will do by far most of the calculations for the 
// texturemapping, so we move towards integer math to improve speed.

void GlobeScanlineTextureMapper::pixelValueApprox(const double& lon,
                                                  const double& lat,
                                                  QRgb *scanLine)
{
    // stepLon/Lat: Distance between two subsequent approximated positions

    double stepLat = lat - m_prevLat;
    double stepLon = lon - m_prevLon;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    if ( fabs(stepLon) < M_PI ) {
        const int itStepLon = (int)( stepLon * m_nInverse * m_rad2PixelX * 128.0 );
        const int itStepLat = (int)( stepLat * m_nInverse * m_rad2PixelY * 128.0 );

        m_prevLon *= m_rad2PixelX;
        m_prevLat *= m_rad2PixelY;

        // To improve speed we unroll 
        // AbstractScanlineTextureMapper::pixelValue(...) here and 
        // calculate the performance critical issues via integers

        int itLon = (int)( ( m_prevLon + m_toTileCoordinatesLon ) * 128.0 );
        int itLat = (int)( ( m_prevLat + m_toTileCoordinatesLat ) * 128.0 );

        for ( int j=1; j < m_n; ++j ) {
            m_posX = ( itLon + itStepLon * j ) >> 7;
            m_posY = ( itLat + itStepLat * j ) >> 7;

            if (  m_posX >= m_tileLoader->tileWidth() 
               || m_posX < 0
               || m_posY >= m_tileLoader->tileHeight()
               || m_posY < 0 )
            {
                nextTile();
                itLon = (int)( ( m_prevLon + m_toTileCoordinatesLon ) * 128.0 );
                itLat = (int)( ( m_prevLat + m_toTileCoordinatesLat ) * 128.0 );
                m_posX = ( itLon + itStepLon * j ) >> 7;
                m_posY = ( itLat + itStepLat * j ) >> 7;
            }

            if ( m_tile->depth() == 8 )
                *scanLine = m_tile->jumpTable8[m_posY][m_posX ];
            else
                *scanLine = m_tile->jumpTable32[m_posY][m_posX ];

            ++scanLine;
        }
    }

    // For the case where we cross the dateline between (lon, lat) and 
    // (prevlon, prevlat) we need a more sophisticated calculation.
    // However as this will happen rather rarely, we use 
    // pixelValue(...) directly to make the code more readable.

    else {
        stepLon = ( TWOPI - fabs(stepLon) ) * m_nInverse;
        stepLat = stepLat * m_nInverse;
        // We need to distinguish two cases:  
        // crossing the dateline from east to west ...

        if ( m_prevLon < lon ) {
            for ( int j = 1; j < m_n; ++j ) {
                m_prevLat += stepLat;
                m_prevLon -= stepLon;
                if ( m_prevLon <= -M_PI ) 
                    m_prevLon += TWOPI;
                pixelValue( m_prevLon, m_prevLat, scanLine );
                ++scanLine;
            }
        }

        // ... and vice versa: from west to east.

        else { 
            double curStepLon = lon - m_n * stepLon;

            for ( int j = 1; j < m_n; ++j ) {
                m_prevLat += stepLat;
                curStepLon += stepLon;
                double  evalLon = curStepLon;
                if ( curStepLon <= -M_PI )
                    evalLon += TWOPI;
                pixelValue( evalLon, m_prevLat, scanLine);
                ++scanLine;
            }
        }
    }
}
