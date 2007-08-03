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
#include "AbstractLayer/AbstractLayer.h"


class QImage;
class TextureTile;
class TileLoader;


class AbstractScanlineTextureMapper : public AbstractLayer
{
    Q_OBJECT

public:
    explicit AbstractScanlineTextureMapper( const QString& path, QObject * parent=0 );
    ~AbstractScanlineTextureMapper();

    virtual void mapTexture(QImage* canvasImage, const int&, 
                            Quaternion& planetAxis) = 0;

    void setMapTheme( const QString& theme );
    void setMaxTileLevel( int level ){ m_maxTileLevel = level; }
    void resizeMap( int width, int height );
    void selectTileLevel(int radius);

 Q_SIGNALS:
    void mapChanged();

 private Q_SLOTS:
    void notifyMapChanged();

 protected:
    void pixelValue(const double& lon, const double& lat, QRgb* scanLine);
    void nextTile();
    void detectMaxTileLevel();

    void tileLevelInit( int tileLevel );

    int     m_posX;
    int     m_posY;

    int     m_imageHeight;
    int     m_imageWidth;
    int     m_imageRadius;

    double  m_prevLat;
    double  m_prevLon;

    int     m_fullRangeLon;
    int     m_halfRangeLat;
    double  m_halfRangeLon;
    double  m_quatRangeLat;

    int     m_fullNormLon;
    int     m_halfNormLat;
    double  m_halfNormLon;
    double  m_quatNormLat;

    double  m_rad2PixelX;
    double  m_rad2PixelY;

    // ------------------------
    // Tile stuff
    TileLoader  *m_tileLoader;
    QRgb        *m_scanLine;

    int          m_maxTileLevel;

    TextureTile *m_tile;
    int          m_tileLevel;
    int          m_tilePosX;
    int          m_tilePosY;
};


#endif
