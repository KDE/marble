//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2008      Inge Wallin      <inge@lysator.liu.se>
//


// local
#include"MercatorScanlineTextureMapper.h"

// posix
#include <cmath>

// Qt
#include <QtCore/QDebug>
#include <QtGui/QImage>

// Marble
#include "GeoPolygon.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoader.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "AbstractProjection.h"
#include "MathHelper.h"

using namespace Marble;

MercatorScanlineTextureMapper::MercatorScanlineTextureMapper( TileLoader *tileLoader,
                                                              QObject * parent )
    : AbstractScanlineTextureMapper( tileLoader, parent )
{
    m_oldCenterLon   = 0.0;
    m_oldYPaintedTop = 0;
}


void MercatorScanlineTextureMapper::mapTexture( ViewParams *viewParams )
{
    QImage    *canvasImage = viewParams->canvasImage();
    const int  radius      = viewParams->radius();

    const bool isHighQuality  = ( viewParams->mapQuality() == Marble::High
				  || viewParams->mapQuality() == Marble::Print );

   // Initialize needed variables:
    qreal  lon = 0.0;
    qreal  lat = 0.0;

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    m_toTileCoordinatesLon = (qreal)(globalWidth() / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (qreal)(globalHeight() / 2 - m_tilePosY);

    // Calculate how many pixel are being represented per radians.
    const float rad2Pixel = (float)( 2 * radius )/M_PI;

    int yTop;
    int yPaintedTop;
    int yPaintedBottom;

    // Reset backend
    m_tileLoader->resetTilehash();
    selectTileLevel( viewParams );

    // Calculate translation of center point
    qreal centerLon, centerLat;
    viewParams->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = 0;

    yCenterOffset = (int)( asinh( tan( centerLat ) ) * rad2Pixel  );

    // Calculate y-range the represented by the center point, yTop and
    // yBottom and what actually can be painted
    yPaintedTop    = yTop = m_imageHeight / 2 - 2 * radius + yCenterOffset;
    yPaintedBottom        = m_imageHeight / 2 + 2 * radius + yCenterOffset;

    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > m_imageHeight)    yPaintedTop = m_imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > m_imageHeight) yPaintedBottom = m_imageHeight;

    float leftLon = + centerLon - ( m_imageWidth / 2 / rad2Pixel );
    while ( leftLon < -M_PI ) leftLon += 2 * M_PI;
    while ( leftLon >  M_PI ) leftLon -= 2 * M_PI;

    const qreal pixel2Rad = 1.0/rad2Pixel;

    // Paint the map.
    for ( int y = yPaintedTop ;y < yPaintedBottom; ++y ) {        
        lat = atan( sinh( ( (m_imageHeight / 2 + yCenterOffset) - y )
                    * pixel2Rad ) );
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        lon = leftLon;
    
        QRgb * scanLineBegin = m_scanLine;
        const QRgb * scanLineEnd   = m_scanLine + m_imageWidth;
    
        for ( QRgb * scanLine = scanLineBegin;
                    scanLine < scanLineEnd;
                    ++scanLine )
        {
                lon += pixel2Rad;
                if ( lon < -M_PI ) lon += 2 * M_PI;
                if ( lon >  M_PI ) lon -= 2 * M_PI;
                pixelValue( lon, lat, scanLine, isHighQuality );
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


#include "MercatorScanlineTextureMapper.moc"
