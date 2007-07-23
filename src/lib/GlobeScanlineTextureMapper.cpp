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

#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "katlasdirs.h"
#include "TextureTile.h"
#include "TileLoader.h"

// Defining INTERLACE will make sure that for two subsequent scanlines
// every second scanline will be a deep copy of the first scanline.
// This results in a more coarse resolution and in a speedup for the 
// texture mapping of approx. 25%.

// #define INTERLACE

GlobeScanlineTextureMapper::GlobeScanlineTextureMapper( const QString& path, QObject * parent  ) : AbstractScanlineTextureMapper(path,parent)
{
    m_fastScanLine = 0;

    m_interpolate  = false;
    m_nBest = 0;

    m_n = 0;
    m_ninv = 0.0;

    m_x = 0;
    m_y = 0;
    m_z = 0;
    m_qr = 0.0; 
    m_qx = 0.0;
    m_qy = 0.0;
    m_qz = 0.0;

    m_interlaced = false;
}


GlobeScanlineTextureMapper::~GlobeScanlineTextureMapper()
{
    delete m_tileLoader;
}

void GlobeScanlineTextureMapper::resizeMap(int width, int height)
{
    AbstractScanlineTextureMapper::resizeMap( width, height );

    // Find the optimal m_n for the current image canvas width
    m_nBest = 2;

    int  nEvalMin = ( m_imageWidth - 1 );
    for ( int it = 1; it < 48; ++it ) {
        int nEval = ( m_imageWidth - 1 ) / it + ( m_imageWidth - 1 ) % it;
        if ( nEval < nEvalMin ) {
            nEvalMin = nEval;
            m_nBest = it; 
        }
    }
            qDebug() << QString( "Optimized n = %1, remainder: %2" ).arg(m_nBest).arg( ( m_imageWidth ) % m_nBest );
}


void GlobeScanlineTextureMapper::mapTexture(QImage* canvasImage, const int& radius, Quaternion& planetAxis)
{
    // Scanline based algorithm to texture map a sphere

    // Initialize needed variables:
    double  lng = 0.0;
    double  lat = 0.0;

    const int     radius2 = radius * radius;
    const double  radiusf = 1.0 / (double)(radius);

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLng = m_halfRangeLng - m_tilePosX;
    m_halfNormLat = m_halfRangeLat - m_tilePosY;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;

    // Reset backend
    m_tileLoader->resetTilehash();
    selectTileLevel(radius);

    // Evaluate the degree of interpolation
    m_n    = ( m_imageRadius < radius2 ) ? m_nBest : 8;
    m_ninv = 1.0 / (double)(m_n);

    // Calculate north pole position to decrease pole distortion later on
    Quaternion  northPole = GeoPoint( 0.0, (double)( -M_PI * 0.5 ) ).quaternion();

    Quaternion  inversePlanetAxis = planetAxis;
    inversePlanetAxis = inversePlanetAxis.inverse();

    northPole.rotateAroundAxis(inversePlanetAxis);

    // calculate axis matrix to represent the planet's rotation
    matrix  planetAxisMatrix;
    planetAxis.toMatrix( planetAxisMatrix );

    const int skip = ( m_interlaced == true ) ? 2 : 1;

    // Calculate the actual y-range of the map on the screen 
    const int yTop = ( m_imageHalfHeight-radius < 0 ) ? 0 : m_imageHalfHeight-radius;
    const int yBottom = (yTop == 0) ? m_imageHeight - skip + 1: yTop + radius + radius - skip + 1;

    for ( m_y = yTop; m_y < yBottom ; m_y+=skip ) {

        /* Should be fixed kind of properly now ... */
        // if(m_y == canvasImage->height()){
            /*FIXME: this is a simple off by one fix, should fix the 
             * cause not the symptom*/ 
        //    continue;
        // }

        // Evaluate coordinates for the 3D position vector of the current pixel
        m_qy = radiusf * (double)( m_y - m_imageHalfHeight );
        m_qr = 1.0 - m_qy * m_qy;

        // rx is the radius component in x direction
        int rx = (int)sqrt( (double)( radius2 
                      - ( ( m_y - m_imageHalfHeight )
                      * ( m_y - m_imageHalfHeight ) ) ) );
// #1
        // Calculate the actual x-range of the map within the current scanline
        const int xLeft  = ( ( m_imageHalfWidth-rx > 0 )
                             ? m_imageHalfWidth - rx : 0 ); 
        const int xRight = ( ( m_imageHalfWidth-rx > 0 )
                             ? xLeft + rx + rx : canvasImage -> width() );

        m_scanLine = (QRgb*)( canvasImage->scanLine( m_y ) ) + xLeft;
// #1
        if ( m_interlaced == true )
        {
            m_fastScanLine = (QRgb*)( canvasImage->scanLine( m_y + 1 ) ) + xLeft;
        }
// #2
        int  xIpLeft  = 1;
        int  xIpRight = m_n * (int)( xRight / m_n - 1) + 1; 
// #2

        if (m_imageHalfWidth-rx > 0) {
            xIpLeft  = m_n * (int)( xLeft  / m_n + 1 );
            xIpRight = m_n * (int)( xRight / m_n - 1 );
        }

        // Decrease pole distortion due to linear approximation ( y-axis )
        bool crossingPoleArea = false;
        int northPoleY = m_imageHalfHeight + (int)( radius * northPole.v[Q_Y] );
        if ( northPole.v[Q_Z] > 0
             && northPoleY - ( m_n * 0.75 ) <= m_y
             && northPoleY + ( m_n * 0.75 ) >= m_y ) 
        {
            crossingPoleArea = true;
        }

        int ncount = 0;


// #3
        for ( m_x = xLeft; m_x < xRight; ++m_x ) {
            // Prepare for interpolation

            int  leftInterval = xIpLeft + ncount * m_n;

            if ( m_x >= xIpLeft && m_x <= xIpRight ) {
// #3
                // Decrease pole distortion due to linear approximation ( x-axis )
                int northPoleX = m_imageHalfWidth + (int)( radius * northPole.v[Q_X] );

//                qDebug() << QString("NorthPole X: %1, LeftInterval: %2").arg( northPoleX ).arg( leftInterval );
                if ( crossingPoleArea == true
                     && northPoleX >= leftInterval + m_n
                     && northPoleX < leftInterval + 2*m_n
                     && m_x < leftInterval + 3 * m_n )
                {
                    m_interpolate = false;
                }
                else {
// #4
                    m_x += m_n - 1;
// #4
                    m_interpolate = true;
                    ++ncount;
                } 
            }
            else
                m_interpolate = false;

            // Evaluate more coordinates for the 3D position vector of the current pixel
            m_qx = (double)( m_x - m_imageHalfWidth ) * radiusf;

            double qr2z = m_qr - m_qx * m_qx;
            m_qz = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;        

            // Create Quaternion from vector coordinates and rotate it
            // around globe axis
            m_qpos.set( 0.0, m_qx, m_qy, m_qz );
            m_qpos.rotateAroundAxis( planetAxisMatrix );        

            m_qpos.getSpherical(lng, lat);

            // if (lat < 0) m_qpos->display(); 

            // Approx for m_n-1 out of n pixels within the boundary of
            // xIpLeft to xIpRight
            if ( m_interpolate ) {
                pixelValueApprox( lng, lat, m_scanLine );
/*
                if ( m_interlaced == true )
                {
                    for ( int j = 0; j < m_n - 1; ++j ) {
                        m_fastScanLine[j] = m_scanLine[j];
                    }
                    m_fastScanLine += ( m_n - 1 );
                }
// #5
*/
                m_scanLine += ( m_n - 1 );
// #5

            }

            // You can temporarily comment out this line and run Marble
            // to understand the interpolation:
            pixelValue( lng, lat, m_scanLine );

            m_prevLat = lat; // preparing for interpolation
            m_prevLng = lng;
/*
            if ( m_interlaced == true )
            {
               *m_fastScanLine = *m_scanLine;
               ++m_fastScanLine;
            }
*/
// #6
            ++m_scanLine;
// #6
        }
    }


    m_tileLoader->cleanupTilehash();
}


// This method interpolates color values for skipped pixels in a scanline.
 
// While moving along the scanline we don't move from pixel to pixel but
// leave out m_n pixels each time and calculate the exact position and 
// color value for the new pixel. The pixel values in between get 
// approximated through linear interpolation across the direct connecting 
// line on the original tiles directly.
// This method will do by far most of the calculations for the 
// texturemapping, so we move towards integer math to improve speed.

void GlobeScanlineTextureMapper::pixelValueApprox(const double& lng, const double& lat,                                     QRgb *scanLine)
{
    // stepLng/Lat: Distance between two subsequent approximated positions

    double stepLat = lat - m_prevLat;
    double stepLng = lng - m_prevLng;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    if ( fabs(stepLng) < M_PI ) 
    {
        const int itStepLng = (int)( stepLng * m_ninv * m_rad2PixelX * 128.0 );
        const int itStepLat = (int)( stepLat * m_ninv * m_rad2PixelY * 128.0 );

        m_prevLng *= m_rad2PixelX;
        m_prevLat *= m_rad2PixelY;

        // To improve speed we unroll 
        // AbstractScanlineTextureMapper::pixelValue(...) here and 
        // calculate the performance critical issues via integers

        int itLng = (int)( ( m_prevLng + m_halfNormLng ) * 128.0 );
        int itLat = (int)( ( m_prevLat + m_quatNormLat ) * 128.0 );

        if (m_tile->depth() == 8)
        {
            for (int j=1; j < m_n; ++j) 
            {
                m_posX = ( itLng + itStepLng * j ) >> 7;
                m_posY = ( itLat + itStepLat * j ) >> 7;

                if (  m_posX >= m_tileLoader->tileWidth() 
                   || m_posX < 0
                   || m_posY >= m_tileLoader->tileHeight()
                   || m_posY < 0 )
                {
                    nextTile();
                    itLng = (int)( ( m_prevLng + m_halfNormLng ) * 128.0 );
                    itLat = (int)( ( m_prevLat + m_quatNormLat ) * 128.0 );
                    m_posX = ( itLng + itStepLng * j ) >> 7;
                    m_posY = ( itLat + itStepLat * j ) >> 7;
                }

                *scanLine = m_tile->jumpTable8[m_posY][m_posX ];
                ++scanLine;
            }
        }
        else
        {
            for (int j=1; j < m_n; ++j) 
            {
                m_posX = ( itLng + itStepLng * j ) >> 7;
                m_posY = ( itLat + itStepLat * j ) >> 7;

                if (  m_posX >= m_tileLoader->tileWidth() 
                   || m_posX < 0
                   || m_posY >= m_tileLoader->tileHeight()
                   || m_posY < 0 )
                {
                    nextTile();
                    itLng = (int)( ( m_prevLng + m_halfNormLng ) * 128.0 );
                    itLat = (int)( ( m_prevLat + m_quatNormLat ) * 128.0 );
                    m_posX = ( itLng + itStepLng * j ) >> 7;
                    m_posY = ( itLat + itStepLat * j ) >> 7;
                }

                *scanLine = m_tile->jumpTable32[m_posY][m_posX ];
                ++scanLine;
            }
        }
    }

    // For the case where we cross the dateline between (lon, lat) and 
    // (prevlon, prevlat) we need a more sophisticated calculation.
    // However as this will happen rather rarely, we use 
    // pixelValue(...) directly to make the code more readable.

    else {
        stepLng = ( TWOPI - fabs(stepLng) ) * m_ninv;
        stepLat = stepLat * m_ninv;
        // We need to distinguish two cases:  
        // crossing the dateline from east to west ...

        if ( m_prevLng < lng ) {
            for ( int j = 1; j < m_n; ++j ) {
                m_prevLat += stepLat;
                m_prevLng -= stepLng;
                if ( m_prevLng <= -M_PI ) 
                    m_prevLng += TWOPI;
                pixelValue( m_prevLng, m_prevLat, scanLine );
                ++scanLine;
            }
        }

        // ... and vice versa: from west to east.

        else { 
            double curStepLng = lng - m_n*stepLng;

            for ( int j = 1; j < m_n; ++j ) {
                m_prevLat += stepLat;
                curStepLng += stepLng;
                double  evalLng = curStepLng;
                if ( curStepLng <= -M_PI )
                    evalLng += TWOPI;
                pixelValue( evalLng, m_prevLat, scanLine);
                ++scanLine;
            }
        }
    }
}
