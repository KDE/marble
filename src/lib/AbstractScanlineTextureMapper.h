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
    virtual ~AbstractScanlineTextureMapper();
    virtual void mapTexture(QImage* canvasImage, const int&, Quaternion& planetAxis) =0;

    void setMapTheme( const QString& theme );
    void setMaxTileLevel( int level ){ m_maxTileLevel = level; }
    void resizeMap(const QImage* canvasImage);
    void selectTileLevel(int radius);

 protected:
    void pixelValue(const double& lng, const double& lat, QRgb* scanLine);
    void nextTile();
    void detectMaxTileLevel();

    void tileLevelInit( int tileLevel );

    int          m_posX;
    int          m_posY;

    TileLoader  *m_tileLoader;
    QRgb        *m_scanLine;

    int          m_maxTileLevel;

    int    m_imageHalfWidth;
    int    m_imageHalfHeight;
    int    m_imageHeight;
    int    m_imageWidth;
    int    m_imageRadius;

    double  m_prevLat;
    double  m_prevLng;

    int    m_tilePosX;
    int    m_tilePosY;

    int    m_fullRangeLng;
    int    m_halfRangeLat;
    double  m_halfRangeLng;
    double  m_quatRangeLat;

    int    m_fullNormLng;
    int    m_halfNormLat;
    double  m_halfNormLng;
    double  m_quatNormLat;

    double  m_rad2PixelX;
    double  m_rad2PixelY;

    TextureTile  *m_tile;
    int           m_tileLevel;

 Q_SIGNALS:
    void mapChanged();

private Q_SLOTS:
    void notifyMapChanged();
};
#endif
