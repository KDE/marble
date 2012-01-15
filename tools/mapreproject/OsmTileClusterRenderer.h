#ifndef RENDEROSMTILECLUSTERTHREAD_H
#define RENDEROSMTILECLUSTERTHREAD_H

#include "NwwMapImage.h"

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtGui/QImage>

class OsmTileClusterRenderer: public QObject
{
    Q_OBJECT

public:
    explicit OsmTileClusterRenderer( QObject * const parent = NULL );

    void setClusterEdgeLengthTiles( int const clusterEdgeLengthTiles );
    void setNwwBaseDirectory( QDir const & nwwBaseDirectory );
    void setNwwInterpolationMethod( InterpolationMethod const interpolationMethod );
    void setNwwTileLevel( int const level );
    void setOsmBaseDirectory( QDir const & osmBaseDirectory );
    void setOsmTileLevel( int const level );

signals:
    void clusterRendered( OsmTileClusterRenderer * );

public slots:
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

    NwwMapImage m_nwwMapImage;
    int m_clusterEdgeLengthTiles;
};

#endif
