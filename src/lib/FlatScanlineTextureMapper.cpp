//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//


#include"FlatScanlineTextureMapper.h"

#include <cmath>

#include <QtCore/QDebug>

#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "katlasdirs.h"
#include "TextureTile.h"
#include "TileLoader.h"


FlatScanlineTextureMapper::FlatScanlineTextureMapper(const QString& path, QObject * parent )
    : AbstractScanlineTextureMapper( path, parent )
{
    m_oldCenterLon   = 0.0;
    m_oldYPaintedTop = 0;
}


void FlatScanlineTextureMapper::mapTexture(QImage* canvasImage, 
                                           const int& radius, 
                                           Quaternion& planetAxis)
{
   // Initialize needed variables:
    double lon = 0.0;
    double lat = 0.0;

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    m_fullNormLon = m_fullRangeLon - m_tilePosX;
    m_halfNormLon = m_halfRangeLon - m_tilePosX;
    m_halfNormLat = m_halfRangeLat - m_tilePosY;
    m_quatNormLat = m_quatRangeLat - m_tilePosY;

    int yTop;
    int yBottom;
    int yPaintedTop;
    int yPaintedBottom;
    int xLeft;
    int xRight;
    int xPaintedLeft;
    int xPaintedRight;

    // Reset backend
    m_tileLoader->resetTilehash();
    selectTileLevel(radius);

    // Calculate axis matrix to represent the planet's rotation
    matrix  planetAxisMatrix;
    planetAxis.toMatrix( planetAxisMatrix );

    // Calculate traslation of center point
    float const centerLat =  planetAxis.pitch();
    float const centerLon = -planetAxis.yaw();
    int yCenterOffset =  (int)((float)( 2 * radius / M_PI) * centerLat );

    //Calculate y-range the represented by the center point, yTop and yBottom 
    //and what actually can be painted
    yPaintedTop    = yTop    = m_imageHeight / 2 - radius + yCenterOffset;
    yPaintedBottom = yBottom = m_imageHeight / 2 + radius + yCenterOffset;

    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > m_imageHeight)    yPaintedTop = m_imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > m_imageHeight) yPaintedBottom = m_imageHeight;

    //Calculate x-range
    xPaintedLeft = 0;
    xLeft = m_imageWidth / 2 - 2 * radius;
    xPaintedRight = m_imageWidth;
    xRight = m_imageWidth / 2 + 2 * radius;

    if (xPaintedLeft < 0)             xPaintedLeft  = 0;
    if (xPaintedLeft > m_imageWidth)  xPaintedLeft  = m_imageWidth;
    if (xPaintedRight < 0)            xPaintedRight = 0;
    if (xPaintedRight > m_imageWidth) xPaintedRight = m_imageWidth;

    // Calculate how many degrees are being represented per pixel.
    float xfactor = 2 * M_PI / (float)(xRight - xLeft);
    float yfactor = M_PI / (float)(yBottom-yTop);

    float leftLon = - centerLon - ( xfactor * m_imageWidth / 2 );
    while ( leftLon < -M_PI ) leftLon += 2 * M_PI;
    while ( leftLon > M_PI )  leftLon -= 2 * M_PI;

    // ----------------------------------------------------------------
    // The real beef: Paint the map.
    for ( int y = yPaintedTop ;y < yPaintedBottom; ++y ) {
        lat = -M_PI/2 + (y - yTop )* yfactor;
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        lon = leftLon;
        for ( int x = xPaintedLeft; x < xPaintedRight; ++x ) {
            lon += xfactor;
            if ( lon < -M_PI ) lon += 2 * M_PI;
            if ( lon > M_PI )  lon -= 2 * M_PI;
            pixelValue( lon, lat, m_scanLine + x );
        }
    }

    // Remove unused lines
    int clearStart = 0;
    int clearStop  = yTop;

    if ( yPaintedTop - m_oldYPaintedTop <= 0 ) {
        clearStart = yPaintedBottom;
        clearStop  = m_imageHeight;
    }

    for ( int y = clearStart; y < clearStop; ++y ) {
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        for ( int x = 0; x <= m_imageWidth; ++x ) {
            *(m_scanLine + x) = 0;
        }
    }
    m_oldYPaintedTop = yPaintedTop;

    m_tileLoader->cleanupTilehash();
}


#include "FlatScanlineTextureMapper.moc"
