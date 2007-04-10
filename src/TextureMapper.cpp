#include <cmath>

#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtGui/QColor>

#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "katlasdirs.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "TextureMapper.h"


// #define INTERLACE

const float TWOPI = 2 * M_PI;

TextureMapper::TextureMapper( const QString& path )
{
    m_maxTileLevel = 0;

    m_tileLoader = new TileLoader( path );

    scanLine = 0;

    x = 0;
    y = 0;
    z = 0;

    qr = 0.0f;
    qx = 0.0f;
    qy = 0.0f;
    qz = 0.0f;

    m_imageHalfWidth = 0; 
    m_imageHalfHeight = 0;
    m_imageRadius = 0;

    m_tileLevel = 0;
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
    float  linearlevel = (float)( radius / 335.0f );
    int    tileLevel   = 0;

    if (linearlevel > 0.0f )
        tileLevel = (int)( logf( linearlevel ) / logf( 2.0f ) ) + 1;

    if ( tileLevel > m_maxTileLevel )
        tileLevel = m_maxTileLevel;

    //        m_tileLoader->setTileLevel(tileLevel);
    if ( tileLevel != m_tileLevel ) {
        m_tileLoader->flush();
        m_tileLevel = tileLevel;
    }

    tileLevelInit( tileLevel );
    //        qDebug() << "Texture Level was set to: " << tileLevel;
}


void TextureMapper::tileLevelInit( int tileLevel )
{
    int ResolutionX = (int)( 4320000.0f 
                         / (float)( TileLoader::levelToColumn( tileLevel ) )
                         / (float)( m_tileLoader->tileWidth() ) );
    int ResolutionY = (int)( 2160000.0f
                         / (float)( TileLoader::levelToRow( tileLevel ) )
                         / (float)( m_tileLoader->tileHeight() ) );

    m_rad2PixelX = ( 2160000.0f / M_PI / (float)(ResolutionX) );
    m_rad2PixelY = ( 2160000.0f / M_PI / (float)(ResolutionY) );


    m_fullRangeLng = (int)  ( 4320000.0f / (float)(ResolutionX) ) - 1;
    m_halfRangeLng = (float)( 2160000.0f / (float)(ResolutionX) );
    m_quatRangeLng = (int)  ( 1080000.0f / (float)(ResolutionX) );
    m_quatRangeLat = (float)( 1080000.0f / (float)(ResolutionY) );
    m_halfRangeLat = (int)  ( 2.0f * m_quatRangeLat ) - 1;

    m_halfNormLng = m_halfRangeLng - m_tilePosX;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;
    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLat = m_halfRangeLat - m_tilePosY;
}


void TextureMapper::resizeMap(const QImage* canvasImage)
{
    m_imageHalfWidth  = canvasImage -> width() / 2;
    m_imageHalfHeight = canvasImage -> height() / 2;
    m_imageRadius     = ( m_imageHalfWidth * m_imageHalfWidth
                          + m_imageHalfHeight * m_imageHalfHeight );

    nopt = 2;

    int  nevalmin = 2 * m_imageHalfWidth;
    for ( int it = 1; it < 32; it++ ) {
        int neval = 2 * m_imageHalfWidth / it + 2 * m_imageHalfWidth % it;
        if ( neval < nevalmin ) {
            nevalmin = neval;
            nopt = it; 
        }
    }
    //        qDebug("Optimized n = " + QString::number(nopt).toLatin1());
}


void TextureMapper::mapTexture(QImage* canvasImage, const int& radius, 
                               Quaternion& planetAxis)
{
    // Scanline based algorithm to texture map a sphere

    float lng = 0.0f;
    float lat = 0.0f;

    const int   radius2 = radius * radius;
    const float radiusf = 1.0f / (float)(radius);

    m_tileLoader->resetTilehash();

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    m_halfNormLng = m_halfRangeLng - m_tilePosX;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;
    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLat = m_halfRangeLat - m_tilePosY;

    selectTileLevel(radius);

    // Evaluate the degree of interpolation
    m_n    = ( m_imageRadius < radius2 ) ? nopt : 8;
    m_ninv = 1.0f / (float)(m_n);

    // Calculate the actual y-range of the map on the screen 
    const int ytop = ( m_imageHalfHeight-radius < 0 ) ? 0 : m_imageHalfHeight-radius;
    const int ybottom = (ytop == 0) ? 2 * m_imageHalfHeight : ytop + radius + radius;

    Quaternion  *qpos = new Quaternion();

    // Calculate north pole position to decrease pole distortion later on
    Quaternion  northPole = GeoPoint( 0.0f, (float)( -M_PI * 0.5 ) ).quaternion();

    Quaternion  inversePlanetAxis = planetAxis;
    inversePlanetAxis = inversePlanetAxis.inverse();

    northPole.rotateAroundAxis(inversePlanetAxis);

    matrix  planetAxisMatrix;
    planetAxis.toMatrix( planetAxisMatrix );

#ifndef INTERLACE
    for (y = ytop; y < ybottom; y++) {
#else
    for (y = ytop; y < ybottom -1; y+=2) {
#endif
        // Evaluate coordinates for the 3D position vector of the current pixel
        qy = radiusf * (float)(y - m_imageHalfHeight);
        qr = 1.0f - qy * qy;

        // rx is the radius component in x direction
        int rx = (int)sqrtf( (float)( radius2 
                                      - ( ( y - m_imageHalfHeight )
                                          * (y - m_imageHalfHeight) ) ) );

        // Calculate the actual x-range of the map within the current scanline
        const int xleft  = (m_imageHalfWidth-rx > 0) ? m_imageHalfWidth - rx : 0; 
        const int xright = (m_imageHalfWidth-rx > 0) ? xleft + rx + rx : 2 * m_imageHalfWidth;

        scanLine = (QRgb*)(canvasImage->scanLine( y )) + xleft;
#ifdef INTERLACE
        fastScanLine = (QRgb*)(canvasImage->scanLine( y + 1 )) + xleft;
#endif

        int  xipleft  = 1;
        int  xipright = (int)(2 * m_imageHalfWidth * m_ninv) * m_n; 

        if (m_imageHalfWidth-rx > 0) {
            xipleft  = m_n * (int)( xleft  / m_n + 1 );
            xipright = m_n * (int)( xright / m_n - 1 );
        }

        // Decrease pole distortion due to linear approximation ( y-axis )
        bool poleyenv = false;
        int northPoleY = m_imageHalfHeight + (int)( radius * northPole.v[Q_Y] );
        if ( northPole.v[Q_Z] > 0
             && northPoleY - m_n/2 <= y
             && northPoleY + m_n/2 >= y ) 
        {
            poleyenv = true;
        }

        int ncount = 0;

        for ( x = xleft; x < xright; x++ ) {
            // Prepare for interpolation

            if ( x >= xipleft && x <= xipright ) {

                // Decrease pole distortion due to linear approximation ( x-axis )
                int northPoleX = m_imageHalfWidth + (int)( radius * northPole.v[Q_X] );

                int  leftinterval = xipleft + ncount * m_n;
                if ( poleyenv == true
                     && northPoleX > leftinterval
                     && northPoleX < leftinterval + m_n
                     && x < leftinterval + m_n )
                {
                    interpolate = false;
                }
                else {
                    x += m_n - 1;
                    interpolate = true;
                    ncount++;
                } 
            }
            else 
                interpolate = false;

            // Evaluate more coordinates for the 3D position vector of the current pixel
            qx = (float)(x - m_imageHalfWidth) * radiusf;

            float qr2z = qr - qx*qx;
            qz = (qr2z > 0.0f) ? sqrtf(qr2z) : 0.0f;        

            // Create Quaternion from vector coordinates and rotate it
            // around globe axis
            qpos->set(0,qx,qy,qz);
            qpos->rotateAroundAxis( planetAxisMatrix );        

            qpos->getSpherical(lng, lat);

            // if (lat < 0) qpos->display(); 

            // Approx for m_n-1 out of n pixels within the boundary of
            // xipleft to xipright
            if ( interpolate ) {
                getPixelValueApprox(lng,lat,scanLine);
#ifdef INTERLACE
                for ( int j = 0; j < m_n - 1; j++ ) {
                    fastScanLine[j]=scanLine[j];
                }
                fastScanLine += ( m_n - 1 );
#endif
                scanLine += ( m_n - 1 );
            }

            getPixelValue(lng, lat, scanLine);
 
            m_prevLat = lat; // preparing for interpolation
            m_prevLng = lng;
 #ifdef INTERLACE
            *fastScanLine=*scanLine;
            fastScanLine++;
#endif
            scanLine++;
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

void TextureMapper::getPixelValueApprox(const float& lng, const float& lat, 
                                        QRgb *scanLine)
{
    // stepLng/Lat: Distance between two subsequent approximated positions

    float stepLat = ( lat - m_prevLat ) * m_ninv;
    float stepLng = lng - m_prevLng;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    if ( fabs(stepLng) < M_PI ) {
        stepLng *= m_ninv;

        for (int j=1; j < m_n; j++) {
            m_prevLat += stepLat;
            m_prevLng += stepLng;
            getPixelValue( m_prevLng, m_prevLat, scanLine);
            scanLine++;
        }
    }

    // For the case where we cross the dateline between (lon, lat) and 
    // (prevlon, prevlat) we need a more sophisticated calculation:

    else {
        stepLng = ( TWOPI - fabs(stepLng) ) * m_ninv;

        // We need to distinguish two cases: crossing the dateline 
        // from east to west and vice versa: from west to east.

        if ( m_prevLng < lng ) {
            for ( int j = 1; j < m_n; j++ ) {
                m_prevLat += stepLat;
                m_prevLng -= stepLng;
                if ( m_prevLng <= -M_PI ) 
                    m_prevLng += TWOPI;
                getPixelValue( m_prevLng, m_prevLat, scanLine );
                scanLine++;
            }
        }

        // if (m_prevLng > lng)
        else { 
            float curStepLng = lng - m_n*stepLng;

            for ( int j = 1; j < m_n; j++ ) {
                m_prevLat += stepLat;
                curStepLng += stepLng;
                float  evalLng = curStepLng;
                if ( curStepLng <= -M_PI )
                    evalLng += TWOPI;
                getPixelValue( evalLng, m_prevLat, scanLine);
                scanLine++;
            }
        }
    }
}


inline void TextureMapper::getPixelValue(const float& lng, 
                                         const float& lat, QRgb* scanLine)
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
         || m_posY < 0 )
    {
        // necessary to prevent e.g. crash if lng = -pi

        if ( m_posX > m_fullNormLng ) m_posX = m_fullNormLng;
        if ( m_posY > m_halfNormLat  ) m_posY = m_halfNormLat;

        // The origin (0, 0) is in the upper left corner
        // lng: 360 deg = 4320000 pixel
        // lat: 180 deg = 2160000 pixel

        int lng = m_posX  + m_tilePosX;
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

        m_halfNormLng =  m_halfRangeLng -  m_tilePosX;
        m_quatNormLat =  m_quatRangeLat -  m_tilePosY;
        m_fullNormLng =  m_fullRangeLng -  m_tilePosX;
        m_halfNormLat =  m_halfRangeLat -  m_tilePosY;

        m_posX = lng - m_tilePosX;
        m_posY = lat - m_tilePosY;
    }

    // Now retrieve the color value of the requested pixel on the tile.
    // This needs to be done differently for grayscale and color images.

    if (m_tile->depth() == 8)
        *scanLine = m_tile->jumpTable8[m_posY][m_posX ];
    else
        *scanLine = m_tile->jumpTable32[m_posY][m_posX ];
}
