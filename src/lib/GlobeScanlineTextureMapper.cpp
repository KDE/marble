
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

void GlobeScanlineTextureMapper::resizeMap(const QImage* canvasImage)
{
    AbstractScanlineTextureMapper::resizeMap(canvasImage);

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

void GlobeScanlineTextureMapper::pixelValueApprox(const double& lng, const double& lat,                                     QRgb *scanLine)
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
