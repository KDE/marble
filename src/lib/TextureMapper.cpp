/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>"
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include "TextureMapper.h"

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

const double TWOPI = 2 * M_PI;

TextureMapper::TextureMapper( const QString& path )
{
    m_posX = 0;
    m_posY = 0;

    m_tileLoader   = new TileLoader( path );
    m_scanLine     = 0;
    m_fastScanLine = 0;

    m_maxTileLevel = 0;
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

    m_imageHalfWidth  = 0;
    m_imageHalfHeight = 0;
    m_imageRadius     = 0;

    m_prevLat = 0.0; 
    m_prevLng = 0.0;

    m_tilePosX = 0;
    m_tilePosY = 0;

    m_fullRangeLng = 0;
    m_halfRangeLat = 0;
    m_halfRangeLng = 0.0;
    m_quatRangeLat = 0.0;
    m_fullNormLng = 0;
    m_halfNormLat = 0;
    m_halfNormLng = 0.0;
    m_quatNormLat = 0.0;

    m_rad2PixelX = 0.0;
    m_rad2PixelY = 0.0;

    m_tile = 0;
    m_tileLevel = 0;

    m_interlaced = false;
}


TextureMapper::~TextureMapper()
{
    delete m_tileLoader;
}


void TextureMapper::setMap( const QString& path )
{
    m_tileLoader->setMap(path);
}


void TextureMapper::selectTileLevel(const int& radius)
{
    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)

    double  linearLevel = ( 2.0 * (double)( radius )
			    / (double) ( m_tileLoader->tileWidth() ) );
    int    tileLevel   = 0;

    if (linearLevel < 1.0 )
        linearLevel = 1.0; // Dirty fix for invalid entry linearLevel

    tileLevel = (int)( log( linearLevel ) / log( 2.0 ) ) + 1;

    if ( tileLevel > m_maxTileLevel )
        tileLevel = m_maxTileLevel;

    if ( tileLevel != m_tileLevel ) {
        m_tileLoader->flush();
        m_tileLevel = tileLevel;

        tileLevelInit( tileLevel );
    }

    // qDebug() << "Texture Level was set to: " << tileLevel;
}


void TextureMapper::tileLevelInit( int tileLevel )
{
    int ResolutionX = (int)( 4320000.0
			     / (double)( TileLoader::levelToColumn( tileLevel ) )
			     / (double)( m_tileLoader->tileWidth() ) );
    int ResolutionY = (int)( 2160000.0
			     / (double)( TileLoader::levelToRow( tileLevel ) )
			     / (double)( m_tileLoader->tileHeight() ) );

    m_rad2PixelX = ( 2160000.0 / M_PI / (double)(ResolutionX) );
    m_rad2PixelY = ( 2160000.0 / M_PI / (double)(ResolutionY) );


    m_fullRangeLng = (int)   ( 4320000.0 / (double)(ResolutionX) ) - 1;
    m_halfRangeLng = (double)( 2160000.0 / (double)(ResolutionX) );
    m_quatRangeLat = (double)( 1080000.0 / (double)(ResolutionY) );
    m_halfRangeLat = (int)   ( 2.0 * m_quatRangeLat ) - 1;

    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLng = m_halfRangeLng - m_tilePosX;
    m_halfNormLat = m_halfRangeLat - m_tilePosY;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;
}


void TextureMapper::resizeMap(const QImage* canvasImage)
{
    m_imageHalfWidth  = canvasImage -> width() / 2;
    m_imageHalfHeight = canvasImage -> height() / 2;
    m_imageRadius     = ( m_imageHalfWidth * m_imageHalfWidth
                          + m_imageHalfHeight * m_imageHalfHeight );

    // Find the optimal m_n for the current image canvas width

    m_nBest = 2;

    int  nEvalMin = 2 * m_imageHalfWidth;
    for ( int it = 1; it < 32; ++it ) {
        int nEval = 2 * m_imageHalfWidth / it + 2 * m_imageHalfWidth % it;
        if ( nEval < nEvalMin ) {
            nEvalMin = nEval;
            m_nBest = it; 
        }
    }
    //        qDebug("Optimized n = " + QString::number(nBest).toLatin1());
}


void TextureMapper::mapTexture(QImage* canvasImage, const int& radius, 
                               Quaternion& planetAxis)
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

    // Calculate the actual y-range of the map on the screen 
    const int yTop = ( m_imageHalfHeight-radius < 0 ) ? 0 : m_imageHalfHeight-radius;
    const int yBottom = (yTop == 0) ? 2 * m_imageHalfHeight : yTop + radius + radius;

    Quaternion  *qpos = new Quaternion();

    // Calculate north pole position to decrease pole distortion later on
    Quaternion  northPole = GeoPoint( 0.0, (double)( -M_PI * 0.5 ) ).quaternion();

    Quaternion  inversePlanetAxis = planetAxis;
    inversePlanetAxis = inversePlanetAxis.inverse();

    northPole.rotateAroundAxis(inversePlanetAxis);

    // calculate axis matrix to represent the planet's rotation
    matrix  planetAxisMatrix;
    planetAxis.toMatrix( planetAxisMatrix );

    const int skip = ( m_interlaced == true ) ? 2 : 1;
    const int yEnd = yBottom - skip + 2;

    for ( m_y = yTop; m_y < yEnd ; m_y+=skip ) {

        // Evaluate coordinates for the 3D position vector of the current pixel
        m_qy = radiusf * (double)( m_y - m_imageHalfHeight );
        m_qr = 1.0 - m_qy * m_qy;

        // rx is the radius component in x direction
        int rx = (int)sqrt( (double)( radius2 
				      - ( ( m_y - m_imageHalfHeight )
					  * ( m_y - m_imageHalfHeight ) ) ) );

        // Calculate the actual x-range of the map within the current scanline
        const int xLeft  = ( ( m_imageHalfWidth-rx > 0 )
                             ? m_imageHalfWidth - rx : 0 ); 
        const int xRight = ( ( m_imageHalfWidth-rx > 0 )
                             ? xLeft + rx + rx : 2 * m_imageHalfWidth );

        m_scanLine = (QRgb*)( canvasImage->scanLine( m_y ) ) + xLeft;

        if ( m_interlaced == true )
        {
            m_fastScanLine = (QRgb*)( canvasImage->scanLine( m_y + 1 ) ) + xLeft;
        }

        int  xIpLeft  = 1;
        int  xIpRight = (int)(2 * m_imageHalfWidth * m_ninv) * m_n; 

        if (m_imageHalfWidth-rx > 0) {
            xIpLeft  = m_n * (int)( xLeft  / m_n + 1 );
            xIpRight = m_n * (int)( xRight / m_n - 1 );
        }

        // Decrease pole distortion due to linear approximation ( y-axis )
        bool crossingPoleArea = false;
        int northPoleY = m_imageHalfHeight + (int)( radius * northPole.v[Q_Y] );
        if ( northPole.v[Q_Z] > 0
             && northPoleY - m_n / 2 <= m_y
             && northPoleY + m_n / 2 >= m_y ) 
        {
            crossingPoleArea = true;
        }

        int ncount = 0;

        for ( m_x = xLeft; m_x < xRight; ++m_x ) {
            // Prepare for interpolation

            if ( m_x >= xIpLeft && m_x <= xIpRight ) {

                // Decrease pole distortion due to linear approximation ( x-axis )
                int northPoleX = m_imageHalfWidth + (int)( radius * northPole.v[Q_X] );

                int  leftInterval = xIpLeft + ncount * m_n;
                if ( crossingPoleArea == true
                     && northPoleX > leftInterval
                     && northPoleX < leftInterval + m_n
                     && m_x < leftInterval + m_n )
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

            // Evaluate more coordinates for the 3D position vector of the current pixel
            m_qx = (double)( m_x - m_imageHalfWidth ) * radiusf;

            double qr2z = m_qr - m_qx * m_qx;
            m_qz = ( qr2z > 0.0 ) ? sqrt( qr2z ) : 0.0;        

            // Create Quaternion from vector coordinates and rotate it
            // around globe axis
            qpos->set( 0, m_qx, m_qy, m_qz );
            qpos->rotateAroundAxis( planetAxisMatrix );        

            qpos->getSpherical(lng, lat);

            // if (lat < 0) qpos->display(); 

            // Approx for m_n-1 out of n pixels within the boundary of
            // xIpLeft to xIpRight
            if ( m_interpolate ) {
                pixelValueApprox( lng, lat, m_scanLine );

                if ( m_interlaced == true )
                {
                    for ( int j = 0; j < m_n - 1; ++j ) {
                        m_fastScanLine[j] = m_scanLine[j];
                    }
                    m_fastScanLine += ( m_n - 1 );
                }

                m_scanLine += ( m_n - 1 );
            }

            // You can temporarily comment out this line and run Marble
            // to understand the interpolation:
            pixelValue( lng, lat, m_scanLine );
 
            m_prevLat = lat; // preparing for interpolation
            m_prevLng = lng;

            if ( m_interlaced == true )
            {
               *m_fastScanLine = *m_scanLine;
               ++m_fastScanLine;
            }

            ++m_scanLine;
        }
    }

    delete qpos;

    m_tileLoader->cleanupTilehash();
}


// This method interpolates color values for skipped pixels in a scanline.
 
// While moving along the scanline we don't move from pixel to pixel but
// leave out m_n pixels each time and calculate the exact position and 
// color value for the new pixel. The pixel values in between get 
// approximated through linear interpolation across the direct connecting 
// line on the original tiles directly.

void TextureMapper::pixelValueApprox(const double& lng, const double& lat, 
                                     QRgb *scanLine)
{
    // stepLng/Lat: Distance between two subsequent approximated positions

    double stepLat = ( lat - m_prevLat ) * m_ninv;
    double stepLng = lng - m_prevLng;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    if ( fabs(stepLng) < M_PI ) {
        stepLng *= m_ninv;

        for (int j=1; j < m_n; ++j) {
            m_prevLat += stepLat;
            m_prevLng += stepLng;
            pixelValue( m_prevLng, m_prevLat, scanLine);
            ++scanLine;
        }
    }

    // For the case where we cross the dateline between (lon, lat) and 
    // (prevlon, prevlat) we need a more sophisticated calculation:

    else {
        stepLng = ( TWOPI - fabs(stepLng) ) * m_ninv;

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


inline void TextureMapper::pixelValue(const double& lng, 
                                      const double& lat, QRgb* scanLine)
{
    // Convert the lng and lat coordinates of the position on the scanline
    // measured in radiant to the pixel position of the requested 
    // coordinate on the current tile:
 
    m_posX = (int)( m_halfNormLng + lng * m_rad2PixelX );
    m_posY = (int)( m_quatNormLat + lat * m_rad2PixelY );

    // Most of the time while moving along the scanLine we'll stay on the 
    // same tile. However at the tile border we might "fall off". If that 
    // happens we need to find out the next tile that needs to be loaded.

    if ( m_posX  >= m_tileLoader->tileWidth() 
         || m_posX < 0
         || m_posY >= m_tileLoader->tileHeight()
         || m_posY < 0 ) nextTile();

    // Now retrieve the color value of the requested pixel on the tile.
    // This needs to be done differently for grayscale ( uchar, 1 byte ).
    // and color ( uint, 4 bytes ) images.

    if (m_tile->depth() == 8)
        *scanLine = m_tile->jumpTable8[m_posY][m_posX ];
    else
        *scanLine = m_tile->jumpTable32[m_posY][m_posX ];
}

void TextureMapper::nextTile()
{
    // necessary to prevent e.g. crash if lng = -pi

    if ( m_posX > m_fullNormLng ) m_posX = m_fullNormLng;
    if ( m_posY > m_halfNormLat  ) m_posY = m_halfNormLat;

    // The origin (0, 0) is in the upper left corner
    // lng: 360 deg = 4320000 pixel
    // lat: 180 deg = 2160000 pixel

    int lng = m_posX + m_tilePosX;
    int lat = m_posY + m_tilePosY;

    // tileCol counts the tile columns left from the current tile.
    // tileRow counts the tile rows on the top from the current tile.

    int tileCol = lng / m_tileLoader->tileWidth();
    int tileRow = lat / m_tileLoader->tileHeight();

    m_tile = m_tileLoader->loadTile( tileCol, tileRow, m_tileLevel );

    // Recalculate some convenience variables for the new tile:
    // m_tilePosX/Y stores the position of the tiles in pixels

    m_tilePosX = tileCol * m_tileLoader->tileWidth();
    m_tilePosY = tileRow * m_tileLoader->tileHeight();

    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLng = m_halfRangeLng - m_tilePosX;
    m_halfNormLat = m_halfRangeLat - m_tilePosY;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;

    m_posX = lng - m_tilePosX;
    m_posY = lat - m_tilePosY;
}

