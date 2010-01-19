//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//

#include "AbstractScanlineTextureMapper.h"

#include "MarbleDebug.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "ViewParams.h"
#include "ViewportParams.h"

using namespace Marble;

AbstractScanlineTextureMapper::AbstractScanlineTextureMapper( TileLoader *tileLoader, QObject * parent )
    : QObject( parent ),
      m_interpolate( false ),
      m_maxGlobalX( 0 ),
      m_maxGlobalY( 0 ),
      m_imageHeight( 0 ),
      m_imageWidth( 0 ),
      m_imageRadius( 0 ),
      m_prevLat( 0.0 ),
      m_prevLon( 0.0 ),
      m_toTileCoordinatesLon( 0.0 ),
      m_toTileCoordinatesLat( 0.0 ),
      m_interlaced( false ),
      m_tileLoader( tileLoader ),
      m_tile( 0 ),
      m_tileLevel( 0 ),
      m_maxTileLevel( 0 ),
      m_tilePosX( 0 ),
      m_tilePosY( 0 ),
      m_nBest( 0 ),
      m_globalWidth( 0 ),
      m_globalHeight( 0 ),
      m_normGlobalWidth( 0.0 ),
      m_normGlobalHeight( 0.0 )
{
    GeoSceneTexture * texture = 0;

    if ( tileLoader ) {
        GeoSceneLayer * layer = tileLoader->layer();
        if ( layer ) {
            texture = static_cast<GeoSceneTexture *>( layer->groundDataset() );
        }
    }

    m_tileProjection = tileLoader && texture
                        ? texture->projection()
                        : GeoSceneTexture::Equirectangular;

    connect( m_tileLoader, SIGNAL( tileUpdateAvailable() ), 
             this,         SLOT( notifyMapChanged() ) );

    detectMaxTileLevel();
}


AbstractScanlineTextureMapper::~AbstractScanlineTextureMapper()
{
      m_tileLoader->disconnect();
//      delete m_tileLoader;
}


void AbstractScanlineTextureMapper::setLayer( GeoSceneLayer * layer )
{
    m_tileLoader->setLayer( layer );
    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( layer->groundDataset() );
    m_tileProjection = texture->projection();
    m_tileLevel = -1;
    detectMaxTileLevel();
}


void AbstractScanlineTextureMapper::selectTileLevel( ViewParams* viewParams )
{
    const int radius = viewParams->radius();

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)

    qreal  linearLevel = ( 2.0 * (qreal)( radius )
			    / (qreal) ( m_tileLoader->tileWidth() ) );
    int     tileLevel   = 0;

    if ( linearLevel < 1.0 )
        linearLevel = 1.0; // Dirty fix for invalid entry linearLevel

    qreal tileLevelF = log( linearLevel ) / log( 2.0 ) + 1.0;
    tileLevel = (int)( tileLevelF );

//    mDebug() << "tileLevelF: " << tileLevelF << " tileLevel: " << tileLevel;

    if ( tileLevel > m_maxTileLevel )
        tileLevel = m_maxTileLevel;

    if ( tileLevel != m_tileLevel ) {
        m_tileLoader->flush();
        tileLevelInit( tileLevel );
    }
}


void AbstractScanlineTextureMapper::tileLevelInit( int tileLevel )
{
    //    mDebug() << "Texture Level was set to: " << tileLevel;
    m_tileLevel = tileLevel;

    m_globalWidth = m_tileLoader->globalWidth( m_tileLevel );
    m_normGlobalWidth = (qreal)( m_globalWidth / ( 2 * M_PI ) );
    m_globalHeight = m_tileLoader->globalHeight( m_tileLevel );
    m_normGlobalHeight = (qreal)( m_globalHeight /  M_PI );

    m_maxGlobalX = m_globalWidth  - 1;
    m_maxGlobalY = m_globalHeight - 1;

    // These variables move the origin of global texture coordinates from 
    // the center to the upper left corner and subtract the tile position 
    // in that coordinate system. In total this equals a coordinate 
    // transformation to tile coordinates.
  
    m_toTileCoordinatesLon = (qreal)(m_globalWidth / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (qreal)(m_globalHeight / 2 - m_tilePosY);
}


void AbstractScanlineTextureMapper::resizeMap(int width, int height)
{
    m_imageHeight = height;
    m_imageWidth  = width;

    m_imageRadius = ( m_imageWidth * m_imageWidth / 4
                      + m_imageHeight * m_imageHeight / 4 );

    // Find the optimal interpolation interval m_nBest for the 
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
}

void AbstractScanlineTextureMapper::pixelValueF(qreal lon,
                                               qreal lat, 
                                               QRgb* scanLine )
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
    
        if ( posX  >= (qreal)( m_tileLoader->tileWidth() ) 
             || posX < 0.0
             || posY >= (qreal)( m_tileLoader->tileHeight() )
             || posY < 0.0 )
        {
            nextTile( posX, posY );
        }
        if (m_tile) {
            *scanLine = m_tile->pixelF( posX, posY );
        }
        else {
            *scanLine = 0;
        }
}

void AbstractScanlineTextureMapper::pixelValue(qreal lon,
                                               qreal lat, 
                                               QRgb* scanLine )
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
    
        if ( iPosX  >= m_tileLoader->tileWidth() 
            || iPosX < 0
            || iPosY >= m_tileLoader->tileHeight()
            || iPosY < 0 )
        {
            nextTile( iPosX, iPosY );
        }

        if ( m_tile ) {
            *scanLine = m_tile->pixel( iPosX, iPosY );
        }
        else {
            *scanLine = 0;
        }
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

void AbstractScanlineTextureMapper::pixelValueApproxF(const qreal& lon,
                              const qreal& lat, QRgb *scanLine,
                              int n )
{
    // stepLon/Lat: Distance between two subsequent approximated positions

    qreal stepLat = lat - m_prevLat;
    qreal stepLon = lon - m_prevLon;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    const qreal nInverse = 1.0 / (qreal)(n);

    if ( fabs(stepLon) < M_PI ) {
            m_prevLon = rad2PixelX( m_prevLon );
            m_prevLat = rad2PixelY( m_prevLat );

            const qreal itStepLon = ( rad2PixelX( lon ) - m_prevLon ) * nInverse;
            const qreal itStepLat = ( rad2PixelY( lat ) - m_prevLat ) * nInverse;
        
            // To improve speed we unroll 
            // AbstractScanlineTextureMapper::pixelValue(...) here and 
            // calculate the performance critical issues via integers
    
            qreal itLon = m_prevLon + m_toTileCoordinatesLon;
            qreal itLat = m_prevLat + m_toTileCoordinatesLat;

            const int tileWidth = m_tileLoader->tileWidth();
            const int tileHeight = m_tileLoader->tileHeight();

/*
            int oldR = 0;
            int oldG = 0;
            int oldB = 0;
*/
            QRgb oldRgb = qRgb( 0, 0, 0 );

            qreal oldPosX = -1;
            qreal oldPosY = 0;

            for ( int j=1; j < n; ++j ) {
                qreal posX = itLon + itStepLon * j;
                qreal posY = itLat + itStepLat * j;
    
                if ( posX >= tileWidth 
                     || posX < 0.0
                     || posY >= tileHeight
                     || posY < 0.0 )
                {
                    nextTile( posX, posY );
                    itLon = m_prevLon + m_toTileCoordinatesLon;
                    itLat = m_prevLat + m_toTileCoordinatesLat;
                    posX = itLon + itStepLon * j;
                    posY = itLat + itStepLat * j;
                    oldPosX = -1;
                }
    
                *scanLine = m_tile->pixel( posX, posY ); 

                // Just perform bilinear interpolation if there's a color change compared to the 
                // last pixel that was evaluated. This speeds up things greatly for maps like OSM
                if ( *scanLine != oldRgb ) {
                    if ( oldPosX != -1 ) {
                        *(scanLine - 1) = m_tile->pixelF( oldPosX, oldPosY, *(scanLine - 1) );
                        oldPosX = -1;
                    }
                    oldRgb = m_tile->pixelF( posX, posY, *scanLine );
                    *scanLine = oldRgb;
                }
                else {
                    oldPosX = posX;
                    oldPosY = posY;
                }
                
/*
                if ( needsFilter( *scanLine, oldR, oldB, oldG  ) ) {
                    *scanLine = m_tile->pixelF( posX, posY );
                }
*/
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

void AbstractScanlineTextureMapper::pixelValueApprox(const qreal& lon,
                              const qreal& lat, QRgb *scanLine,
                              int n )
{
    // stepLon/Lat: Distance between two subsequent approximated positions

    qreal stepLat = lat - m_prevLat;
    qreal stepLon = lon - m_prevLon;

    // As long as the distance is smaller than 180 deg we can assume that 
    // we didn't cross the dateline.

    const qreal nInverse = 1.0 / (qreal)(n);

    if ( fabs(stepLon) < M_PI ) {
            m_prevLon = rad2PixelX( m_prevLon );
            m_prevLat = rad2PixelY( m_prevLat );

            const int itStepLon = (int)( ( rad2PixelX( lon ) - m_prevLon ) * nInverse * 128.0 );
            const int itStepLat = (int)( ( rad2PixelY( lat ) - m_prevLat ) * nInverse * 128.0 );
        
            // To improve speed we unroll 
            // AbstractScanlineTextureMapper::pixelValue(...) here and 
            // calculate the performance critical issues via integers
    
            int itLon = (int)( ( m_prevLon + m_toTileCoordinatesLon ) * 128.0 );
            int itLat = (int)( ( m_prevLat + m_toTileCoordinatesLat ) * 128.0 );

            const int tileWidth = m_tileLoader->tileWidth();
            const int tileHeight = m_tileLoader->tileHeight();

            for ( int j = 1; j < n; ++j ) {
                int iPosX = ( itLon + itStepLon * j ) >> 7;
                int iPosY = ( itLat + itStepLat * j ) >> 7;
    
                if ( iPosX >= tileWidth 
                     || iPosX < 0
                     || iPosY >= tileHeight
                     || iPosY < 0 )
                {
                    nextTile( iPosX, iPosY );
                    itLon = (int)( ( m_prevLon + m_toTileCoordinatesLon ) * 128.0 );
                    itLat = (int)( ( m_prevLat + m_toTileCoordinatesLat ) * 128.0 );
                    iPosX = ( itLon + itStepLon * j ) >> 7;
                    iPosY = ( itLat + itStepLat * j ) >> 7;
                }

                *scanLine = m_tile->pixel( iPosX, iPosY ); 
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

int AbstractScanlineTextureMapper::interpolationStep( ViewParams *viewParams ) const
{
    if ( viewParams->mapQuality() == PrintQuality ) {
        return 1;    // Don't interpolate for print quality.
    }

    if ( ! viewParams->viewport()->globeCoversViewport() ) {
        return 8;
    }

    return m_nBest;
}


void AbstractScanlineTextureMapper::nextTile( int &posX, int &posY )
{
    // Move from tile coordinates to global texture coordinates 
    // ( with origin in upper left corner, measured in pixel) 

    int lon = posX + m_tilePosX;
    if ( lon > m_maxGlobalX ) lon -= m_maxGlobalX;
    if ( lon < 0 ) lon += m_maxGlobalX;

    int lat = posY + m_tilePosY;
    if ( lat > m_maxGlobalY ) lat -= m_maxGlobalY;
    if ( lat < 0 ) lat += m_maxGlobalY;

    // tileCol counts the tile columns left from the current tile.
    // tileRow counts the tile rows on the top from the current tile.

    int tileCol = lon / m_tileLoader->tileWidth();
    int tileRow = lat / m_tileLoader->tileHeight();

    m_tile = m_tileLoader->loadTile( tileCol, tileRow, m_tileLevel );

    // Update position variables:
    // m_tilePosX/Y stores the position of the tiles in 
    // global texture coordinates 
    // ( origin upper left, measured in pixels )

    m_tilePosX = tileCol * m_tileLoader->tileWidth();
    m_toTileCoordinatesLon = (qreal)(m_globalWidth / 2 - m_tilePosX);
    posX = lon - m_tilePosX;

    m_tilePosY = tileRow * m_tileLoader->tileHeight();
    m_toTileCoordinatesLat = (qreal)(m_globalHeight / 2 - m_tilePosY);
    posY = lat - m_tilePosY;
}

void AbstractScanlineTextureMapper::nextTile( qreal &posX, qreal &posY )
{
    // Move from tile coordinates to global texture coordinates 
    // ( with origin in upper left corner, measured in pixel) 

    int lon = (int)(posX + m_tilePosX);
    if ( lon > m_maxGlobalX ) lon -= m_maxGlobalX;
    if ( lon < 0 ) lon += m_maxGlobalX;

    int lat = (int)(posY + m_tilePosY);
    if ( lat > m_maxGlobalY ) lat -= m_maxGlobalY;
    if ( lat < 0 ) lat += m_maxGlobalY;

    // tileCol counts the tile columns left from the current tile.
    // tileRow counts the tile rows on the top from the current tile.

    int tileCol = lon / m_tileLoader->tileWidth();
    int tileRow = lat / m_tileLoader->tileHeight();

    m_tile = m_tileLoader->loadTile( tileCol, tileRow, m_tileLevel );

    // Update position variables:
    // m_tilePosX/Y stores the position of the tiles in 
    // global texture coordinates 
    // ( origin upper left, measured in pixels )

    m_tilePosX = tileCol * m_tileLoader->tileWidth();
    m_toTileCoordinatesLon = (qreal)(m_globalWidth / 2 - m_tilePosX);
    posX = lon - m_tilePosX;

    m_tilePosY = tileRow * m_tileLoader->tileHeight();
    m_toTileCoordinatesLat = (qreal)(m_globalHeight / 2 - m_tilePosY);
    posY = lat - m_tilePosY;
}

void AbstractScanlineTextureMapper::notifyMapChanged()
{
    detectMaxTileLevel();
//    mDebug() << "MAPCHANGED";
    emit mapChanged();
}

void AbstractScanlineTextureMapper::detectMaxTileLevel()
{
    m_maxTileLevel = TileLoader::maxPartialTileLevel( m_tileLoader->layer() ) + 1 ;
//    mDebug() << "MaxTileLevel: " << m_maxTileLevel;
}

#include "AbstractScanlineTextureMapper.moc"
