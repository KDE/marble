//
// C++ Interface: TextureMapper
//
// Description: TextureMapper 

// The TextureMapper maps the Elevationvalues onto the respective projection.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution


#ifndef __MARBLE__TEXTUREMAPPER_H
#define __MARBLE__TEXTUREMAPPER_H

#include "Quaternion.h"

/**
@author Torsten Rahn
*/

class QImage;
class TextureTile;
class TileLoader;

class TextureMapper
{
 public:
    TextureMapper( const QString& path );
    virtual ~TextureMapper();

    void setMap( const QString& path );
    void setMaxTileLevel( int level ){ m_maxTileLevel = level; }
    void resizeMap(const QImage*);
    void mapTexture(QImage*, const int&, Quaternion&);
    void selectTileLevel(const int& radius);

 protected:
    void getPixelValueApprox(const float&, const float&, QRgb*);
    void getPixelValue(const float&, const float&, QRgb*);

    void tileLevelInit( int tileLevel );

    int   m_posX;
    int   m_posY;

    TileLoader  *m_tileLoader;
    QRgb        *scanLine;
    QRgb        *fastScanLine;

    int          m_maxTileLevel;
    bool         interpolate;
    int          nBest;

    int    m_n;
    float  m_ninv;

    int    x;
    int    y;
    int    z;

    float  qr;
    float  qx;
    float  qy;
    float  qz;

    int    m_imageHalfWidth;
    int    m_imageHalfHeight;
    int    m_imageRadius;

    float  m_prevLat;
    float  m_prevLng;

    int    m_tilePosX;
    int    m_tilePosY;

    int    m_fullRangeLng;
    int    m_halfRangeLat;
    float  m_halfRangeLng;
    float  m_quatRangeLat;

    int    m_fullNormLng;
    int    m_halfNormLat;
    float  m_halfNormLng;
    float  m_quatNormLat;

    float  m_rad2PixelX;
    float  m_rad2PixelY;

    TextureTile  *m_tile;

    int    m_tileLevel;
};


#endif // __MARBLE__TEXTUREMAPPER_H
