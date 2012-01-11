#include "NasaWorldWindToOpenStreetMapConverter.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <cmath>


NasaWorldWindToOpenStreetMapConverter::NasaWorldWindToOpenStreetMapConverter( QObject * const parent )
    : QObject( parent ),
      m_osmTileEdgeLengthPixel( 256 ),
      m_emptyPixel( qRgba( 0, 0, 0, 255 )),
      m_nwwMapImage(),
      m_osmTileLevel(),
      m_osmMapEdgeLengthTiles(),
      m_osmMapEdgeLengthPixel()
{
}

void NasaWorldWindToOpenStreetMapConverter::setSourcePath( QDir const & sourcePath )
{
    m_nwwMapImage.setBaseDirectory( sourcePath );
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
    m_nwwMapImage.setTileLevel( level );
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
    // render Osm tiles using quadratic clusters of tiles instead of stripes
    // to increase Nww tile cache usage

    int const osmTileClusterEdgeLengthTiles = 64;
    int const osmMapEdgeLengthClusters = m_osmMapEdgeLengthTiles / osmTileClusterEdgeLengthTiles;
    if ( m_osmMapEdgeLengthTiles % osmTileClusterEdgeLengthTiles != 0 )
        qFatal("Bad tile cluster size");

    for ( int clusterX = 0; clusterX < osmMapEdgeLengthClusters; ++clusterX )
        for ( int clusterY = 0; clusterY < osmMapEdgeLengthClusters; ++clusterY )
            renderOsmTileCluster( clusterX, clusterY, osmTileClusterEdgeLengthTiles );

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

//    qDebug() << "\nTesting lon[rad] -> nww pixel x";
//    qDebug() <<       -M_PI << "->" << lonRadToNwwPixelX( -M_PI );
//    qDebug() << -M_PI / 2.0 << "->" << lonRadToNwwPixelX( -M_PI / 2.0 );
//    qDebug() <<           0 << "->" << lonRadToNwwPixelX(     0 );
//    qDebug() <<  M_PI / 2.0 << "->" << lonRadToNwwPixelX(  M_PI / 2.0 );
//    qDebug() <<        M_PI << "->" << lonRadToNwwPixelX(  M_PI );

//    qDebug() << "\nTesting lat[rad] -> nww pixel y";
//    qDebug() <<  M_PI / 2.0 << "->" << latRadToNwwPixelY(  M_PI / 2.0 );
//    qDebug() <<           0 << "->" << latRadToNwwPixelY(     0 );
//    qDebug() << -M_PI / 2.0 << "->" << latRadToNwwPixelY( -M_PI / 2.0 );
}

void NasaWorldWindToOpenStreetMapConverter::renderOsmTileCluster( int const clusterX, int const clusterY,
                                                                  int const clusterEdgeLengthTiles )
{
    int const tileX1 = clusterX * clusterEdgeLengthTiles;
    int const tileX2 = tileX1 + clusterEdgeLengthTiles;
    int const tileY1 = clusterY * clusterEdgeLengthTiles;
    int const tileY2 = tileY1 + clusterEdgeLengthTiles;

    for ( int tileX = tileX1; tileX < tileX2; ++tileX ) {
        QDir const tileDirectory = checkAndCreateDirectory( tileX );
        for ( int tileY = tileY1; tileY < tileY2; ++tileY ) {
            QImage const osmTile = renderOsmTile( tileX, tileY );

            // hack
            if ( osmTile.isNull() )
                continue;

            QString const filename = tileDirectory.path() + QString( "/%1.png" ).arg( tileY );
            bool const saved = osmTile.save( filename );
            if ( !saved )
                qFatal("Unable to save tile '%s'.", filename.toStdString().c_str() );
        }
    }
}

QImage NasaWorldWindToOpenStreetMapConverter::renderOsmTile( int const tileX, int const tileY )
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
            QRgb const color = m_nwwMapImage.pixel( lonRad, latRad );

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
    return -atan( sinh(( pixelYd - 0.5 * osmMapEdgeLengthPixeld ) * 2.0 * M_PI / osmMapEdgeLengthPixeld ));
}
