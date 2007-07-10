
#include"FlatScanlineTextureMapper.h"

#include <cmath>

#include <QtCore/QDebug>

#include "GeoPoint.h"
#include "GeoPolygon.h"
#include "katlasdirs.h"
#include "TextureTile.h"
#include "TileLoader.h"

FlatScanlineTextureMapper::FlatScanlineTextureMapper(const QString& path, QObject * parent ) : AbstractScanlineTextureMapper(path,parent)
{
    m_oldCenterLng = 0.0;
    m_oldYPaintedTop = 0;
}

void FlatScanlineTextureMapper::mapTexture(QImage* canvasImage, const int& radius, 
                                Quaternion& planetAxis)
{
   // Initialize needed variables:
    double lng = 0.0;
    double lat = 0.0;

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    m_fullNormLng = m_fullRangeLng - m_tilePosX;
    m_halfNormLng = m_halfRangeLng - m_tilePosX;
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

    // calculate axis matrix to represent the planet's rotation
    matrix  planetAxisMatrix;
    planetAxis.toMatrix( planetAxisMatrix );

    //Calculate traslation of center point
    float const centerLat=planetAxis.pitch();
    float const centerLng=-planetAxis.yaw();
    int yCenterOffset =  (int)((float)(2*radius / M_PI) * centerLat);

    //Calculate y-range the represented by the center point, yTop and yBottom 
    //and what actually can be painted
    yPaintedTop = yTop = m_imageHalfHeight - radius + yCenterOffset;
    yPaintedBottom = yBottom = m_imageHalfHeight + radius + yCenterOffset;

    if(yPaintedTop < 0) yPaintedTop = 0;
    if(yPaintedTop > m_imageHeight) yPaintedTop = m_imageHeight;
    if(yPaintedBottom < 0) yPaintedBottom = 0;
    if(yPaintedBottom > m_imageHeight) yPaintedBottom = m_imageHeight;

    //Calculate x-range
    xPaintedLeft = 0;
    xLeft = m_imageHalfWidth - 2*radius;
    xPaintedRight = m_imageWidth;
    xRight = m_imageHalfWidth + 2*radius;

    if(xPaintedLeft < 0) xPaintedLeft = 0;
    if(xPaintedLeft > m_imageWidth) xPaintedLeft = m_imageWidth;
    if(xPaintedRight < 0) xPaintedRight = 0;
    if(xPaintedRight > m_imageWidth) xPaintedRight = m_imageWidth;

    //Calculate how many degrees are being represented per pixel
    float xfactor = 2*M_PI/(float)(xRight - xLeft);
    float yfactor = M_PI/(float)(yBottom-yTop);

    float leftLng = - centerLng - (xfactor * m_imageHalfWidth);
    while(leftLng < -M_PI) leftLng += 2*M_PI;
    while(leftLng > M_PI) leftLng -= 2*M_PI;

    //Paint the map
    for(int y=yPaintedTop;y<yPaintedBottom;++y)
    {
        lat = -M_PI/2 + (y - yTop )* yfactor;
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        lng = leftLng;
        for(int x=xPaintedLeft;x<xPaintedRight;++x)
        {
            lng += xfactor;
            if(lng < -M_PI) lng += 2*M_PI;
            if(lng > M_PI) lng -= 2*M_PI;
            pixelValue( lng, lat, m_scanLine + x );
        }
    }

    //remove unused lines
    int clearStart = 0;
    int clearStop = yTop;

    if (yPaintedTop - m_oldYPaintedTop <= 0)
    {
        clearStart=yPaintedBottom;
        clearStop=m_imageHeight;
    }

    for(int y=clearStart;y<clearStop;++y)
    {
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        for(int x=0;x<=m_imageWidth;++x)
        {
            *(m_scanLine +x) = 0;
        }
    }
    m_oldYPaintedTop = yPaintedTop;

    m_tileLoader->cleanupTilehash();
}

#include "FlatScanlineTextureMapper.moc"
