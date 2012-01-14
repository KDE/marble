#include "NasaWorldWindToOpenStreetMapConverter.h"

#include "OsmTileClusterRenderer.h"
#include "Thread.h"

#include <QtCore/QDebug>
#include <QtCore/QMetaObject>
#include <QtCore/QString>
#include <QtCore/QThread>

#include <cmath>

NasaWorldWindToOpenStreetMapConverter::NasaWorldWindToOpenStreetMapConverter( QObject * const parent )
    : QObject( parent ),
      m_threadCount(),
      m_nwwTileLevel(),
      m_nwwInterpolationMethod(),
      m_osmTileLevel(),
      m_osmTileClusterEdgeLengthTiles(),
      m_osmMapEdgeLengthClusters(),
      m_nextClusterX(),
      m_nextClusterY()
{
}

void NasaWorldWindToOpenStreetMapConverter::setNwwBaseDirectory( QDir const & nwwBaseDirectory )
{
    m_nwwBaseDirectory = nwwBaseDirectory;
}

void NasaWorldWindToOpenStreetMapConverter::setNwwInterpolationMethod( InterpolationMethod const interpolationMethod )
{
    m_nwwInterpolationMethod = interpolationMethod;
}

void NasaWorldWindToOpenStreetMapConverter::setNwwTileLevel( int const level )
{
    m_nwwTileLevel = level;
}

void NasaWorldWindToOpenStreetMapConverter::setOsmBaseDirectory( QDir const & osmBaseDirectory )
{
    if ( !osmBaseDirectory.exists() ) {
        qDebug() << "Destination path " << osmBaseDirectory << "does not exist, creating.";
        osmBaseDirectory.mkpath( osmBaseDirectory.path() );
    }
    m_osmBaseDirectory = osmBaseDirectory;
}

void NasaWorldWindToOpenStreetMapConverter::setOsmTileClusterEdgeLengthTiles( int const clusterEdgeLengthTiles )
{
    m_osmTileClusterEdgeLengthTiles = clusterEdgeLengthTiles;
}

void NasaWorldWindToOpenStreetMapConverter::setOsmTileLevel( int const level )
{
    m_osmTileLevel = level;
}

void NasaWorldWindToOpenStreetMapConverter::setThreadCount(const int threadCount)
{
    m_threadCount = threadCount;
}

QVector<QPair<Thread*, OsmTileClusterRenderer*> > NasaWorldWindToOpenStreetMapConverter::start()
{
    // create directory for osm tile level if necessary
    QDir const levelDirectory( m_osmBaseDirectory.path() + QString("/%1").arg( m_osmTileLevel ));
    if ( !levelDirectory.exists() ) {
        bool const created = levelDirectory.mkpath( levelDirectory.path() );
        if ( !created ) {
            // maybe it was created in the meantime by something else
            if ( !levelDirectory.exists() )
                qFatal("Unable to create directory '%s'.", levelDirectory.path().toStdString().c_str() );
        }
    }

    // render Osm tiles using quadratic clusters of tiles instead of stripes
    // to increase Nww tile cache usage
    int const osmMapEdgeLengthTiles = pow( 2, m_osmTileLevel );
    m_osmMapEdgeLengthClusters = osmMapEdgeLengthTiles / m_osmTileClusterEdgeLengthTiles;
    if ( osmMapEdgeLengthTiles % m_osmTileClusterEdgeLengthTiles != 0 )
        qFatal("Bad tile cluster size");

    QVector<QPair<Thread*, OsmTileClusterRenderer*> > renderThreads;

    for ( int i = 0; i < m_threadCount; ++i ) {
        OsmTileClusterRenderer * const renderer = new OsmTileClusterRenderer;
        renderer->setObjectName( QString("Renderer %1").arg( i ));
        renderer->setClusterEdgeLengthTiles( m_osmTileClusterEdgeLengthTiles );
        renderer->setNwwBaseDirectory( m_nwwBaseDirectory );
        renderer->setNwwInterpolationMethod( m_nwwInterpolationMethod );
        renderer->setNwwTileLevel( m_nwwTileLevel );
        renderer->setOsmBaseDirectory( m_osmBaseDirectory );
        renderer->setOsmTileLevel( m_osmTileLevel );
        QObject::connect( renderer, SIGNAL(clusterRendered(OsmTileClusterRenderer*)),
                          this, SLOT(assignNextCluster(OsmTileClusterRenderer*)) );

        Thread * const thread = new Thread;
        thread->launchWorker( renderer );
        QMetaObject::invokeMethod( renderer, "renderOsmTileCluster", Qt::QueuedConnection,
                                   Q_ARG( int, m_nextClusterX ), Q_ARG( int, m_nextClusterY ));
        incNextCluster();
        renderThreads.push_back( qMakePair( thread, renderer ));
    }
    return renderThreads;
}

void NasaWorldWindToOpenStreetMapConverter::testReprojection()
{
//    qDebug() << "\nTesting osm pixel x -> lon[rad]";
//    qDebug() << 0 << "->" << osmPixelXtoLonRad( 0 );
//    qDebug() << m_osmMapEdgeLengthPixel / 4 << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel / 4 );
//    qDebug() << m_osmMapEdgeLengthPixel / 2 << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel / 2 );
//    qDebug() << m_osmMapEdgeLengthPixel / 4 * 3 << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel / 4 * 3 );
//    qDebug() << m_osmMapEdgeLengthPixel << "->" << osmPixelXtoLonRad( m_osmMapEdgeLengthPixel );

//    qDebug() << "\nTesting osm pixel y -> lat[rad]";
//    qDebug() << 0 << "->" << osmPixelYtoLatRad( 0 );
//    qDebug() << m_osmMapEdgeLengthPixel / 4 << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel / 4 );
//    qDebug() << m_osmMapEdgeLengthPixel / 2 << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel / 2 );
//    qDebug() << m_osmMapEdgeLengthPixel / 4 * 3 << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel / 4 * 3 );
//    qDebug() << m_osmMapEdgeLengthPixel << "->" << osmPixelYtoLatRad( m_osmMapEdgeLengthPixel );

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

void NasaWorldWindToOpenStreetMapConverter::assignNextCluster( OsmTileClusterRenderer * renderer )
{
    if ( m_nextClusterX == m_osmMapEdgeLengthClusters || m_nextClusterY == m_osmMapEdgeLengthClusters )
        return;

    QMetaObject::invokeMethod( renderer, "renderOsmTileCluster", Qt::QueuedConnection,
                               Q_ARG( int, m_nextClusterX ), Q_ARG( int, m_nextClusterY ));
    incNextCluster();
}

void NasaWorldWindToOpenStreetMapConverter::incNextCluster()
{
    ++m_nextClusterY;
    if ( m_nextClusterY == m_osmMapEdgeLengthClusters ) {
        m_nextClusterY = 0;
        ++m_nextClusterX;
        if ( m_nextClusterX == m_osmMapEdgeLengthClusters )
            emit finished();
    }
}
