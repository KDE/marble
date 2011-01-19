//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#ifndef MARBLE_ABSTRACTSCANLINETEXTUREMAPPER_H
#define MARBLE_ABSTRACTSCANLINETEXTUREMAPPER_H

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtGui/QColor>

#include <cmath>
#include <math.h>

#include "GeoSceneTexture.h"
#include "MarbleMath.h"
#include "MathHelper.h"


namespace Marble
{

class StackedTile;
class StackedTileLoader;
class TextureColorizer;
class ViewParams;

class AbstractScanlineTextureMapper : public QObject
{
    Q_OBJECT

public:
    AbstractScanlineTextureMapper( StackedTileLoader * const tileLoader,
                                   QObject * const parent = 0 );
    ~AbstractScanlineTextureMapper();

    virtual void mapTexture( ViewParams *viewParams, TextureColorizer *texColorizer ) = 0;

    bool interlaced() const;
    void setInterlaced( const bool enabled );
    int tileZoomLevel() const;

 Q_SIGNALS:
    void tileLevelChanged( int newTileLevel );

 protected:
    void pixelValueF( const qreal lon, const qreal lat,
                      QRgb* const scanLine );
    void pixelValue( const qreal lon, const qreal lat,
                     QRgb* const scanLine );

    void pixelValueApproxF( const qreal lon, const qreal lat,
                            QRgb *scanLine, const int n );
    void pixelValueApprox( const qreal lon, const qreal lat,
                           QRgb *scanLine, const int n );

    static int interpolationStep( ViewParams * const viewParams );

    // method for fast integer calculation
    void nextTile( int& posx, int& posy );

    // method for precise interpolation
    void nextTile( qreal& posx, qreal& posy );

    void setRadius( int radius );

    int globalWidth() const;
    int globalHeight() const;

    // Converts Radian to global texture coordinates 
    // ( with origin in center, measured in pixel) 
    qreal rad2PixelX( const qreal lon ) const;
    qreal rad2PixelY( const qreal lat ) const;

    // Checks whether the pixelValueApprox method will make use of more than
    // one tile
    bool isOutOfTileRange( const int itLon, const int itLat,
                           const int itStepLon, const int itStepLat,
                           const int n ) const;

    bool isOutOfTileRangeF( const qreal itLon, const qreal itLat,
                            const qreal itStepLon, const qreal itStepLat,
                            const int n ) const;

    // maximum values for global texture coordinates
    // ( with origin in upper left corner, measured in pixel) 
    int     m_maxGlobalX;
    int     m_maxGlobalY; // could be private also

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

    StackedTileLoader *m_tileLoader;

    // Position of the tile in global Texture Coordinates
    // ( with origin in upper left corner, measured in pixel) 
    int          m_tilePosX;
    int          m_tilePosY;

    GeoSceneTexture::Projection const m_textureProjection;
    /// size of the tiles of of the current texture layer
    QSize const m_tileSize;

 private:
    Q_DISABLE_COPY( AbstractScanlineTextureMapper )
    StackedTile *m_tile;
    int         m_previousRadius;

    int         m_tileLevel;
    int         m_globalWidth;
    int         m_globalHeight;
    qreal       m_normGlobalWidth;
    qreal       m_normGlobalHeight;
};

inline bool AbstractScanlineTextureMapper::interlaced() const
{
    return m_interlaced;
}

inline void AbstractScanlineTextureMapper::setInterlaced( const bool enabled )
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

inline qreal AbstractScanlineTextureMapper::rad2PixelX( const qreal lon ) const
{
    return lon * m_normGlobalWidth;
}

inline qreal AbstractScanlineTextureMapper::rad2PixelY( const qreal lat ) const
{
    switch ( m_textureProjection ) {
    case GeoSceneTexture::Equirectangular:
        return -lat * m_normGlobalHeight;
    case GeoSceneTexture::Mercator:
        if ( fabs( lat ) < 1.4835 ) {
            // We develop the inverse Gudermannian into a MacLaurin Series:
            // In spite of the many elements needed to get decent 
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

inline int AbstractScanlineTextureMapper::tileZoomLevel() const
{
    return m_tileLevel;
}

}

#endif
