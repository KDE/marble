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

#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "katlasdirs.h"
#include "TextureTile.h"
#include "TileLoader.h"


// Defining INTERLACE will make sure that for two subsequent scanlines
// every second scanline will be a deep copy of the first scanline.
// This results in a more coarse resolution and in a speedup for the 
// texture mapping of approx. 25%.



AbstractScanlineTextureMapper::AbstractScanlineTextureMapper( const QString& path, QObject * parent )
    :AbstractLayer(parent)
{
    m_posX = 0;
    m_posY = 0;

    m_tileLoader   = new TileLoader( path );
    m_scanLine     = 0;

    connect( m_tileLoader,      SIGNAL( tileUpdateAvailable() ), 
             this,              SLOT( notifyMapChanged() ) );

    detectMaxTileLevel();

    m_imageHalfWidth  = 0;
    m_imageHalfHeight = 0;
    m_imageWidth  = 0;
    m_imageHeight = 0;

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
}


AbstractScanlineTextureMapper::~AbstractScanlineTextureMapper()
{
      m_tileLoader->disconnect();
      delete m_tileLoader;
}


void AbstractScanlineTextureMapper::setMapTheme( const QString& theme )
{
    m_tileLoader->setMapTheme(theme);
    detectMaxTileLevel();
}


void AbstractScanlineTextureMapper::selectTileLevel(int radius)
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


void AbstractScanlineTextureMapper::tileLevelInit( int tileLevel )
{
    int ResolutionX = (int)( 1728000000.0
			     / (double)( TileLoader::levelToColumn( tileLevel ) )
			     / (double)( m_tileLoader->tileWidth() ) );
    int ResolutionY = (int)( 864000000.0
			     / (double)( TileLoader::levelToRow( tileLevel ) )
			     / (double)( m_tileLoader->tileHeight() ) );

    m_rad2PixelX = ( 864000000.0 / M_PI / (double)(ResolutionX) );
    m_rad2PixelY = ( 864000000.0 / M_PI / (double)(ResolutionY) );


    m_fullRangeLng = (int)   ( 1728000000.0 / (double)(ResolutionX) ) - 1;
    m_halfRangeLng = (double)( 864000000.0 / (double)(ResolutionX) );
    m_quatRangeLat = (double)( 432000000.0 / (double)(ResolutionY) );
    m_halfRangeLat = (int)   ( 2.0 * m_quatRangeLat ) - 1;

    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLng = m_halfRangeLng - m_tilePosX;
    m_halfNormLat = m_halfRangeLat - m_tilePosY;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;
}


void AbstractScanlineTextureMapper::resizeMap(int width, int height)
{
    m_imageHeight     = height;
    m_imageWidth      = width;
    // FIXME: These should be removed.  A memory access is much slower than a divide by 2.
    m_imageHalfWidth  = width / 2;
    m_imageHalfHeight = height / 2;

    m_imageRadius     = ( m_imageHalfWidth * m_imageHalfWidth
                          + m_imageHalfHeight * m_imageHalfHeight );
}

void AbstractScanlineTextureMapper::pixelValue(const double& lng, 
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

void AbstractScanlineTextureMapper::nextTile()
{
    // necessary to prevent e.g. crash if lng = -pi

    if ( m_posX > m_fullNormLng ) m_posX = m_fullNormLng;
    if ( m_posY > m_halfNormLat ) m_posY = m_halfNormLat;

    // The origin (0, 0) is in the upper left corner
    // lng: 360 deg = 1728000000 pixel
    // lat: 180 deg = 864000000 pixel

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

    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLng = m_halfRangeLng - m_tilePosX;
    m_posX = lng - m_tilePosX;

    m_tilePosY = tileRow * m_tileLoader->tileHeight();

    m_halfNormLat = m_halfRangeLat - m_tilePosY;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;
    m_posY = lat - m_tilePosY;
}

void AbstractScanlineTextureMapper::notifyMapChanged()
{
    detectMaxTileLevel();
    qDebug() << "MAPCHANGED";
    emit mapChanged();
}

void AbstractScanlineTextureMapper::detectMaxTileLevel()
{
    m_maxTileLevel = TileLoader::maxPartialTileLevel( m_tileLoader->mapTheme() ) + 1 ;
    qDebug() << "MaxTileLevel: " << m_maxTileLevel;
}

#include "AbstractScanlineTextureMapper.moc"
