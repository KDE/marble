//
// C++ Interface: TextureMapper
//
// Description: TextureMapper 

// The TextureMapper maps the Elevationvalues onto the respective projection.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution


#ifndef TEXTUREMAPPER_H
#define TEXTUREMAPPER_H

#include <QtGui/QImage>

#include "Quaternion.h"

/**
@author Torsten Rahn
*/

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

    int   m_posx;
    int   m_posy;

    TileLoader  *m_tileLoader;
    QRgb        *line;
    QRgb        *linefast;

    int          m_maxTileLevel;
    bool         interpolate;
    int          nopt;

    int  x;
    int  y;
    int  z;

    float  qr;
    float  qx;
    float  qy;
    float  qz;

    int    m_imageRadius;
    int    m_imageHalfWidth;
    int    m_imageHalfHeight;

    float  m_prevLat;
    float  m_prevLng;

    int    m_n;
    float  m_ninv;

    int    m_tilxw;
    int    m_tilyh;

    int    maxfullalpha;
    int    maxquatalpha;
    int    maxhalfbeta;
    float  maxhalfalpha;
    float  maxquatbeta;
    int    normfullalpha;
    int    normhalfbeta;
    float  normhalfalpha;
    float  normquatbeta;

    float  m_rad2pixw;
    float  m_rad2pixh;

    TextureTile  *m_tile;

    int    m_tileLevel;
};


#endif
