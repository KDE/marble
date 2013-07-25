#include "NwwMapImage.h"

#include "InterpolationMethod.h"

#include <QDebug>
#include <cmath>

NwwMapImage::NwwMapImage( QDir const & baseDirectory, int const tileLevel )
    : m_tileEdgeLengthPixel( 512 ),
      m_emptyPixel( qRgba( 0, 0, 0, 255 )),
      m_baseDirectory( baseDirectory ),
      m_tileLevel( tileLevel ),
      m_mapWidthTiles( 10 * pow( 2, m_tileLevel )),
      m_mapHeightTiles( 5 * pow( 2, m_tileLevel )),
      m_mapWidthPixel( m_mapWidthTiles * m_tileEdgeLengthPixel ),
      m_mapHeightPixel( m_mapHeightTiles * m_tileEdgeLengthPixel ),
      m_interpolationMethod(),
      m_tileCache( DefaultCacheSizeBytes )
{
    if ( !m_baseDirectory.exists() )
        qFatal( "Base directory '%s' does not exist.", m_baseDirectory.path().toStdString().c_str() );

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
    return m_interpolationMethod->interpolate( x, y );
}

QRgb NwwMapImage::pixel( int const x, int const y )
{
    int const tileX = x / m_tileEdgeLengthPixel;
    int const tileY = y / m_tileEdgeLengthPixel;

    // fast check if tile is missing
    int const tileKey = tileId( tileX, tileY );
    if ( m_tileMissing.contains( tileKey ))
        return m_emptyPixel;

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

void NwwMapImage::setCacheSizeBytes(const int cacheSizeBytes)
{
    m_tileCache.setMaxCost( cacheSizeBytes );
}

void NwwMapImage::setInterpolationMethod( InterpolationMethod * const method )
{
    m_interpolationMethod = method;
    m_interpolationMethod->setMapImage( this );
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

QRgb NwwMapImage::bilinearInterpolation( double const x, double const y )
{
    int const x1 = x;
    int const x2 = x1 + 1;
    int const y1 = y;
    int const y2 = y1 + 1;

    QRgb const lowerLeftPixel = pixel( x1, y1 );
    QRgb const lowerRightPixel = pixel( x2, y1 );
    QRgb const upperLeftPixel = pixel( x1, y2 );
    QRgb const upperRightPixel = pixel( x2, y2 );

    // interpolate horizontically
    //
    // x2 - x    x2 - x
    // ------- = ------ = x1 + 1 - x = 1 - fractionX
    // x2 - x1      1
    //
    // x - x1    x - x1
    // ------- = ------ = fractionX
    // x2 - x1     1

    double const fractionX = x - x1;
    double const lowerMidRed   = ( 1.0 - fractionX ) * qRed( lowerLeftPixel )   + fractionX * qRed( lowerRightPixel );
    double const lowerMidGreen = ( 1.0 - fractionX ) * qGreen( lowerLeftPixel ) + fractionX * qGreen( lowerRightPixel );
    double const lowerMidBlue  = ( 1.0 - fractionX ) * qBlue( lowerLeftPixel )  + fractionX * qBlue( lowerRightPixel );
    double const lowerMidAlpha = ( 1.0 - fractionX ) * qAlpha( lowerLeftPixel ) + fractionX * qAlpha( lowerRightPixel );

    double const upperMidRed   = ( 1.0 - fractionX ) * qRed( upperLeftPixel )   + fractionX * qRed( upperRightPixel );
    double const upperMidGreen = ( 1.0 - fractionX ) * qGreen( upperLeftPixel ) + fractionX * qGreen( upperRightPixel );
    double const upperMidBlue  = ( 1.0 - fractionX ) * qBlue( upperLeftPixel )  + fractionX * qBlue( upperRightPixel );
    double const upperMidAlpha = ( 1.0 - fractionX ) * qAlpha( upperLeftPixel ) + fractionX * qAlpha( upperRightPixel );

    // interpolate vertically
    //
    // y2 - y    y2 - y
    // ------- = ------ = y1 + 1 - y = 1 - fractionY
    // y2 - y1      1
    //
    // y - y1    y - y1
    // ------- = ------ = fractionY
    // y2 - y1     1

    double const fractionY = y - y1;
    double const red   = ( 1.0 - fractionY ) * lowerMidRed   + fractionY * upperMidRed;
    double const green = ( 1.0 - fractionY ) * lowerMidGreen + fractionY * upperMidGreen;
    double const blue  = ( 1.0 - fractionY ) * lowerMidBlue  + fractionY * upperMidBlue;
    double const alpha = ( 1.0 - fractionY ) * lowerMidAlpha + fractionY * upperMidAlpha;

    return qRgba( round( red ), round( green ), round( blue ), round( alpha ));
}
