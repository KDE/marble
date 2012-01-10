#include "NasaWorldWindToOpenStreetMapConverter.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <cmath>


NasaWorldWindToOpenStreetMapConverter::NasaWorldWindToOpenStreetMapConverter( QObject * const parent )
    : QObject( parent ),
      m_osmTileEdgeLengthPixel( 256 ),
      m_nwwTileEdgeLengthPixel( 512 ),
      m_emptyPixel( qRgba( 0, 0, 0, 255 )),
      m_nwwTileLevel(),
      m_nwwMapWidthTiles(),
      m_nwwMapHeightTiles(),
      m_nwwMapWidthPixel(),
      m_nwwMapHeightPixel(),
      m_osmTileLevel(),
      m_osmMapEdgeLengthTiles(),
      m_osmMapEdgeLengthPixel()
{
}

void NasaWorldWindToOpenStreetMapConverter::setSourcePath( QDir const & sourcePath )
{
    if ( !sourcePath.exists() )
        qFatal("Source path does not exist.");
    m_sourceBaseDirectory = sourcePath;
}

void NasaWorldWindToOpenStreetMapConverter::setDestinationPath( QDir const & destinationPath )
{
    if ( !destinationPath.exists() ) {
        qDebug() << "Destination path " << destinationPath << "does not exist, creating.";
        destinationPath.mkpath( destinationPath.path() );
    }
    m_destinationBaseDirectory = destinationPath;
}

void NasaWorldWindToOpenStreetMapConverter::setNwwTileLevel( int const level )
{
    m_nwwTileLevel = level;
    m_nwwMapWidthTiles = 10 * pow( 2, m_nwwTileLevel );
    m_nwwMapHeightTiles = 5 * pow( 2, m_nwwTileLevel );
    m_nwwMapWidthPixel = m_nwwMapWidthTiles * m_nwwTileEdgeLengthPixel;
    m_nwwMapHeightPixel = m_nwwMapHeightTiles * m_nwwTileEdgeLengthPixel;
    qDebug() << "nwwTileLevel:" << m_nwwTileLevel
             << "\nnwwMapWidthTiles:" << m_nwwMapWidthTiles
             << "\nnwwMapHeightTiles:" << m_nwwMapHeightTiles
             << "\nnwwMapWidthPixel:" << m_nwwMapWidthPixel
             << "\nnwwMapHeightPixel:" << m_nwwMapHeightPixel;
}

void NasaWorldWindToOpenStreetMapConverter::setOsmTileLevel( int const level )
{
    m_osmTileLevel = level;
    m_osmMapEdgeLengthTiles = pow( 2, m_osmTileLevel );
    m_osmMapEdgeLengthPixel = m_osmMapEdgeLengthTiles * m_osmTileEdgeLengthPixel;
    qDebug() << "osmTileLevel:" << m_osmTileLevel
             << "\nosmMapEdgeLengthTiles:" << m_osmMapEdgeLengthTiles
             << "\nosmMapEdgeLengthPixel:" << m_osmMapEdgeLengthPixel;
}

void NasaWorldWindToOpenStreetMapConverter::start()
{
    int const tileX1 = 0; // 11:1114 12:1125;
    int const tileX2 = m_osmMapEdgeLengthTiles;
    int const tileY1 = 0;
    int const tileY2 = m_osmMapEdgeLengthTiles;

    for ( int tileX = tileX1; tileX < tileX2; ++tileX ) {
        QDir const tileDirectory = checkAndCreateDirectory( tileX );
        for ( int tileY = tileY1; tileY < tileY2; ++tileY ) {
            QImage const osmTile = calcOsmTile( tileX, tileY );

            // hack
            if ( osmTile.isNull() )
                continue;

            QString const filename = tileDirectory.path() + QString( "/%1.png" ).arg( tileY );
            bool const saved = osmTile.save( filename );
            if ( !saved )
                qFatal("Unable to save tile '%s'.", filename.toStdString().c_str() );
        }
    }
    emit finished();
}

void NasaWorldWindToOpenStreetMapConverter::testReprojection()
{
    qDebug() << "\nTesting osm pixel x -> lon[rad]";
    qDebug() << 0 << "->" << osmPixelXtoLonRad( 0 );
    qDebug() << m_osmMapEdgeLengthPixel / 4 << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel / 4 );
    qDebug() << m_osmMapEdgeLengthPixel / 2 << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel / 2 );
    qDebug() << m_osmMapEdgeLengthPixel / 4 * 3 << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel / 4 * 3 );
    qDebug() << m_osmMapEdgeLengthPixel << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel );

    qDebug() << "\nTesting osm pixel y -> lat[rad]";
    qDebug() << 0 << "->" << osmPixelYtoLatRad( 0 );
    qDebug() << m_osmMapEdgeLengthPixel / 4 << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel / 4 );
    qDebug() << m_osmMapEdgeLengthPixel / 2 << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel / 2 );
    qDebug() << m_osmMapEdgeLengthPixel / 4 * 3 << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel / 4 * 3 );
    qDebug() << m_osmMapEdgeLengthPixel << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel );

    qDebug() << "\nTesting lon[rad] -> nww pixel x";
    qDebug() <<       -M_PI << "->" << lonRadToNwwPixelX( -M_PI );
    qDebug() << -M_PI / 2.0 << "->" << lonRadToNwwPixelX( -M_PI / 2.0 );
    qDebug() <<           0 << "->" << lonRadToNwwPixelX(     0 );
    qDebug() <<  M_PI / 2.0 << "->" << lonRadToNwwPixelX(  M_PI / 2.0 );
    qDebug() <<        M_PI << "->" << lonRadToNwwPixelX(  M_PI );

    qDebug() << "\nTesting lat[rad] -> nww pixel y";
    qDebug() <<  M_PI / 2.0 << "->" << latRadToNwwPixelY(  M_PI / 2.0 );
    qDebug() <<           0 << "->" << latRadToNwwPixelY(     0 );
    qDebug() << -M_PI / 2.0 << "->" << latRadToNwwPixelY( -M_PI / 2.0 );
}

QImage NasaWorldWindToOpenStreetMapConverter::calcOsmTile( int const tileX, int const tileY )
{
    qDebug() << "calcOsmTile( tileX=" << tileX << ", tileY=" << tileY << " )";
    int const basePixelX = tileX * m_osmTileEdgeLengthPixel;
    int const basePixelY = tileY * m_osmTileEdgeLengthPixel;

    QSize const tileSize( m_osmTileEdgeLengthPixel, m_osmTileEdgeLengthPixel );
    QImage tile( tileSize, QImage::Format_ARGB32 );
    bool notEmpty = false;

    for ( int y = 0; y < m_osmTileEdgeLengthPixel; ++y ) {
        int const pixelY = basePixelY + y;
        double const latRad = osmPixelYtoLatRad( pixelY );

        for ( int x = 0; x < m_osmTileEdgeLengthPixel; ++x ) {
            int const pixelX = basePixelX + x;
            double const lonRad = osmPixelXtoLonRad( pixelX );
            QRgb const color = nwwPixel( lonRad, latRad );

            if ( color != m_emptyPixel )
                notEmpty = true;

            tile.setPixel( x, y, color );
        }
    }
    return notEmpty ? tile : QImage();
}

QDir NasaWorldWindToOpenStreetMapConverter::checkAndCreateDirectory( int const tileX ) const
{
    QDir const tileDirectory( m_destinationBaseDirectory.path() + QString("/%1/%2").arg( m_osmTileLevel ).arg( tileX ));
    if ( !tileDirectory.exists() ) {
        bool const created = tileDirectory.mkpath( tileDirectory.path() );
        if ( !created )
            qFatal("Unable to create directory '%s'.", tileDirectory.path().toStdString().c_str() );
    }
    return tileDirectory;
}

QRgb NasaWorldWindToOpenStreetMapConverter::nwwPixel( double const lonRad, double const latRad )
{
    double const x = lonRadToNwwPixelX( lonRad );
    double const y = latRadToNwwPixelY( latRad );

    // for now, just round
    int const xr = round( x );
    int const yr = round( y );
    int const roundedTileId = ( xr << 16 ) + yr;
    if ( m_nwwTileMissing.contains( roundedTileId ))
        return m_emptyPixel;

    return nwwPixel( xr, yr );


    int const x1 = x;
    int const x2 = x1 + 1;
    int const y1 = y;
    int const y2 = y1 + 1;

    // first check if all tiles are missing
    int const topLeftTileId = (x1 << 16) + y2;
    int const topRightTileId = (x2 << 16) + y2;
    int const bottomLeftTileId = (x1 << 16) + y1;
    int const bottomRightTileId = (x2 << 16) + y1;
    if ( m_nwwTileMissing.contains( topLeftTileId ) && topLeftTileId == topRightTileId
         && topLeftTileId == bottomLeftTileId && topLeftTileId == bottomRightTileId )
        return m_emptyPixel;

    if ( m_nwwTileMissing.contains( topRightTileId )
         && m_nwwTileMissing.contains( bottomLeftTileId ) && m_nwwTileMissing.contains( bottomRightTileId ))
        return m_emptyPixel;

    QRgb const topLeftPixel = nwwPixel( x1, y2 );
    QRgb const topRightPixel = nwwPixel( x2, y2 );
    QRgb const bottomLeftPixel = nwwPixel( x1, y1 );
    QRgb const bottomRightPixel = nwwPixel( x2, y1 );

    // cheap hack to see if osm tiles look somehow ok
    if ( topLeftPixel != m_emptyPixel )
        return topLeftPixel;
    else if ( topRightPixel != m_emptyPixel )
        return topRightPixel;
    else if ( bottomLeftPixel != m_emptyPixel )
        return bottomLeftPixel;
    else if ( bottomRightPixel != m_emptyPixel )
        return bottomRightPixel;
    else
        return m_emptyPixel;

    //double const fractionalX = x - x1;
    //double const fractionalY = y - y1;
}

QRgb NasaWorldWindToOpenStreetMapConverter::nwwPixel( int const pixelX, int const pixelY )
{
    int const tileX = pixelX / m_nwwTileEdgeLengthPixel;
    int const tileY = pixelY / m_nwwTileEdgeLengthPixel;
    QPair<QImage, bool> tile = nwwTile( tileX, tileY );
    if ( !tile.second )
        return m_emptyPixel;
    else
        return tile.first.pixel( pixelX % m_nwwTileEdgeLengthPixel,
                                 m_nwwTileEdgeLengthPixel - pixelY % m_nwwTileEdgeLengthPixel - 1 );
}

QPair<QImage, bool> NasaWorldWindToOpenStreetMapConverter::nwwTile( int const tileX, int const tileY )
{
    int const tileId = (tileX << 16) + tileY;

    // first check cache
    QImage * const cachedTile = m_nwwTileCache.object( tileId );
    if ( cachedTile )
        return QPair<QImage, bool>( *cachedTile, true );

    // fast check if tile is missing, perhaps move into caller
    if ( m_nwwTileMissing.contains( tileId ))
        return QPair<QImage, bool>( QImage(), false );

    QString const filename = QString("%1/%2/%2_%3.jpg")
            .arg( m_sourceBaseDirectory.path() )
            .arg( tileY, 4, 10, QLatin1Char('0'))
            .arg( tileX, 4, 10, QLatin1Char('0'));
    QImage tile;
    bool const loaded = tile.load( filename );
    if ( !loaded ) {
        m_nwwTileMissing.insert( tileId );
        //qDebug() << "Tile" << filename << "not found";
    } else {
        m_nwwTileCache.insert( tileId, new QImage( tile ));
        qDebug() << "Tile" << filename << "loaded and inserted in cache";
    }
    return QPair<QImage, bool>( tile, loaded );
}

inline double NasaWorldWindToOpenStreetMapConverter::osmPixelXtoLonRad( int const pixelX ) const
{
    double const pixelXd = static_cast<double>( pixelX );
    double const osmMapEdgeLengthPixeld = static_cast<double>( m_osmMapEdgeLengthPixel );
    return pixelXd * 2.0 * M_PI / osmMapEdgeLengthPixeld - M_PI;
}

inline double NasaWorldWindToOpenStreetMapConverter::osmPixelYtoLatRad( int const pixelY ) const
{
    double const pixelYd = static_cast<double>( pixelY );
    double const osmMapEdgeLengthPixeld = static_cast<double>( m_osmMapEdgeLengthPixel );
    double const latRad = -atan( sinh(( pixelYd - 0.5 * osmMapEdgeLengthPixeld ) * 2.0 * M_PI / osmMapEdgeLengthPixeld ));
    return latRad;
}

inline double NasaWorldWindToOpenStreetMapConverter::lonRadToNwwPixelX( double const lonRad ) const
{
    return static_cast<double>( m_nwwMapWidthPixel ) / ( 2.0 * M_PI ) * lonRad
            + 0.5 * static_cast<double>( m_nwwMapWidthPixel );
}

inline double NasaWorldWindToOpenStreetMapConverter::latRadToNwwPixelY( double const latRad ) const
{
    return static_cast<double>( m_nwwMapHeightPixel ) / M_PI * latRad
            + 0.5 * static_cast<double>( m_nwwMapHeightPixel );
}
