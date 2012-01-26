#ifndef NWWIMAGE_H
#define NWWIMAGE_H

#include "mapreproject.h"
#include "ReadOnlyMapImage.h"

#include <QtCore/QCache>
#include <QtCore/QDir>
#include <QtCore/QPair>
#include <QtCore/QSet>
#include <QtGui/QColor>
#include <QtGui/QImage>

class NwwMapImage: public ReadOnlyMapImage
{
public:
    NwwMapImage();
    NwwMapImage( QDir const & baseDirectory, int const tileLevel );

    virtual QRgb pixel( double const lonRad, double const latRad );

    QRgb pixel( int const x, int const y );
    void setBaseDirectory( QDir const & baseDirectory );
    void setInterpolationMethod( InterpolationMethod const method );
    void setTileLevel( int const level );

private:
    enum { DefaultCacheSizeBytes = 32 * 1024 * 1024 };

    static int tileId( int const tileX, int const tileY );
    QPair<QImage, bool> tile( int const tileX, int const tileY );
    double lonRadToPixelX( double const lonRad ) const;
    double latRadToPixelY( double const latRad ) const;

    // Interpolation methods
    QRgb nearestNeighbor( double const x, double const y );
    QRgb bilinearInterpolation( double const x, double const y );

    int const m_tileEdgeLengthPixel;
    QRgb const m_emptyPixel;

    QDir m_baseDirectory;
    int m_tileLevel;
    int m_mapWidthTiles;
    int m_mapHeightTiles;
    int m_mapWidthPixel;
    int m_mapHeightPixel;


    InterpolationMethod m_interpolationMethod;

    QSet<int> m_tileMissing;
    QCache<int, QImage> m_tileCache;
};

#endif
