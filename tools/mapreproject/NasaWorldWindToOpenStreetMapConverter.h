#ifndef NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H
#define NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H

#include "mapreproject.h"
#include "ReadOnlyMapDefinition.h"

#include <QDir>
#include <QObject>
#include <QPair>
#include <QVector>

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

    void setMapSources( QVector<ReadOnlyMapDefinition> const & mapSources );
    void setOsmBaseDirectory( QDir const & nwwBaseDirectory );
    void setOsmTileClusterEdgeLengthTiles( int const clusterEdgeLengthTiles );
    void setOsmTileLevel( int const level );
    void setThreadCount( int const threadCount );

    QVector<QPair<Thread*, OsmTileClusterRenderer*> > start();

    void testReprojection();

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void assignNextCluster( OsmTileClusterRenderer * );

private:
    void checkAndCreateLevelDirectory() const;
    void incNextCluster();

    int m_threadCount;
    QVector<ReadOnlyMapDefinition> m_mapSources;
    QDir m_osmBaseDirectory;
    int m_osmTileLevel;

    int m_osmTileClusterEdgeLengthTiles;
    int m_osmMapEdgeLengthClusters;
    int m_nextClusterX;
    int m_nextClusterY;
};

#endif
