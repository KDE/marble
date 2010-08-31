//
// This file is part of the Marble Desktop Globe.
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
class ViewParams;

class AbstractScanlineTextureMapper : public QObject
{
    Q_OBJECT

public:
    AbstractScanlineTextureMapper( GeoSceneTexture *textureLayer, StackedTileLoader *tileLoader,
                                   QObject *parent = 0 );
    ~AbstractScanlineTextureMapper();

    virtual void mapTexture( ViewParams *viewParams ) = 0;

    bool interlaced() const;
    void setInterlaced( bool enabled );
    int tileZoomLevel() const;
    GeoSceneTexture const * textureLayer() const;

 Q_SIGNALS:
    void mapChanged();
    void tileLevelChanged( int newTileLevel );

 private Q_SLOTS:
    void notifyMapChanged();
    
 protected:
    void pixelValueF( const qreal lon, const qreal lat,
                      QRgb* scanLine );
    void pixelValue( const qreal lon, const qreal lat,
                     QRgb* scanLine );

    void pixelValueApproxF(const qreal& lon, const qreal& lat,
                          QRgb *scanLine, int n );
    void pixelValueApprox(const qreal& lon, const qreal& lat,
                          QRgb *scanLine, int n );

    static int interpolationStep( ViewParams *viewParams );

    // method for fast integer calculation
    void nextTile( int& posx, int& posy );

    // method for precise interpolation
    void nextTile( qreal& posx, qreal& posy );

    void selectTileLevel( ViewParams* viewParams );
    void detectMaxTileLevel();
    void tileLevelInit( int tileLevel );

    int globalWidth() const;
    int globalHeight() const;

    // Converts Radian to global texture coordinates 
    // ( with origin in center, measured in pixel) 
    qreal rad2PixelX( const qreal longitude ) const;
    qreal rad2PixelY( const qreal latitude ) const;

    // Checks whether the pixelValueApprox method will make use of more than
    // one tile
    bool isOutOfTileRange( int itLon, int itLat,
                           int itStepLon, int itStepLat,
                           int n ) const;

    bool isOutOfTileRangeF( qreal itLon, qreal itLat,
                            qreal itStepLon, qreal itStepLat,
                            int n ) const;
                           
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

 private:
    Q_DISABLE_COPY( AbstractScanlineTextureMapper )
    void initGlobalWidth();
    void initGlobalHeight();

    GeoSceneTexture const * const m_textureLayer;
    /// size of the tiles of of the current texture layer
    QSize const m_tileSize;
    StackedTile *m_tile;
    int         m_previousRadius;

    int         m_tileLevel;
    int         m_maxTileLevel;
    int         m_globalWidth;
    int         m_globalHeight;
    qreal       m_normGlobalWidth;
    qreal       m_normGlobalHeight;
    uint        m_mapThemeIdHash;
};

inline bool AbstractScanlineTextureMapper::interlaced() const
{
    return m_interlaced;
}

inline void AbstractScanlineTextureMapper::setInterlaced( bool enabled )
{
    m_interlaced = enabled;
}

inline GeoSceneTexture const * AbstractScanlineTextureMapper::textureLayer() const
{
    return m_textureLayer;
}

inline int AbstractScanlineTextureMapper::globalWidth() const
{
    return m_globalWidth;
}

inline int AbstractScanlineTextureMapper::globalHeight() const
{
    return m_globalHeight;
}

inline qreal AbstractScanlineTextureMapper::rad2PixelX( qreal longitude ) const
{
    return longitude * m_normGlobalWidth;
}

inline qreal AbstractScanlineTextureMapper::rad2PixelY( qreal lat ) const
{
    switch ( m_textureLayer->projection() ) {
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
