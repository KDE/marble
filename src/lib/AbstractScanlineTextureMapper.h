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

#include <QtCore/QObject>
#include <QtGui/QColor>

#include <cmath>
#include <math.h>

#include "MarbleMath.h"
#include "TileLoader.h"
#include "TextureTile.h"
#include "GeoSceneTexture.h"
#include "MathHelper.h"


namespace Marble
{

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

    void setLayer( GeoSceneLayer * layer );
    void setMaxTileLevel( int level );
    virtual void resizeMap( int width, int height );
    void selectTileLevel( ViewParams* viewParams );
    bool interlaced() const;
    void setInterlaced( bool enabled );

    void centerTiles( ViewParams *viewParams, const int tileLevel,
                      qreal& tileCol, qreal& tileRow );

 Q_SIGNALS:
    void mapChanged();

 private Q_SLOTS:
    void notifyMapChanged();

 protected:
    void pixelValue( const qreal& lon, const qreal& lat, 
                     QRgb* scanLine, bool smooth = false );

    // method for fast integer calculation
    void nextTile( int& posx, int& posy );

    // method for precise interpolation
    void nextTile( qreal& posx, qreal& posy );

    void detectMaxTileLevel();
    void tileLevelInit( int tileLevel );

    int globalWidth() const;
    int globalHeight() const;

    // Converts Radian to global texture coordinates 
    // ( with origin in center, measured in pixel) 
    qreal rad2PixelX( const qreal longitude ) const;
    qreal rad2PixelY( const qreal latitude ) const;

    QRgb bilinearSmooth( const QRgb& topLeftValue ) const;

    // Coordinates on the tile for fast integer calculation
    int        m_iPosX;
    int        m_iPosY;
    // Coordinates on the tile for precise interpolation
    qreal     m_posX;
    qreal     m_posY;

    // maximum values for global texture coordinates
    // ( with origin in upper left corner, measured in pixel) 
    int     m_maxGlobalX;
    int     m_maxGlobalY;

    int     m_imageHeight;
    int     m_imageWidth;
    int     m_imageRadius;

    // Previous coordinates
    qreal  m_prevLat;
    qreal  m_prevLon;

    // Coordinate transformations:

    // Converts global texture coordinates 
    // ( with origin in center, measured in pixel) 
    // to tile coordinates ( measured in pixel )
    qreal  m_toTileCoordinatesLon;
    qreal  m_toTileCoordinatesLat;

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
    qreal       m_normGlobalWidth;
    qreal       m_normGlobalHeight;
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

inline qreal AbstractScanlineTextureMapper::rad2PixelX( const qreal longitude ) const
{
    return longitude * m_normGlobalWidth;
}

inline qreal AbstractScanlineTextureMapper::rad2PixelY( const qreal lat ) const
{
    switch ( m_tileProjection ) {
    case GeoSceneTexture::Equirectangular:
        return -lat * m_normGlobalHeight;
    case GeoSceneTexture::Mercator:
        if ( fabs( lat ) < 1.4835 )
    {
        // We develop the inverse Gudermannian into a MacLaurin Series:
        // Inspite of the many elements needed to get decent 
        // accuracy this is still faster by far than calculating the 
        // trigonometric expression:
        // return - asinh( tan( lat ) ) * 0.5 * m_normGlobalHeight;

        // We are using the Horner Scheme as a polynom representation

            return - gdInv( lat ) * 0.5 * m_normGlobalHeight;
    }
        if ( lat >= +1.4835 )
            // asinh( tan (1.4835)) => 3.1309587
            return - 3.1309587 * 0.5 * m_normGlobalHeight; 
        if ( lat <= -1.4835 )
            // asinh( tan( -1.4835 )) => −3.1309587
            return 3.1309587 * 0.5 * m_normGlobalHeight; 
    }

    // Dummy value to avoid a warning.
    return 0.0;
}

inline QRgb AbstractScanlineTextureMapper::bilinearSmooth( const QRgb& topLeftValue ) const
{
    qreal fY = m_posY - (int)(m_posY);

    // Interpolation in y-direction
    if ( ( m_posY + 1.0 ) < m_tileLoader->tileHeight() ) {

        QRgb bottomLeftValue  =  m_tile->pixel( (int)(m_posX), (int)(m_posY + 1.0) );

        // blending the color values of the top left and bottom left point
        int ml_red   = (int)( ( 1.0 - fY ) * qRed  ( topLeftValue  ) + fY * qRed  ( bottomLeftValue  ) );
        int ml_green = (int)( ( 1.0 - fY ) * qGreen( topLeftValue  ) + fY * qGreen( bottomLeftValue  ) );
        int ml_blue  = (int)( ( 1.0 - fY ) * qBlue ( topLeftValue  ) + fY * qBlue ( bottomLeftValue  ) );

        // Interpolation in x-direction
        if ( ( m_posX + 1.0 ) < m_tileLoader->tileWidth() ) {

            qreal fX = m_posX - (int)(m_posX);

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

            qreal fX = m_posX - (int)(m_posX);

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

}

#endif
