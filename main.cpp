#include "NasaWorldWindToOpenStreetMapConverter.h"
#include "OsmTileClusterRenderer.h"
#include "Thread.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QPair>
#include <QtCore/QVector>

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

namespace po = boost::program_options;

int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    // --interpolation-method=NearestNeighbor|Bilinear

    QString inputDirectory;
    int inputTileLevel = -1; // tile level -1 makes no sense

    QString outputDirectory;
    int outputTileLevel = -1;

    int threadCount = 0; // threads count 0 makes no sense
    int clusterSize = 0; // cluster size 0 makes no sense

    int opt;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("input-directory", po::value<std::string>(), "input base directory")
            ("input-tile-level", po::value<int>(), "tile level of input map")
            ("output-directory", po::value<std::string>(),
             "output base directory, where the resulting tiles will be stored")
            ("output-tile-level", po::value<int>(), "tile level of resulting map")
            ("jobs", po::value<int>( &opt )->default_value( QThread::idealThreadCount() ),
             "number of threads, use to override default of one thread per cpu core")
            ("cluster-size", po::value<int>( &opt )->default_value( 64 ),
             "edge length of tile clusters in tiles")
            ("interpolation-method", "method used for interpolating between pixels");

    po::variables_map variables;
    po::store( po::parse_command_line( argc, argv, desc ), variables );
    po::notify( variables );

    if ( variables.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    // mandatory arguments
    if ( variables.count("input-directory"))
        inputDirectory = variables["input-directory"].as<std::string>().c_str();
    else
        qFatal("Mandatory argument '--input-directory' missing");

    if ( variables.count("input-tile-level"))
        inputTileLevel = variables["input-tile-level"].as<int>();
    else
        qFatal("Mandatory argument '--input-tile-level' missing");

    if ( variables.count("output-directory"))
        outputDirectory = variables["output-directory"].as<std::string>().c_str();
    else
        qFatal("Mandatory argument '--output-directory' missing");

    if ( variables.count("output-tile-level"))
        outputTileLevel = variables["output-tile-level"].as<int>();
    else
        qFatal("Mandatory argument '--output-tile-level' missing");

    // optional arguments
    if ( variables.count("cluster-size"))
        clusterSize = variables["cluster-size"].as<int>();
    if ( variables.count("jobs"))
        threadCount = variables["jobs"].as<int>();

    qDebug() << "input directory:" << inputDirectory
             << "\ninput tile level:" << inputTileLevel
             << "\noutput directory:" << outputDirectory
             << "\noutput tile level:" << outputTileLevel;

    NasaWorldWindToOpenStreetMapConverter converter;
    converter.setNwwBaseDirectory( QDir( inputDirectory ));
    converter.setNwwTileLevel( inputTileLevel );
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
