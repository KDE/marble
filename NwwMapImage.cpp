#include "NwwMapImage.h"

#include <QtCore/QDebug>
#include <cmath>

NwwMapImage::NwwMapImage()
    : m_tileEdgeLengthPixel( 512 ),
      m_emptyPixel( qRgba( 0, 0, 0, 255 )),
      m_interpolationMethod( NearestNeighborInterpolation ),
      m_tileCache( 100 * 1024 * 1024 ) // 100 MB cache
{
}

NwwMapImage::NwwMapImage( QDir const & baseDirectory, int const tileLevel )
    : m_tileEdgeLengthPixel( 512 ),
      m_emptyPixel( qRgba( 0, 0, 0, 255 )),
      m_baseDirectory( baseDirectory ),
      m_tileLevel( tileLevel ),
      m_mapWidthTiles( 10 * pow( 2, m_tileLevel )),
      m_mapHeightTiles( 5 * pow( 2, m_tileLevel )),
      m_mapWidthPixel( m_mapWidthTiles * m_tileEdgeLengthPixel ),
      m_mapHeightPixel( m_mapHeightTiles * m_tileEdgeLengthPixel ),
      m_interpolationMethod( NearestNeighborInterpolation ),
      m_tileCache( 100 * 1024 * 1024 ) // 100 MB cache
{
    if ( !m_baseDirectory.exists() )
        qFatal("Base directory does not exist.");

    qDebug() << "tileLevel:" << m_tileLevel
             << "\nmapWidthTiles:" << m_mapWidthTiles
             << "\nmapHeightTiles:" << m_mapHeightTiles
             << "\nmapWidthPixel:" << m_mapWidthPixel
             << "\nmapHeightPixel:" << m_mapHeightPixel;
}

QRgb NwwMapImage::pixel( double const lonRad, double const latRad )
{
    double const x = lonRadToPixelX( lonRad );
    double const y = latRadToPixelY( latRad );

    switch ( m_interpolationMethod ) {
    case NearestNeighborInterpolation:
        return nearestNeighbor( x, y );
    default:
        return nearestNeighbor( x, y );
    }
}

QRgb NwwMapImage::pixel( int const x, int const y )
{
    int const tileX = x / m_tileEdgeLengthPixel;
    int const tileY = y / m_tileEdgeLengthPixel;
    QPair<QImage, bool> potentialTile = tile( tileX, tileY );
    if ( !potentialTile.second )
        return m_emptyPixel;
    else
        return potentialTile.first.pixel( x % m_tileEdgeLengthPixel,
                                          m_tileEdgeLengthPixel - y % m_tileEdgeLengthPixel - 1 );
}

void NwwMapImage::setBaseDirectory( QDir const & baseDirectory )
{
    m_baseDirectory = baseDirectory;
}

void NwwMapImage::setTileLevel( int const tileLevel )
{
    m_tileLevel = tileLevel;
    m_mapWidthTiles = 10 * pow( 2, m_tileLevel );
    m_mapHeightTiles = 5 * pow( 2, m_tileLevel );
    m_mapWidthPixel = m_mapWidthTiles * m_tileEdgeLengthPixel;
    m_mapHeightPixel = m_mapHeightTiles * m_tileEdgeLengthPixel;
}

inline int NwwMapImage::tileId( int const tileX, int const tileY )
{
    return (tileX << 16) + tileY;
}

QPair<QImage, bool> NwwMapImage::tile( int const tileX, int const tileY )
{
    int const tileKey = tileId( tileX, tileY );

    // first check cache
    QImage * const cachedTile = m_tileCache.object( tileKey );
    if ( cachedTile )
        return QPair<QImage, bool>( *cachedTile, true );

    // fast check if tile is missing, perhaps move into caller
    if ( m_tileMissing.contains( tileKey ))
        return QPair<QImage, bool>( QImage(), false );

    QString const filename = QString("%1/%2/%2_%3.jpg")
            .arg( m_baseDirectory.path() )
            .arg( tileY, 4, 10, QLatin1Char('0'))
            .arg( tileX, 4, 10, QLatin1Char('0'));
    QImage tile;
    bool const loaded = tile.load( filename );
    if ( !loaded ) {
        m_tileMissing.insert( tileKey );
        //qDebug() << "Tile" << filename << "not found";
    } else {
        m_tileCache.insert( tileKey, new QImage( tile ), tile.byteCount() );
        //qDebug() << "Tile" << filename << "loaded and inserted in cache";
    }
    return QPair<QImage, bool>( tile, loaded );
}

inline double NwwMapImage::lonRadToPixelX( double const lonRad ) const
{
    return static_cast<double>( m_mapWidthPixel ) / ( 2.0 * M_PI ) * lonRad
            + 0.5 * static_cast<double>( m_mapWidthPixel );
}

inline double NwwMapImage::latRadToPixelY( double const latRad ) const
{
    return static_cast<double>( m_mapHeightPixel ) / M_PI * latRad
            + 0.5 * static_cast<double>( m_mapHeightPixel );
}

QRgb NwwMapImage::nearestNeighbor( double const x, double const y )
{
    int const xr = round( x );
    int const yr = round( y );
    return pixel( xr, yr );
}
