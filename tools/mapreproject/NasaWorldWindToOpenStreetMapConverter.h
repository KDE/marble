#ifndef NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H
#define NASAWORLDWINDTOOPENSTREETMAPCONVERTER_H

#include "ReadOnlyMapDefinition.h"
#include "mapreproject.h"

#include <QDir>
#include <QList>
#include <QObject>
#include <QPair>

class OsmTileClusterRenderer;
class Thread;

// Abbreviations used:
//   Nww, nww: NASA WorldWind
//   Osm, osm: OpenStreetMap
//   Lon, lon: Longitude
//   Lat, lat: Latitude
//   Rad, rad: Radiant

class NasaWorldWindToOpenStreetMapConverter : public QObject
{
    Q_OBJECT

public:
    explicit NasaWorldWindToOpenStreetMapConverter(QObject *const parent = nullptr);

    void setMapSources(QList<ReadOnlyMapDefinition> const &mapSources);
    void setOsmBaseDirectory(QDir const &nwwBaseDirectory);
    void setOsmTileClusterEdgeLengthTiles(int const clusterEdgeLengthTiles);
    void setOsmTileLevel(int const level);
    void setThreadCount(int const threadCount);

    QList<QPair<Thread *, OsmTileClusterRenderer *>> start();

    void testReprojection();

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void assignNextCluster(OsmTileClusterRenderer *);

private:
    void checkAndCreateLevelDirectory() const;
    void incNextCluster();

    int m_threadCount;
    QList<ReadOnlyMapDefinition> m_mapSources;
    QDir m_osmBaseDirectory;
    int m_osmTileLevel;

    int m_osmTileClusterEdgeLengthTiles;
    int m_osmMapEdgeLengthClusters;
    int m_nextClusterX;
    int m_nextClusterY;
};

#endif
