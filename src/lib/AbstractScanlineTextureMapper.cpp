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

#include <cmath>

#include <QtCore/QDebug>

#include "GeoDataPoint.h"
#include "GeoPolygon.h"
#include "GeoSceneTexture.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "ViewParams.h"


// Defining INTERLACE will make sure that for two subsequent scanlines
// every second scanline will be a deep copy of the first scanline.
// This results in a more coarse resolution and in a speedup for the 
// texture mapping of approx. 25%.



AbstractScanlineTextureMapper::AbstractScanlineTextureMapper( TileLoader *tileLoader, QObject * parent )
    : QObject( parent ),
      m_posX( 0 ),
      m_posY( 0 ),
      m_maxGlobalX( 0 ),
      m_maxGlobalY( 0 ),
      m_imageHeight( 0 ),
      m_imageWidth( 0 ),
      m_imageRadius( 0 ),
      m_prevLat( 0.0 ),
      m_prevLon( 0.0 ),
      m_rad2PixelX( 0.0 ),
      m_rad2PixelY( 0.0 ),
      m_toTileCoordinatesLon( 0.0 ),
      m_toTileCoordinatesLat( 0.0 ),
      m_interlaced( false ),
      m_tileLoader( tileLoader ),
      m_scanLine( 0 ),
      m_tile( 0 ),
      m_tileLevel( 0 ),
      m_maxTileLevel( 0 ),
      m_preloadTileLevel( -1 ),
      m_previousRadius( 0 ),
      m_tilePosX( 0 ),
      m_tilePosY( 0 ),
      m_globalWidth( 0 ),
      m_globalHeight( 0 )
{
    connect( m_tileLoader, SIGNAL( tileUpdateAvailable() ), 
             this,         SLOT( notifyMapChanged() ) );

    detectMaxTileLevel();
}


AbstractScanlineTextureMapper::~AbstractScanlineTextureMapper()
{
      m_tileLoader->disconnect();
//      delete m_tileLoader;
}


void AbstractScanlineTextureMapper::setTextureLayer( GeoSceneTexture *textureLayer )
{
    m_tileLoader->setTextureLayer( textureLayer );
    m_tileLevel = -1;
    detectMaxTileLevel();

    m_preloadTileLevel = -1;
    m_previousRadius = 0;
}


void AbstractScanlineTextureMapper::selectTileLevel( ViewParams* viewParams )
{
    const int radius = viewParams->radius();

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)

    double  linearLevel = ( 2.0 * (double)( radius )
			    / (double) ( m_tileLoader->tileWidth() ) );
    int     tileLevel   = 0;

    if ( linearLevel < 1.0 )
        linearLevel = 1.0; // Dirty fix for invalid entry linearLevel

    double tileLevelF = log( linearLevel ) / log( 2.0 ) + 1.0;
    tileLevel = (int)( tileLevelF );

//    qDebug() << "tileLevelF: " << tileLevelF << " tileLevel: " << tileLevel;

    double tileCol = 0.0; 
    double tileRow = 0.0;

    if (    tileLevelF > tileLevel + 0.3 
         && m_preloadTileLevel != tileLevel + 1
         && m_previousRadius < radius 
         && tileLevel > 0 && tileLevel < m_maxTileLevel ) {

        m_preloadTileLevel = tileLevel + 1;

        centerTiles( viewParams, m_preloadTileLevel, tileCol, tileRow );
//        qDebug() << "Preload tileLevel: " << m_preloadTileLevel
//        << " tileCol: " << tileCol << " tileRow: " << tileRow;
    }
    if (    tileLevelF < tileLevel + 0.7 
         && m_preloadTileLevel != tileLevel - 1
         && m_previousRadius > radius 
         && tileLevel > 1 && tileLevel < m_maxTileLevel + 1 ) {

        m_preloadTileLevel = tileLevel - 1;

        centerTiles( viewParams, m_preloadTileLevel, tileCol, tileRow );
//        qDebug() << "Preload tileLevel: " << m_preloadTileLevel
//        << " tileCol: " << tileCol << " tileRow: " << tileRow;
    }
    if ( m_previousRadius == radius ) m_preloadTileLevel = -1;
    else m_previousRadius = radius;

    if ( tileLevel > m_maxTileLevel )
        tileLevel = m_maxTileLevel;

    if ( tileLevel != m_tileLevel ) {
        m_tileLoader->flush();
        tileLevelInit( tileLevel );
    }
}


void AbstractScanlineTextureMapper::centerTiles( ViewParams *viewParams, 
    const int tileLevel, double& tileCol, double& tileRow )
{
    double centerLon, centerLat;
    viewParams->centerCoordinates( centerLon, centerLat );

    tileCol = TileLoaderHelper::levelToColumn( m_tileLoader->textureLayer()->levelZeroColumns(),
                                               tileLevel )
              * ( 1.0 + centerLon / M_PI ) / 2.0;

    tileRow = TileLoaderHelper::levelToRow( m_tileLoader->textureLayer()->levelZeroRows(),
                                            tileLevel )
              * ( 0.5 - centerLat / M_PI );
}


void AbstractScanlineTextureMapper::tileLevelInit( int tileLevel )
{
    //    qDebug() << "Texture Level was set to: " << tileLevel;
    m_tileLevel = tileLevel;

    m_globalWidth = m_tileLoader->globalWidth( m_tileLevel );
    m_globalHeight = m_tileLoader->globalHeight( m_tileLevel );

    // rad2PixelY might later on evolve into a method to allow 
    // Mercator as a source texture format. That's why we have it
    // in addition to rad2PixelX.
    m_rad2PixelX = +(double)(m_globalWidth)  / (2.0 * M_PI);
    m_rad2PixelY = -(double)(m_globalHeight) / M_PI;

    m_maxGlobalX = m_globalWidth  - 1;
    m_maxGlobalY = m_globalHeight - 1;

    // These variables move the origin of global texture coordinates from 
    // the center to the upper left corner and subtract the tile position 
    // in that coordinate system. In total this equals a coordinate 
    // transformation to tile coordinates.
  
    m_toTileCoordinatesLon = (double)(m_globalWidth / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (double)(m_globalHeight / 2 - m_tilePosY);
}


void AbstractScanlineTextureMapper::resizeMap(int width, int height)
{
    m_imageHeight = height;
    m_imageWidth  = width;

    m_imageRadius = ( m_imageWidth * m_imageWidth / 4
                      + m_imageHeight * m_imageHeight / 4 );
}

void AbstractScanlineTextureMapper::pixelValue(const double& lon,
                                               const double& lat, 
                                               QRgb* scanLine)
{
    // Convert the lon and lat coordinates of the position on the scanline
    // measured in radian to the pixel position of the requested 
    // coordinate on the current tile.

//     qDebug() << "AbstractScanlineTextureMapper::pixelValue 1";
    
    m_posX = (int)( m_toTileCoordinatesLon + lon * m_rad2PixelX );
    m_posY = (int)( m_toTileCoordinatesLat + lat * m_rad2PixelY );

    // Most of the time while moving along the scanLine we'll stay on the 
    // same tile. However at the tile border we might "fall off". If that 
    // happens we need to find out the next tile that needs to be loaded.

    if ( m_posX  >= m_tileLoader->tileWidth() 
         || m_posX < 0
         || m_posY >= m_tileLoader->tileHeight()
         || m_posY < 0 )
    {
        nextTile();
    }

    *scanLine = m_tile->pixel( m_posX, m_posY ); 
}

void AbstractScanlineTextureMapper::nextTile()
{
    // Move from tile coordinates to global texture coordinates 
    // ( with origin in upper left corner, measured in pixel) 

    int lon = m_posX + m_tilePosX;
    if ( lon > m_maxGlobalX ) lon -= m_maxGlobalX;
    if ( lon < 0 ) lon += m_maxGlobalX;

    int lat = m_posY + m_tilePosY;
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
    m_toTileCoordinatesLon = (double)(m_globalWidth / 2 - m_tilePosX);
    m_posX = lon - m_tilePosX;

    m_tilePosY = tileRow * m_tileLoader->tileHeight();
    m_toTileCoordinatesLat = (double)(m_globalHeight / 2 - m_tilePosY);
    m_posY = lat - m_tilePosY;
}

void AbstractScanlineTextureMapper::notifyMapChanged()
{
    detectMaxTileLevel();
//    qDebug() << "MAPCHANGED";
    emit mapChanged();
}

void AbstractScanlineTextureMapper::detectMaxTileLevel()
{
    m_maxTileLevel = TileLoader::maxPartialTileLevel( m_tileLoader->textureLayer() ) + 1 ;
    qDebug() << "MaxTileLevel: " << m_maxTileLevel;
}

#include "AbstractScanlineTextureMapper.moc"
