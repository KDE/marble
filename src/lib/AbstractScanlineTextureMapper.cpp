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

#include <QtCore/QDebug>

#include "GeoPolygon.h"
#include "GeoSceneTexture.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "ViewParams.h"

using namespace Marble;

// Defining INTERLACE will make sure that for two subsequent scanlines
// every second scanline will be a deep copy of the first scanline.
// This results in a more coarse resolution and in a speedup for the 
// texture mapping of approx. 25%.



AbstractScanlineTextureMapper::AbstractScanlineTextureMapper( TileLoader *tileLoader, QObject * parent )
    : QObject( parent ),
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
      m_scanLine( 0 ),
      m_tile( 0 ),
      m_tileLevel( 0 ),
      m_maxTileLevel( 0 ),
      m_preloadTileLevel( -1 ),
      m_previousRadius( 0 ),
      m_tilePosX( 0 ),
      m_tilePosY( 0 ),
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

    m_preloadTileLevel = -1;
    m_previousRadius = 0;
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

//    qDebug() << "tileLevelF: " << tileLevelF << " tileLevel: " << tileLevel;

    qreal tileCol = 0.0; 
    qreal tileRow = 0.0;

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
    int tileLevel, qreal& tileCol, qreal& tileRow )
{
    qreal centerLon, centerLat;
    viewParams->centerCoordinates( centerLon, centerLat );

    GeoSceneTexture * texture = static_cast<GeoSceneTexture *>( m_tileLoader->layer()->groundDataset() );

    tileCol = TileLoaderHelper::levelToColumn( 
                                texture->levelZeroColumns(), 
                                tileLevel ) * ( 1.0 + centerLon / M_PI ) / 2.0;

    tileRow = TileLoaderHelper::levelToRow( 
                                texture->levelZeroRows(),
                                tileLevel ) * ( 0.5 - centerLat / M_PI );
}


void AbstractScanlineTextureMapper::tileLevelInit( int tileLevel )
{
    //    qDebug() << "Texture Level was set to: " << tileLevel;
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
}

void AbstractScanlineTextureMapper::pixelValue(qreal lon,
                                               qreal lat, 
                                               QRgb* scanLine,
                                               bool smooth )
{
    // The same method using integers performs about 33% faster.
    // However we need the qreal version to create the high quality mode.

    if ( smooth ) {
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
    else {
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
//    qDebug() << "MAPCHANGED";
    emit mapChanged();
}

void AbstractScanlineTextureMapper::detectMaxTileLevel()
{
    m_maxTileLevel = TileLoader::maxPartialTileLevel( m_tileLoader->layer() ) + 1 ;
//    qDebug() << "MaxTileLevel: " << m_maxTileLevel;
}

#include "AbstractScanlineTextureMapper.moc"
