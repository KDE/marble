#ifndef OSMTILECLUSTERRENDERER_H
#define OSMTILECLUSTERRENDERER_H

#include "ReadOnlyMapDefinition.h"
#include "mapreproject.h"

#include <QDir>
#include <QImage>
#include <QList>
#include <QObject>

class ReadOnlyMapImage;

class OsmTileClusterRenderer : public QObject
{
    Q_OBJECT

public:
    explicit OsmTileClusterRenderer(QObject *const parent = nullptr);

    void setClusterEdgeLengthTiles(int const clusterEdgeLengthTiles);
    void setMapSources(QList<ReadOnlyMapDefinition> const &mapSources);
    void setOsmBaseDirectory(QDir const &osmBaseDirectory);
    void setOsmTileLevel(int const level);

Q_SIGNALS:
    void clusterRendered(OsmTileClusterRenderer *);

public Q_SLOTS:
    void initMapSources();
    void renderOsmTileCluster(int const clusterX, int const clusterY);

private:
    QDir checkAndCreateDirectory(int const tileX) const;
    QImage renderOsmTile(int const tileX, int const tileY);
    double osmPixelXtoLonRad(int const pixelX) const;
    double osmPixelYtoLatRad(int const pixelY) const;

    int const m_osmTileEdgeLengthPixel;
    QRgb const m_emptyPixel;

    QDir m_osmBaseDirectory;
    int m_osmTileLevel;
    int m_osmMapEdgeLengthPixel;
    int m_clusterEdgeLengthTiles;

    QList<ReadOnlyMapDefinition> m_mapSourceDefinitions;
    QList<ReadOnlyMapImage *> m_mapSources;
    int m_mapSourceCount;
};

#endif
