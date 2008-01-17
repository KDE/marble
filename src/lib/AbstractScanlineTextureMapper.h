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


#include <QtCore/QString>
#include <QtGui/QColor>

#include "Quaternion.h"
#include "AbstractLayer.h"      // In AbstractLayer/


class QImage;
class TextureTile;
class TileLoader;


class AbstractScanlineTextureMapper : public AbstractLayer
{
    Q_OBJECT

public:
    explicit AbstractScanlineTextureMapper( TileLoader *tileLoader, QObject * parent=0 );
    ~AbstractScanlineTextureMapper();

    virtual void mapTexture( ViewParams *viewParams ) = 0;

    void setMapTheme( const QString& theme );
    void setMaxTileLevel( int level ){ m_maxTileLevel = level; }
    virtual void resizeMap( int width, int height );
    void selectTileLevel( ViewParams* viewParams );
    bool interlaced() { return m_interlaced; }
    void setInterlaced( bool enabled ) { m_interlaced = enabled; }

    void centerTiles( ViewParams *viewParams, const int tileLevel,
                      double& tileCol, double& tileRow );

 Q_SIGNALS:
    void mapChanged();

 private Q_SLOTS:
    void notifyMapChanged();

 protected:
    void pixelValue(const double& lon, const double& lat, QRgb* scanLine);
    void nextTile();
    void detectMaxTileLevel();

    void tileLevelInit( int tileLevel );

    // Coordinates on the tile
    int     m_posX;
    int     m_posY;

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

    // Converts Radian to global texture coordinates 
    // ( with origin in center, measured in pixel) 
    double  m_rad2PixelX;
    double  m_rad2PixelY;

    // Converts global texture coordinates 
    // ( with origin in center, measured in pixel) 
    // to tile coordinates ( measured in pixel )
    double  m_toTileCoordinatesLon;
    double  m_toTileCoordinatesLat;

    bool m_interlaced;

    // ------------------------
    // Tile stuff
    TileLoader  *m_tileLoader;
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
};


#endif
