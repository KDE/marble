#ifndef RENDEROSMTILECLUSTERTHREAD_H
#define RENDEROSMTILECLUSTERTHREAD_H

#include "mapreproject.h"
#include "ReadOnlyMapDefinition.h"

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtGui/QImage>

class ReadOnlyMapImage;

class OsmTileClusterRenderer: public QObject
{
    Q_OBJECT

public:
    explicit OsmTileClusterRenderer( QObject * const parent = NULL );

    void setClusterEdgeLengthTiles( int const clusterEdgeLengthTiles );
    void setMapSources( QVector<ReadOnlyMapDefinition> const & mapSources );
    void setOsmBaseDirectory( QDir const & osmBaseDirectory );
    void setOsmTileLevel( int const level );

signals:
    void clusterRendered( OsmTileClusterRenderer * );

public slots:
    void initMapSources();
    void renderOsmTileCluster( int const clusterX, int const clusterY );

private:
    QDir checkAndCreateDirectory( int const tileX ) const;
    QImage renderOsmTile( int const tileX, int const tileY );
    double osmPixelXtoLonRad( int const pixelX ) const;
    double osmPixelYtoLatRad( int const pixelY ) const;

    int const m_osmTileEdgeLengthPixel;
    QRgb const m_emptyPixel;

    QDir m_osmBaseDirectory;
    int m_osmTileLevel;
    int m_osmMapEdgeLengthTiles;
    int m_osmMapEdgeLengthPixel;
    int m_clusterEdgeLengthTiles;

    QVector<ReadOnlyMapDefinition> m_mapSourceDefinitions;
    QVector<ReadOnlyMapImage*> m_mapSources;
    int m_mapSourceCount;
    int m_tilesRenderedCount;
};

#endif
