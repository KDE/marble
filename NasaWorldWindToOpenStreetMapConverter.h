#ifndef NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H
#define NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QVector>

class OsmTileClusterRenderer;
class Thread;

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

    void setNwwBaseDirectory( QDir const & osmBaseDirectory );
    void setNwwTileLevel( int const level );
    void setOsmBaseDirectory( QDir const & nwwBaseDirectory );
    void setOsmTileLevel( int const level );

    QVector<QPair<Thread*, OsmTileClusterRenderer*> > start();

    void testReprojection();

signals:
    void finished();

public slots:
    void assignNextCluster( OsmTileClusterRenderer * );

private:
    void incNextCluster();

    QDir m_nwwBaseDirectory;
    int m_nwwTileLevel;
    QDir m_osmBaseDirectory;
    int m_osmTileLevel;

    int m_osmMapEdgeLengthClusters;
    int m_nextClusterX;
    int m_nextClusterY;
};

#endif
