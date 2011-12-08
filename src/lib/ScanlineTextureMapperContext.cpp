//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de
//

#include "ScanlineTextureMapperContext.h"

#include <QtGui/QImage>

#include "MarbleDebug.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "TileId.h"
#include "ViewParams.h"
#include "ViewportParams.h"

using namespace Marble;

ScanlineTextureMapperContext::ScanlineTextureMapperContext( StackedTileLoader * const tileLoader, int tileLevel )
    : m_tileLoader( tileLoader ),
      m_textureProjection( tileLoader->tileProjection() ),  // cache texture projection
      m_tileSize( tileLoader->tileSize() ),  // cache tile size
      m_tileLevel( tileLevel ),
      m_globalWidth( m_tileSize.width() * m_tileLoader->tileColumnCount( m_tileLevel ) ),
      m_globalHeight( m_tileSize.height() * m_tileLoader->tileRowCount( m_tileLevel ) ),
      m_normGlobalWidth( m_globalWidth / ( 2 * M_PI ) ),
      m_normGlobalHeight( m_globalHeight /  M_PI ),
      m_tile( 0 ),
      m_deltaLevel( 0 ),
      m_tilePosX( 65535 ),
      m_tilePosY( 65535 ),
      m_vTileStartX( 0 ),
      m_vTileStartY( 0 ),
      m_toTileCoordinatesLon( 0.5 * m_globalWidth  - m_tilePosX ),
      m_toTileCoordinatesLat( 0.5 * m_globalHeight - m_tilePosY ),
      m_prevLat( 0.0 ),
      m_prevLon( 0.0 )
{
}

void ScanlineTextureMapperContext::pixelValueF( const qreal lon, const qreal lat,
                                                QRgb* const scanLine )
{
    // The same method using integers performs about 33% faster.
    // However we need the qreal version to create the high quality mode.

    // Convert the lon and lat coordinates of the position on the scanline
    // measured in radian to the pixel position of the requested 
    // coordinate on the current tile.

    qreal posX = m_toTileCoordinatesLon + rad2PixelX( lon );
    qreal posY = m_toTileCoordinatesLat + rad2PixelY( lat );

    // Most of the time while moving along the scanLine we'll stay on the 
    // same tile. However at the tile border we might "fall off". If that 
    // happens we need to find out the next tile that needs to be loaded.

    if ( posX  >= (qreal)( m_tileSize.width() )
         || posX < 0.0
         || posY >= (qreal)( m_tileSize.height() )
         || posY < 0.0 )
    {
        nextTile( posX, posY );
    }
    if ( m_tile ) {
        *scanLine = m_tile->pixelF( ( (int)posX + m_vTileStartX ) / ( 1 << m_deltaLevel ),
                                    ( (int)posY + m_vTileStartY ) / ( 1 << m_deltaLevel ) );
    }
    else {
        *scanLine = 0;
    }

    m_prevLon = lon;
    m_prevLat = lat; // preparing for interpolation
}

void ScanlineTextureMapperContext::pixelValue( const qreal lon, const qreal lat,
                                               QRgb* const scanLine )
{
    // The same method using integers performs about 33% faster.
    // However we need the qreal version to create the high quality mode.

    // Convert the lon and lat coordinates of the position on the scanline
    // measured in radian to the pixel position of the requested 
    // coordinate on the current tile.

    int iPosX = (int)( m_toTileCoordinatesLon + rad2PixelX( lon ) );
    int iPosY = (int)( m_toTileCoordinatesLat + rad2PixelY( lat ) );

    // Most of the time while moving along the scanLine we'll stay on the 
    // same tile. However at the tile border we might "fall off". If that 
    // happens we need to find out the next tile that needs to be loaded.

    if ( iPosX  >= m_tileSize.width() 
         || iPosX < 0
         || iPosY >= m_tileSize.height()
         || iPosY < 0 )
    {
        nextTile( iPosX, iPosY );
    }

    if ( m_tile ) {
        *scanLine = m_tile->pixel( ( iPosX + m_vTileStartX ) >> m_deltaLevel,
                                   ( iPosY + m_vTileStartY ) >> m_deltaLevel );
    }
    else {
        *scanLine = 0;
    }

    m_prevLon = lon;
    m_prevLat = lat; // preparing for interpolation
}

// This method interpolates color values for skipped pixels in a scanline.
// 
// While moving along the scanline we don't move from pixel to pixel but
// leave out n pixels each time and calculate the exact position and 
// color value for the new pixel. The pixel values in between get 
// approximated through linear interpolation across the direct connecting 
// line on the original tiles directly.
// This method will do by far most of the calculations for the 
// texturemapping, so we move towards integer math to improve speed.

void ScanlineTextureMapperContext::pixelValueApproxF( const qreal lon, const qreal lat,
                                                      QRgb *scanLine, const int n )
{
    // stepLon/Lat: Distance between two subsequent approximated positions

    qreal stepLat = lat - m_prevLat;
    qreal stepLon = lon - m_prevLon;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    const qreal nInverse = 1.0 / (qreal)(n);

    if ( fabs(stepLon) < M_PI ) {
        const qreal prevPixelX = rad2PixelX( m_prevLon );
        const qreal prevPixelY = rad2PixelY( m_prevLat );

        const qreal itStepLon = ( rad2PixelX( lon ) - prevPixelX ) * nInverse;
        const qreal itStepLat = ( rad2PixelY( lat ) - prevPixelY ) * nInverse;

        // To improve speed we unroll 
        // AbstractScanlineTextureMapper::pixelValue(...) here and 
        // calculate the performance critical issues via integers

        qreal itLon = prevPixelX + m_toTileCoordinatesLon;
        qreal itLat = prevPixelY + m_toTileCoordinatesLat;

        const int tileWidth = m_tileSize.width();
        const int tileHeight = m_tileSize.height();

        // int oldR = 0;
        // int oldG = 0;
        // int oldB = 0;

        QRgb oldRgb = qRgb( 0, 0, 0 );

        qreal oldPosX = -1;
        qreal oldPosY = 0;

        const bool alwaysCheckTileRange =
                isOutOfTileRangeF( itLon, itLat, itStepLon, itStepLat, n );
        
        for ( int j=1; j < n; ++j ) {
            qreal posX = itLon + itStepLon * j;
            qreal posY = itLat + itStepLat * j;
            if ( alwaysCheckTileRange )
                if ( posX >= tileWidth
                    || posX < 0.0
                    || posY >= tileHeight
                    || posY < 0.0 )
                {
                    nextTile( posX, posY );
                    itLon = prevPixelX + m_toTileCoordinatesLon;
                    itLat = prevPixelY + m_toTileCoordinatesLat;
                    posX = qMax<qreal>( 0.0, qMin<qreal>( tileWidth-1.0, itLon + itStepLon * j ) );
                    posY = qMax<qreal>( 0.0, qMin<qreal>( tileHeight-1.0, itLat + itStepLat * j ) );
                    oldPosX = -1;
                }

            *scanLine = m_tile->pixel( ( (int)posX + m_vTileStartX ) >> m_deltaLevel,
                                       ( (int)posY + m_vTileStartY ) >> m_deltaLevel ); 

            // Just perform bilinear interpolation if there's a color change compared to the 
            // last pixel that was evaluated. This speeds up things greatly for maps like OSM
            if ( *scanLine != oldRgb ) {
                if ( oldPosX != -1 ) {
                    *(scanLine - 1) = m_tile->pixelF( ( oldPosX + m_vTileStartX ) / ( 1 << m_deltaLevel ),
                                                      ( oldPosY + m_vTileStartY ) / ( 1 << m_deltaLevel ),
                                                      *(scanLine - 1) );
                    oldPosX = -1;
                }
                oldRgb = m_tile->pixelF( ( posX + m_vTileStartX ) / ( 1 << m_deltaLevel ),
                                         ( posY + m_vTileStartY ) / ( 1 << m_deltaLevel ),
                                         *scanLine );
                *scanLine = oldRgb;
            }
            else {
                oldPosX = posX;
                oldPosY = posY;
            }

            // if ( needsFilter( *scanLine, oldR, oldB, oldG  ) ) {
            //     *scanLine = m_tile->pixelF( posX, posY );
            // }

            ++scanLine;
        }
    }

    // For the case where we cross the dateline between (lon, lat) and 
    // (prevlon, prevlat) we need a more sophisticated calculation.
    // However as this will happen rather rarely, we use 
    // pixelValue(...) directly to make the code more readable.

    else {
        stepLon = ( TWOPI - fabs(stepLon) ) * nInverse;
        stepLat = stepLat * nInverse;
        // We need to distinguish two cases:  
        // crossing the dateline from east to west ...

        if ( m_prevLon < lon ) {

            for ( int j = 1; j < n; ++j ) {
                m_prevLat += stepLat;
                m_prevLon -= stepLon;
                if ( m_prevLon <= -M_PI ) 
                    m_prevLon += TWOPI;
                pixelValueF( m_prevLon, m_prevLat, scanLine );
                ++scanLine;
            }
        }

        // ... and vice versa: from west to east.

        else { 
            qreal curStepLon = lon - n * stepLon;

            for ( int j = 1; j < n; ++j ) {
                m_prevLat += stepLat;
                curStepLon += stepLon;
                qreal  evalLon = curStepLon;
                if ( curStepLon <= -M_PI )
                    evalLon += TWOPI;
                pixelValueF( evalLon, m_prevLat, scanLine );
                ++scanLine;
            }
        }
    }
}


bool ScanlineTextureMapperContext::isOutOfTileRangeF( const qreal itLon, const qreal itLat,
                                                      const qreal itStepLon, const qreal itStepLat,
                                                      const int n ) const
{
    const qreal minIPosX = itLon + itStepLon;
    const qreal minIPosY = itLat + itStepLat;
    const qreal maxIPosX = itLon + itStepLon * ( n - 1 );
    const qreal maxIPosY = itLat + itStepLat * ( n - 1 );
    return (    maxIPosX >= m_tileSize.width()  || maxIPosX < 0
             || maxIPosY >= m_tileSize.height() || maxIPosY < 0
             || minIPosX >= m_tileSize.width()  || minIPosX < 0
             || minIPosY >= m_tileSize.height() || minIPosY < 0 );
}


void ScanlineTextureMapperContext::pixelValueApprox( const qreal lon, const qreal lat,
                                                     QRgb *scanLine, const int n )
{
    // stepLon/Lat: Distance between two subsequent approximated positions

    qreal stepLat = lat - m_prevLat;
    qreal stepLon = lon - m_prevLon;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    const qreal nInverse = 1.0 / (qreal)(n);

    if ( fabs(stepLon) < M_PI ) {
        const qreal prevPixelX = rad2PixelX( m_prevLon );
        const qreal prevPixelY = rad2PixelY( m_prevLat );

        const int itStepLon = (int)( ( rad2PixelX( lon ) - prevPixelX ) * nInverse * 128.0 );
        const int itStepLat = (int)( ( rad2PixelY( lat ) - prevPixelY ) * nInverse * 128.0 );

        // To improve speed we unroll 
        // AbstractScanlineTextureMapper::pixelValue(...) here and 
        // calculate the performance critical issues via integers

        int itLon = (int)( ( prevPixelX + m_toTileCoordinatesLon ) * 128.0 );
        int itLat = (int)( ( prevPixelY + m_toTileCoordinatesLat ) * 128.0 );

        const int tileWidth = m_tileSize.width();
        const int tileHeight = m_tileSize.height();

        const bool alwaysCheckTileRange =
                isOutOfTileRange( itLon, itLat, itStepLon, itStepLat, n );
                                  
        if ( !alwaysCheckTileRange ) {
            int iPosXf = itLon;
            int iPosYf = itLat;
            for ( int j = 1; j < n; ++j ) {
                iPosXf += itStepLon;
                iPosYf += itStepLat;
                *scanLine = m_tile->pixel( ( ( iPosXf >> 7 ) + m_vTileStartX ) >> m_deltaLevel,
                                           ( ( iPosYf >> 7 ) + m_vTileStartY ) >> m_deltaLevel );
                ++scanLine;
            }
        }        
        else {
            for ( int j = 1; j < n; ++j ) {
                int iPosX = ( itLon + itStepLon * j ) >> 7;
                int iPosY = ( itLat + itStepLat * j ) >> 7;

                if ( iPosX >= tileWidth
                    || iPosX < 0
                    || iPosY >= tileHeight
                    || iPosY < 0 )
                {
                    nextTile( iPosX, iPosY );
                    itLon = (int)( ( prevPixelX + m_toTileCoordinatesLon ) * 128.0 );
                    itLat = (int)( ( prevPixelY + m_toTileCoordinatesLat ) * 128.0 );
                    iPosX = ( itLon + itStepLon * j ) >> 7;
                    iPosY = ( itLat + itStepLat * j ) >> 7;
                }

                *scanLine = m_tile->pixel( ( iPosX + m_vTileStartX ) >> m_deltaLevel,
                                           ( iPosY + m_vTileStartY ) >> m_deltaLevel );
                ++scanLine;
            }
        }
    }

    // For the case where we cross the dateline between (lon, lat) and 
    // (prevlon, prevlat) we need a more sophisticated calculation.
    // However as this will happen rather rarely, we use 
    // pixelValue(...) directly to make the code more readable.

    else {
        stepLon = ( TWOPI - fabs(stepLon) ) * nInverse;
        stepLat = stepLat * nInverse;
        // We need to distinguish two cases:  
        // crossing the dateline from east to west ...

        if ( m_prevLon < lon ) {

            for ( int j = 1; j < n; ++j ) {
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
            qreal curStepLon = lon - n * stepLon;

            for ( int j = 1; j < n; ++j ) {
                m_prevLat += stepLat;
                curStepLon += stepLon;
                qreal  evalLon = curStepLon;
                if ( curStepLon <= -M_PI )
                    evalLon += TWOPI;
                pixelValue( evalLon, m_prevLat, scanLine );
                ++scanLine;
            }
        }
    }
}


bool ScanlineTextureMapperContext::isOutOfTileRange( const int itLon, const int itLat,
                                                     const int itStepLon, const int itStepLat,
                                                     const int n ) const
{
    const int minIPosX = ( itLon + itStepLon ) >> 7;
    const int minIPosY = ( itLat + itStepLat ) >> 7;
    const int maxIPosX = ( itLon + itStepLon * ( n - 1 ) ) >> 7;
    const int maxIPosY = ( itLat + itStepLat * ( n - 1 ) ) >> 7;
    return (    maxIPosX >= m_tileSize.width()  || maxIPosX < 0
             || maxIPosY >= m_tileSize.height() || maxIPosY < 0
             || minIPosX >= m_tileSize.width()  || minIPosX < 0
             || minIPosY >= m_tileSize.height() || minIPosY < 0 );
}


int ScanlineTextureMapperContext::interpolationStep( const ViewportParams *viewport, MapQuality mapQuality )
{
    if ( mapQuality == PrintQuality ) {
        return 1;    // Don't interpolate for print quality.
    }

    if ( ! viewport->globeCoversViewport() ) {
        return 8;
    }

    // Find the optimal interpolation interval m_nBest for the 
    // current image canvas width
    const int width = viewport->width();

    int nBest = 2;
    int nEvalMin = width - 1;
    for ( int it = 1; it < 48; ++it ) {
        int nEval = ( width - 1 ) / it + ( width - 1 ) % it;
        if ( nEval < nEvalMin ) {
            nEvalMin = nEval;
            nBest = it; 
        }
    }

    return nBest;
}


QImage::Format ScanlineTextureMapperContext::optimalCanvasImageFormat( const ViewportParams *viewport )
{
    // If the globe covers fully the screen then we can use the faster
    // RGB32 as there are no translucent areas involved.
    QImage::Format imageFormat = ( viewport->mapCoversViewport() )
                                 ? QImage::Format_RGB32
                                 : QImage::Format_ARGB32_Premultiplied;

    return imageFormat;
}


void ScanlineTextureMapperContext::nextTile( int &posX, int &posY )
{
    // Move from tile coordinates to global texture coordinates 
    // ( with origin in upper left corner, measured in pixel) 

    int lon = posX + m_tilePosX;
    if ( lon >= m_globalWidth )
        lon -= m_globalWidth;
    else if ( lon < 0 )
        lon += m_globalWidth;

    int lat = posY + m_tilePosY;
    if ( lat >= m_globalHeight )
        lat -= m_globalHeight;
    else if ( lat < 0 )
        lat += m_globalHeight;

    // tileCol counts the tile columns left from the current tile.
    // tileRow counts the tile rows on the top from the current tile.

    int tileCol = lon / m_tileSize.width();
    int tileRow = lat / m_tileSize.height();

    m_deltaLevel = 0;
    m_tile = m_tileLoader->loadTile( TileId( 0, m_tileLevel - m_deltaLevel, tileCol >> m_deltaLevel, tileRow >> m_deltaLevel ) );

    // Update position variables:
    // m_tilePosX/Y stores the position of the tiles in 
    // global texture coordinates 
    // ( origin upper left, measured in pixels )

    m_tilePosX = tileCol * m_tileSize.width();
    m_vTileStartX = ( tileCol - ( ( tileCol >> m_deltaLevel ) << m_deltaLevel ) ) * m_tileSize.width();
    m_toTileCoordinatesLon = (qreal)(0.5 * m_globalWidth - m_tilePosX);
    posX = lon - m_tilePosX;

    m_tilePosY = tileRow * m_tileSize.height();
    m_vTileStartY = ( tileRow - ( ( tileRow >> m_deltaLevel ) << m_deltaLevel ) ) * m_tileSize.height();
    m_toTileCoordinatesLat = (qreal)(0.5 * m_globalHeight - m_tilePosY);
    posY = lat - m_tilePosY;
}

void ScanlineTextureMapperContext::nextTile( qreal &posX, qreal &posY )
{
    // Move from tile coordinates to global texture coordinates 
    // ( with origin in upper left corner, measured in pixel) 

    int lon = (int)(posX + m_tilePosX);
    if ( lon >= m_globalWidth )
        lon -= m_globalWidth;
    else if ( lon < 0 )
        lon += m_globalWidth;

    int lat = (int)(posY + m_tilePosY);
    if ( lat >= m_globalHeight )
        lat -= m_globalHeight;
    else if ( lat < 0 )
        lat += m_globalHeight;

    // tileCol counts the tile columns left from the current tile.
    // tileRow counts the tile rows on the top from the current tile.

    int tileCol = lon / m_tileSize.width();
    int tileRow = lat / m_tileSize.height();

    m_deltaLevel = 0;
    m_tile = m_tileLoader->loadTile( TileId( 0, m_tileLevel - m_deltaLevel, tileCol >> m_deltaLevel, tileRow >> m_deltaLevel ) );

    // Update position variables:
    // m_tilePosX/Y stores the position of the tiles in 
    // global texture coordinates 
    // ( origin upper left, measured in pixels )

    m_tilePosX = tileCol * m_tileSize.width();
    m_vTileStartX = ( tileCol - ( ( tileCol >> m_deltaLevel ) << m_deltaLevel ) ) * m_tileSize.width();
    m_toTileCoordinatesLon = (qreal)(0.5 * m_globalWidth - m_tilePosX);
    posX = lon - m_tilePosX;

    m_tilePosY = tileRow * m_tileSize.height();
    m_vTileStartY = ( tileRow - ( ( tileRow >> m_deltaLevel ) << m_deltaLevel ) ) * m_tileSize.height();
    m_toTileCoordinatesLat = (qreal)(0.5 * m_globalHeight - m_tilePosY);
    posY = lat - m_tilePosY;
}
