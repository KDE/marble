//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//


// local
#include"EquirectScanlineTextureMapper.h"

// posix
#include <cmath>

// Qt
#include <QtCore/QDebug>

// Marble
#include "GeoDataPoint.h"
#include "GeoPolygon.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "ViewParams.h"


#ifdef Q_CC_MSVC
static double msvc_asinh(double x)
{
  if ( _isnan ( x ) ) {
    errno = EDOM;
    return x;
  }

  return ( log( x + sqrt ( x * x + 1.0 ) ) );
}
#define asinh msvc_asinh
#endif


EquirectScanlineTextureMapper::EquirectScanlineTextureMapper( TileLoader *tileLoader, QObject * parent )
    : AbstractScanlineTextureMapper( tileLoader, parent )
{
    m_oldCenterLon   = 0.0;
    m_oldYPaintedTop = 0;
}


void EquirectScanlineTextureMapper::mapTexture( ViewParams *viewParams )
{
    QImage    *canvasImage = viewParams->canvasImage();
    const int  radius      = viewParams->radius();

   // Initialize needed variables:
    double  lon = 0.0;
    double  lat = 0.0;

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    m_toTileCoordinatesLon = (double)(globalWidth() / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (double)(globalHeight() / 2 - m_tilePosY);

    // Calculate how many degrees are being represented per pixel.
    const float rad2Pixel = M_PI / (float)( 2 * radius );

    int yTop;
    int yPaintedTop;
    int yPaintedBottom;

    // Reset backend
    m_tileLoader->resetTilehash();
    selectTileLevel( viewParams );

    // Calculate translation of center point
    double centerLon, centerLat;
    viewParams->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = 0;
    if ( viewParams->projection() == Equirectangular ) {
        yCenterOffset = (int)( centerLat / rad2Pixel );
    }
    else if ( viewParams->projection() == Mercator ) {
        if ( fabs( centerLat ) < atan( sinh( M_PI ) ) )
            yCenterOffset = (int)( asinh( tan( centerLat ) ) / rad2Pixel  );
        else {
            yCenterOffset = (int)(centerLat / fabs(centerLat)) * 2 * radius;
            qDebug() << "Southpole?" << yCenterOffset * 180.0 / M_PI ;
        }
    }

    // Calculate y-range the represented by the center point, yTop and
    // yBottom and what actually can be painted
    if ( viewParams->projection() == Equirectangular ) {
        yPaintedTop    = yTop = m_imageHeight / 2 - radius + yCenterOffset;
        yPaintedBottom        = m_imageHeight / 2 + radius + yCenterOffset;
    }
    else if ( viewParams->projection() == Mercator ) {
        yPaintedTop    = yTop = m_imageHeight / 2 - 2 * radius + yCenterOffset;
        yPaintedBottom        = m_imageHeight / 2 + 2 * radius + yCenterOffset;
    }

    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > m_imageHeight)    yPaintedTop = m_imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > m_imageHeight) yPaintedBottom = m_imageHeight;

    float leftLon = + centerLon - ( rad2Pixel * m_imageWidth / 2 );
    while ( leftLon < -M_PI ) leftLon += 2 * M_PI;
    while ( leftLon >  M_PI ) leftLon -= 2 * M_PI;

    // Paint the map.
    for ( int y = yPaintedTop ;y < yPaintedBottom; ++y ) {
      
        if ( viewParams->projection() == Equirectangular )
            lat = M_PI/2 - (y - yTop )* rad2Pixel;
        else if ( viewParams->projection() == Mercator )
            lat = atan( sinh( ( (m_imageHeight / 2 + yCenterOffset) - y )
			      / (double)(2 * radius) * M_PI ) );
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        lon = leftLon;

        QRgb * scanLineBegin = m_scanLine;
        const QRgb * scanLineEnd   = m_scanLine + m_imageWidth;

        for ( QRgb * scanLine = scanLineBegin;
	      scanLine < scanLineEnd;
              ++scanLine )
	{
            lon += rad2Pixel;
            if ( lon < -M_PI ) lon += 2 * M_PI;
            if ( lon >  M_PI ) lon -= 2 * M_PI;
            pixelValue( lon, lat, scanLine );
        }
    }

    // Remove unused lines
    const int clearStart = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? yPaintedBottom : 0;
    const int clearStop  = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? m_imageHeight  : yTop;

    for ( int y = clearStart; y < clearStop; ++y ) {
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        for ( int x = 0; x < m_imageWidth; ++x ) {
            *(m_scanLine + x) = 0;
        }
    }
    m_oldYPaintedTop = yPaintedTop;

    m_tileLoader->cleanupTilehash();
}


#include "EquirectScanlineTextureMapper.moc"
