#ifndef NWWIMAGE_H
#define NWWIMAGE_H

#include <QtCore/QCache>
#include <QtCore/QDir>
#include <QtCore/QPair>
#include <QtCore/QSet>
#include <QtGui/QColor>
#include <QtGui/QImage>

class NwwMapImage
{
public:
    NwwMapImage();
    NwwMapImage( QDir const & baseDirectory, int const tileLevel );

    QRgb pixel( double const lonRad, double const latRad );
    QRgb pixel( int const x, int const y );
    void setBaseDirectory( QDir const & baseDirectory );
    void setTileLevel( int const level );

private:
    static int tileId( int const tileX, int const tileY );
    QPair<QImage, bool> tile( int const tileX, int const tileY );
    double lonRadToPixelX( double const lonRad ) const;
    double latRadToPixelY( double const latRad ) const;


    int const m_tileEdgeLengthPixel;
    QRgb const m_emptyPixel;

    QDir m_baseDirectory;
    int m_tileLevel;
    int m_mapWidthTiles;
    int m_mapHeightTiles;
    int m_mapWidthPixel;
    int m_mapHeightPixel;

    QSet<int> m_tileMissing;
    QCache<int, QImage> m_tileCache;
};

#endif
