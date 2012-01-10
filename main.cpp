#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

#include "NasaWorldWindToOpenStreetMapConverter.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    NasaWorldWindToOpenStreetMapConverter converter;

    QDir sourcePath( "/home/jmho/mapdata/nasa-worldwind/4" );
    converter.setSourcePath( sourcePath );
    QDir destinationPath( "/home/jmho/mapdata/nasa-worldwind/osm" );
    converter.setDestinationPath( destinationPath );

    converter.setNwwTileLevel( 8 );
    converter.setOsmTileLevel( 12 );

    QObject::connect( &converter, SIGNAL(finished()), &app, SLOT(quit()));
    converter.testReprojection();
    converter.start();

    return app.exec();
}
