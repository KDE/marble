//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de
//

#ifndef MARBLE_SCANLINETEXTUREMAPPERCONTEXT_H
#define MARBLE_SCANLINETEXTUREMAPPERCONTEXT_H

#include <QtCore/QSize>
#include <QtGui/QImage>

#include "GeoSceneTiled.h"
#include "MarbleMath.h"
#include "MathHelper.h"

namespace Marble
{

class StackedTile;
class StackedTileLoader;
class ViewportParams;


class ScanlineTextureMapperContext
{
public:
    ScanlineTextureMapperContext( StackedTileLoader * const tileLoader, int tileLevel );

    void pixelValueF( const qreal lon, const qreal lat,
                      QRgb* const scanLine );
    void pixelValue( const qreal lon, const qreal lat,
                     QRgb* const scanLine );

    void pixelValueApproxF( const qreal lon, const qreal lat,
                            QRgb *scanLine, const int n );
    void pixelValueApprox( const qreal lon, const qreal lat,
                           QRgb *scanLine, const int n );

    static int interpolationStep( const ViewportParams *viewport, MapQuality mapQuality );

    static QImage::Format optimalCanvasImageFormat( const ViewportParams *viewport );

    int globalWidth() const;
    int globalHeight() const;

    const StackedTile getTile();

private:
    // method for fast integer calculation
    void nextTile( int& posx, int& posy );

    // method for precise interpolation
    void nextTile( qreal& posx, qreal& posy );

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

private:
    StackedTileLoader *const m_tileLoader;
    GeoSceneTiled::Projection const m_textureProjection;
    /// size of the tiles of of the current texture layer
    QSize const m_tileSize;

    int const        m_tileLevel;
    int const        m_globalWidth;
    int const        m_globalHeight;
    qreal const      m_normGlobalWidth;
    qreal const      m_normGlobalHeight;

    const StackedTile *m_tile;

    int m_deltaLevel;

    // Coordinate transformations:

    // Position of the tile in global Texture Coordinates
    // ( with origin in upper left corner, measured in pixel) 
    int          m_tilePosX;
    int          m_tilePosY;
    int m_vTileStartX;
    int m_vTileStartY;

    // Converts global texture coordinates 
    // ( with origin in center, measured in pixel) 
    // to tile coordinates ( measured in pixel )
    qreal  m_toTileCoordinatesLon;
    qreal  m_toTileCoordinatesLat;

    // Previous coordinates
    qreal  m_prevLat;
    qreal  m_prevLon;
};

inline int ScanlineTextureMapperContext::globalWidth() const
{
    return m_globalWidth;
}

inline int ScanlineTextureMapperContext::globalHeight() const
{
    return m_globalHeight;
}

inline qreal ScanlineTextureMapperContext::rad2PixelX( const qreal lon ) const
{
    return lon * m_normGlobalWidth;
}

inline qreal ScanlineTextureMapperContext::rad2PixelY( const qreal lat ) const
{
    switch ( m_textureProjection ) {
    case GeoSceneTiled::Equirectangular:
        return -lat * m_normGlobalHeight;
    case GeoSceneTiled::Mercator:
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

}

#endif
