#ifndef NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H
#define NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H

#include "NwwMapImage.h"

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
    void renderOsmTileCluster( int const clusterX, int const clusterY, int const clusterEdgeLengthTiles );
    QImage calcOsmTile( int const tileX, int const tileY );
    QDir checkAndCreateDirectory( int const tileX ) const;
    double osmPixelXtoLonRad( int const pixelX ) const;
    double osmPixelYtoLatRad( int const pixelY ) const;

    int const m_osmTileEdgeLengthPixel;
    QRgb const m_emptyPixel;

    NwwMapImage m_nwwMapImage;
    QDir m_destinationBaseDirectory;

    int m_osmTileLevel;
    int m_osmMapEdgeLengthTiles;
    int m_osmMapEdgeLengthPixel;
};

#endif
