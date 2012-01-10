#ifndef NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H
#define NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H

#include <QtCore/QCache>
#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QSet>
#include <QtGui/QImage>

// Abbreviations used:
//   Nww, nww: NASA WorldWind
//   Osm, osm: OpenStreetMap
//   Lon, lon: Longitude
//   Lat, lat: Latitude
//   Rad, rad: Radiant

class NasaWorldWindToOpenStreetMapConverter: public QObject
{
    Q_OBJECT
public:
    explicit NasaWorldWindToOpenStreetMapConverter( QObject * const parent = NULL );
    void setSourcePath( QDir const & sourcePath );
    void setDestinationPath( QDir const & destinationPath );
    void setNwwTileLevel( int const level );
    void setOsmTileLevel( int const level );
    void start();
    void testReprojection();

signals:
    void finished() const;

public slots:
    
private:
    QImage calcOsmTile( int const tileX, int const tileY );
    QDir checkAndCreateDirectory( int const tileX ) const;
    QRgb nwwPixel( double const lonRad, double const latRad );
    QRgb nwwPixel( int const pixelX, int const pixelY );
    QPair<QImage, bool> nwwTile( int const tileX, int const tileY );
    double osmPixelXtoLonRad( int const pixelX ) const;
    double osmPixelYtoLatRad( int const pixelY ) const;
    double lonRadToNwwPixelX( double const lonRad ) const;
    double latRadToNwwPixelY( double const latRad ) const;

    int const m_osmTileEdgeLengthPixel;
    int const m_nwwTileEdgeLengthPixel;
    QRgb const m_emptyPixel;
    QDir m_sourceBaseDirectory;
    QDir m_destinationBaseDirectory;

    int m_nwwTileLevel;
    int m_nwwMapWidthTiles;
    int m_nwwMapHeightTiles;
    int m_nwwMapWidthPixel;
    int m_nwwMapHeightPixel;

    int m_osmTileLevel;
    int m_osmMapEdgeLengthTiles;
    int m_osmMapEdgeLengthPixel;

    QSet<int> m_nwwTileMissing;
    QCache<int, QImage> m_nwwTileCache;
};

#endif
