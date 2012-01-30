#include "BilinearInterpolation.h"
#include "NasaWorldWindToOpenStreetMapConverter.h"
#include "NearestNeighborInterpolation.h"
#include "OsmTileClusterRenderer.h"
#include "ReadOnlyMapDefinition.h"
#include "Thread.h"
#include "mapreproject.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QPair>
#include <QtCore/QThread>
#include <QtCore/QVector>

#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

/* example usage
mapreproject --simulate --output-directory=/home/jmho --jobs 7 --cluster-size 64 --output-tile-level=10 \
    --input=type=NasaWW,base-directory=/home,tile-level=8,interpolation-method=bilinear,cache-size=200000000 \
    --input type=Bathymetry,file=BLAH.tiff
*/


MapSourceType parseType( char const * const value )
{
    MapSourceType result = UnknownMapSource;
    if ( !value )
        qFatal("Suboption 'type' does not have a value.");
    if ( strcmp( value, "NasaWW") == 0 )
        result = NasaWorldWindMap;
    else if ( strcmp( value, "Bathymetry" ) == 0 )
        result = BathymetryMap;
    else
        qFatal("Suboption 'type': Unrecognized value '%s'.", value );
    return result;
}

QString parseString( char const * const value )
{
    QString result;
    if ( !value )
        qFatal("Suboption does not have a value.");
    result = value;
    return result;
}

int parseInt( char const * const value )
{
    if ( !value )
        qFatal("Suboption does not have a value.");
    QString str( value );
    bool ok;
    int const result = str.toInt( &ok );
    if ( !ok )
        qFatal("Suboption does not have an integer value.");
    return result;
}

EInterpolationMethod parseInterpolationMethod( char const * const value )
{
    EInterpolationMethod result = UnknownInterpolationMethod;
    if ( !value )
        qFatal("Suboption 'interpolation-method' does not have a value.");
    if ( strcmp( value, "integer") == 0 )
        result = IntegerInterpolationMethod;
    if ( strcmp( value, "nearest-neighbor") == 0 )
        result = NearestNeighborInterpolationMethod;
    if ( strcmp( value, "average") == 0 )
        result = AverageInterpolationMethod;
    else if ( strcmp( value, "bilinear" ) == 0 )
        result =  BilinearInterpolationMethod;
    else
        qFatal("Suboption 'interpolation-method': Unrecognized value '%s'.", value );
    return result;
}

ReadOnlyMapDefinition parseInput( char * subopts )
{
    if ( !subopts )
        qFatal("Missing argument for '--input'");

    enum
    {
        TypeOption = 0,
        BaseDirectoryOption,
        FileOption,
        TileLevelOption,
        InterpolationOption,
        CacheSizeOption,
        TheEnd
    };

    char * const input_opts[] =
    {
        "type",
        "base-directory",
        "file",
        "tile-level",
        "interpolation-method",
        "cache-size",
        NULL
    };

    ReadOnlyMapDefinition mapDefinition;

    char * value;
    while ( *subopts != '\0' ) {
        switch ( getsubopt( &subopts, input_opts, &value )) {

        case TypeOption:
            mapDefinition.setMapType( parseType( value ));
            break;

        case BaseDirectoryOption:
            mapDefinition.setBaseDirectory( parseString( value ));
            break;

        case FileOption:
            mapDefinition.setFileName( parseString( value ));
            break;

        case TileLevelOption:
            mapDefinition.setTileLevel( parseInt( value ));
            break;

        case InterpolationOption:
            mapDefinition.setInterpolationMethod( parseInterpolationMethod( value ));
            break;

        case CacheSizeOption:
            mapDefinition.setCacheSizeBytes( parseInt( value ));
            break;

        default:
            qFatal("Unrecognized input suboption.");
        }
    }
    return mapDefinition;
}


int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    // --interpolation-method=NearestNeighbor|Bilinear

    QString outputDirectory;
    int outputTileLevel = -1;

    int threadCount = QThread::idealThreadCount();
    int clusterSize = 0; // cluster size 0 makes no sense
    bool onlySimulate = false;

    QVector<ReadOnlyMapDefinition> mapSources;

    // input: type, tile-level, base-dir|file
    // --input,type=NasaWW,tile-level=8,base-directory=<dir>,interpolation-method=Bilinear
    // --input,type=Image,file=<file>

    enum { HelpOption = 1,
           InputOption,
           OutputDirectoryOption,
           OutputTileLevelOption,
           JobsOption,
           ClusterSizeOption,
           SimulateOption };

    static struct option long_options[] = {
        {"help",              no_argument,       NULL, HelpOption },
        {"input",             required_argument, NULL, InputOption },
        {"output-directory",  required_argument, NULL, OutputDirectoryOption },
        {"output-tile-level", required_argument, NULL, OutputTileLevelOption },
        {"jobs",              required_argument, NULL, JobsOption },
        {"cluster-size",      required_argument, NULL, ClusterSizeOption },
        {"simulate",          no_argument,       NULL, SimulateOption },
        {0, 0, 0, 0 }
    };

    while ( true ) {
        int option_index = 0;

        int const opt = getopt_long( argc, argv, "", long_options, &option_index );
        if ( opt == -1 )
            break;

        switch ( opt ) {

        case HelpOption:
            break;

        case InputOption:
            mapSources.push_back( parseInput( optarg ));
            break;

        case OutputDirectoryOption:
            outputDirectory = parseString( optarg );
            break;

        case OutputTileLevelOption:
            outputTileLevel = parseInt( optarg );
            break;

        case JobsOption:
            threadCount = parseInt( optarg );
            break;

        case ClusterSizeOption:
            clusterSize = parseInt( optarg );
            break;

        case SimulateOption:
            onlySimulate = true;
            break;

        case '?':
            break;
        }
    }

    qDebug() << "\noutput directory:" << outputDirectory
             << "\noutput tile level:" << outputTileLevel
             << "\ncluster size:" << clusterSize
             << "\nthreads:" << threadCount
             << "\ninputs:" << mapSources;

    if (onlySimulate)
        return 0;

    NasaWorldWindToOpenStreetMapConverter converter;
    converter.setMapSources( mapSources );
    converter.setOsmBaseDirectory( QDir( outputDirectory ));
    converter.setOsmTileLevel( outputTileLevel );
    converter.setOsmTileClusterEdgeLengthTiles( clusterSize );
    converter.setThreadCount( threadCount );

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
