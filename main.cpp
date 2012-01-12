#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QPair>
#include <QtCore/QVector>

#include "NasaWorldWindToOpenStreetMapConverter.h"
#include "OsmTileClusterRenderer.h"
#include "Thread.h"

int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    // TODO: use cmd line args
    QDir const nwwBaseDirectory( "/home/jmho/mapdata/nasa-worldwind/4" );
    int const nwwTileLevel = 8;
    QDir const osmBaseDirectory( "/home/jmho/mapdata/nasa-worldwind/osm" );
    int const osmTileLevel = 13;

    NasaWorldWindToOpenStreetMapConverter converter;
    converter.setNwwBaseDirectory( nwwBaseDirectory );
    converter.setNwwTileLevel( nwwTileLevel );
    converter.setOsmBaseDirectory( osmBaseDirectory );
    converter.setOsmTileLevel( osmTileLevel );
    QObject::connect( &converter, SIGNAL(finished()), &app, SLOT(quit()));

    QVector<QPair<Thread*, OsmTileClusterRenderer*> > renderThreads = converter.start();
    app.exec();

    QVector<QPair<Thread*, OsmTileClusterRenderer*> >::iterator pos = renderThreads.begin();
    QVector<QPair<Thread*, OsmTileClusterRenderer*> >::iterator const end = renderThreads.end();
    for (; pos != end; ++pos ) {
        (*pos).first->stop();
        (*pos).first->wait();
        delete (*pos).second;
    }

    return 0;
}
