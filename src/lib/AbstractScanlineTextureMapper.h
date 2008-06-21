//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//


#ifndef ABSTRACTSCANLINETEXTUREMAPPER_H
#define ABSTRACTSCANLINETEXTUREMAPPER_H

#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtGui/QColor>

#include <cmath>
#include <math.h>
#ifdef _MSC_VER
#include <msvc/math.h>
#endif

#include "TileLoader.h"
#include "TextureTile.h"
#include "GeoSceneTexture.h"

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

#ifdef Q_CC_MSVC
static double msvc_atanh(double x)
{
  return ( 0.5 * log( ( 1.0 + x ) / ( 1.0 - x ) ) );
}
#define atanh msvc_atanh
#endif

class TextureTile;
class TileLoader;
class ViewParams;


class AbstractScanlineTextureMapper : public QObject
{
    Q_OBJECT

public:
    explicit AbstractScanlineTextureMapper( TileLoader *tileLoader, QObject * parent=0 );
    ~AbstractScanlineTextureMapper();

    virtual void mapTexture( ViewParams *viewParams ) = 0;

    void setTextureLayer( GeoSceneTexture *textureLayer );
    void setMaxTileLevel( int level );
    virtual void resizeMap( int width, int height );
    void selectTileLevel( ViewParams* viewParams );
    bool interlaced() const;
    void setInterlaced( bool enabled );

    void centerTiles( ViewParams *viewParams, const int tileLevel,
                      double& tileCol, double& tileRow );

 Q_SIGNALS:
    void mapChanged();

 private Q_SLOTS:
    void notifyMapChanged();

 protected:
    void pixelValue( const double& lon, const double& lat, 
                     QRgb* scanLine, bool smooth = false );

    // method for fast integer calculation
    void nextTile( int& posx, int& posy );

    // method for precise interpolation
    void nextTile( double& posx, double& posy );

    void detectMaxTileLevel();
    void tileLevelInit( int tileLevel );

    int globalWidth() const;
    int globalHeight() const;

    // Converts Radian to global texture coordinates 
    // ( with origin in center, measured in pixel) 
    double rad2PixelX( const double longitude ) const;
    double rad2PixelY( const double latitude ) const;

    QRgb bilinearSmooth( const QRgb& topLeftValue ) const;

    // Coordinates on the tile for fast integer calculation
    int        m_iPosX;
    int        m_iPosY;
    // Coordinates on the tile for precise interpolation
    double     m_posX;
    double     m_posY;

    // maximum values for global texture coordinates
    // ( with origin in upper left corner, measured in pixel) 
    int     m_maxGlobalX;
    int     m_maxGlobalY;

    int     m_imageHeight;
    int     m_imageWidth;
    int     m_imageRadius;

    // Previous coordinates
    double  m_prevLat;
    double  m_prevLon;

    // Coordinate transformations:

    // Converts global texture coordinates 
    // ( with origin in center, measured in pixel) 
    // to tile coordinates ( measured in pixel )
    double  m_toTileCoordinatesLon;
    double  m_toTileCoordinatesLat;

    bool m_interlaced;

    // ------------------------
    // Tile stuff
    TileLoader  *m_tileLoader;
    GeoSceneTexture::Projection m_tileProjection;
    QRgb        *m_scanLine;


    TextureTile *m_tile;

    int          m_tileLevel;
    int          m_maxTileLevel;

    int          m_preloadTileLevel;
    int          m_previousRadius;

    // Position of the tile in global Texture Coordinates
    // ( with origin in upper left corner, measured in pixel) 
    int          m_tilePosX;
    int          m_tilePosY;

 private:
    Q_DISABLE_COPY( AbstractScanlineTextureMapper )
    int          m_globalWidth;
    int          m_globalHeight;
    double       m_normGlobalWidth;
    double       m_normGlobalHeight;
};

inline void AbstractScanlineTextureMapper::setMaxTileLevel( int level )
{
    m_maxTileLevel = level;
}

inline bool AbstractScanlineTextureMapper::interlaced() const
{
    return m_interlaced;
}

inline void AbstractScanlineTextureMapper::setInterlaced( bool enabled )
{
    m_interlaced = enabled;
}

inline int AbstractScanlineTextureMapper::globalWidth() const
{
    return m_globalWidth;
}

inline int AbstractScanlineTextureMapper::globalHeight() const
{
    return m_globalHeight;
}

inline double AbstractScanlineTextureMapper::rad2PixelX( const double longitude ) const
{
    return longitude * m_normGlobalWidth;
}

inline double AbstractScanlineTextureMapper::rad2PixelY( const double latitude ) const
{
    switch ( m_tileProjection ) {
    case GeoSceneTexture::Equirectangular:
        return -latitude * m_normGlobalHeight;
    case GeoSceneTexture::Mercator:
        if ( fabs( latitude ) < 1.4835 )
            return - asinh( tan( latitude ) ) * 0.5 * m_normGlobalHeight;
        if ( latitude >= +1.4835 )
            // asinh( tan (1.4835)) => 3.1309587
            return - 3.1309587 * 0.5 * m_normGlobalHeight; 
        if ( latitude <= -1.4835 )
            // asinh( tan( -1.4835 )) => −3.1309587
            return 3.1309587 * 0.5 * m_normGlobalHeight; 
    }

    // Dummy value to avoid a warning.
    return 0.0;
}


inline QRgb AbstractScanlineTextureMapper::bilinearSmooth( const QRgb& topLeftValue ) const
{
    double fY = m_posY - (int)(m_posY);

    // Interpolation in y-direction
    if ( ( m_posY + 1.0 ) < m_tileLoader->tileHeight() ) {

        QRgb bottomLeftValue  =  m_tile->pixel( (int)(m_posX), (int)(m_posY + 1.0) );

        // blending the color values of the top left and bottom left point
        int ml_red   = (int)( ( 1.0 - fY ) * qRed  ( topLeftValue  ) + fY * qRed  ( bottomLeftValue  ) );
        int ml_green = (int)( ( 1.0 - fY ) * qGreen( topLeftValue  ) + fY * qGreen( bottomLeftValue  ) );
        int ml_blue  = (int)( ( 1.0 - fY ) * qBlue ( topLeftValue  ) + fY * qBlue ( bottomLeftValue  ) );

        // Interpolation in x-direction
        if ( ( m_posX + 1.0 ) < m_tileLoader->tileWidth() ) {

            double fX = m_posX - (int)(m_posX);

            QRgb topRightValue    =  m_tile->pixel( (int)(m_posX + 1.0), (int)(m_posY      ) );
            QRgb bottomRightValue =  m_tile->pixel( (int)(m_posX + 1.0), (int)(m_posY + 1.0) );

            // blending the color values of the top right and bottom right point
            int mr_red   = (int)( ( 1.0 - fY ) * qRed  ( topRightValue ) + fY * qRed  ( bottomRightValue ) );
            int mr_green = (int)( ( 1.0 - fY ) * qGreen( topRightValue ) + fY * qGreen( bottomRightValue ) );
            int mr_blue  = (int)( ( 1.0 - fY ) * qBlue ( topRightValue ) + fY * qBlue ( bottomRightValue ) );
    
            // blending the color values of the resulting middle left 
            // and middle right points
            int mm_red   = (int)( ( 1.0 - fX ) * ml_red   + fX * mr_red   );
            int mm_green = (int)( ( 1.0 - fX ) * ml_green + fX * mr_green );
            int mm_blue  = (int)( ( 1.0 - fX ) * ml_blue  + fX * mr_blue  );
    
            return qRgb( mm_red, mm_green, mm_blue );
        }
        else {
            return qRgb( ml_red, ml_green, ml_blue );
        }
    }
    else {
        // Interpolation in x-direction
        if ( ( m_posX + 1.0 ) < m_tileLoader->tileWidth() ) {

            double fX = m_posX - (int)(m_posX);

            if ( fX == 0.0 ) 
                return topLeftValue;

            QRgb topRightValue    =  m_tile->pixel( (int)( m_posX + 1 ), (int)( m_posY     ) );

            // blending the color values of the top left and top right point
            int tm_red   = (int)( ( 1.0 - fX ) * qRed  ( topLeftValue ) + fX * qRed  ( topRightValue ) );
            int tm_green = (int)( ( 1.0 - fX ) * qGreen( topLeftValue ) + fX * qGreen( topRightValue ) );
            int tm_blue  = (int)( ( 1.0 - fX ) * qBlue ( topLeftValue ) + fX * qBlue ( topRightValue ) );

            return qRgb( tm_red, tm_green, tm_blue );
        }
    }

    return topLeftValue;
}

#endif
