//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "GeoDataDocumentWriter.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QElapsedTimer>

#include <QMessageLogContext>

#include "LineStringProcessor.h"
#include "ShpCoastlineProcessor.h"
#include "TinyPlanetProcessor.h"
#include "NodeReducer.h"
#include "WayConcatenator.h"
#include "TileIterator.h"

using namespace Marble;

enum DebugLevel {
    Debug,
    Info,
    Mute
};

DebugLevel debugLevel = Info;

void debugOutput( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
    switch ( type ) {
    case QtDebugMsg:
        if ( debugLevel == Debug ) {
            qDebug() << "Debug: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtInfoMsg:
        if ( debugLevel < Mute ) {
            qInfo() << "Info: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtWarningMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Warning: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtCriticalMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Critical: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtFatalMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Fatal: " << context.file << ":" << context.line << " " << msg;
            abort();
        }
    }
}

GeoDataDocument* mergeDocuments(GeoDataDocument* map1, GeoDataDocument* map2)
{
    GeoDataDocument* mergedMap = new GeoDataDocument(*map1);

    foreach (GeoDataFeature* feature, map2->featureList()) {
        mergedMap->append(feature);
    }

    return mergedMap;
}

bool writeTile(const QCommandLineParser &parser, const QString &outputName, GeoDataDocument* tile, int x, int y, int zoomLevel)
{
    QString const extension = parser.value("extension");
    QString const baseDir = parser.isSet("output") ? (outputName + QLatin1Char('/')) : QString();
    QString const outputDir = QString("%1%2/%3").arg(baseDir).arg(zoomLevel).arg(x);
    QDir().mkpath(outputDir);
    QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(y).arg(extension);
    if (!GeoDataDocumentWriter::write(outputFile, *tile)) {
        qDebug() << "Could not write the file " << outputName;
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("osm-simplify");
    QApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("A tool for Marble, which is used to reduce the details of osm maps.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input", "The input .osm or .shp file.");

    parser.addOptions({
      {{"d","debug"}, "Debug output in the terminal."},
      {{"s","silent"}, "Don't output to terminal."},
      {{"m","merge"}, "Merge the main document with the file <file_to_merge_with>. This works together with the -c flag.", "file_to_merge_with"},
      {{"c", "cut-to-tiles"}, "Cuts into tiles based on the zoom level passed using -z."},
      {{"n", "node-reduce"}, "Reduces the number of nodes for a given way based on zoom level"},
      {{"z", "zoom-level"}, "Zoom level according to which OSM information has to be processed.", "number"},
      {{"t", "tags-filter"}, "Tag key-value pairs which are to be be considered", "k1=v1,k2=v2..."},
      {{"and", "tags-and"}, "For a feature to be considered for processing it must contain all the specified using tags-filter"},
      {{"w", "concat-ways"}, "Concatenates the ways which are specified using tags-filter"},
      {{"o", "output"}, "Generates an output .osmfile based on other flags. If the cut-to-tiles flag is set, then this needs to be a directory.", "output_file.osm"},
      {{"e", "extension"}, "Output file type: osm (default), o5m or kml", "file extension", "osm"}
    });

    // Process the actual command line arguments given by the user
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        parser.showHelp();
        return 0;
    }
    // input is args.at(0), output is args.at(1)

    QString inputFileName = args.at(0);
    QString mergeFileName = parser.value("merge");
    bool debug = parser.isSet("debug");
    bool silent = parser.isSet("silent");
    unsigned int zoomLevel = parser.value("zoom-level").toInt();
    qDebug()<<"Zoom level is "<<zoomLevel<<endl;

    QString outputName;
    if(parser.isSet("output")) {
        outputName = parser.value("output");
    } else {
        outputName = "s_" + inputFileName;
    }
    qDebug() << "Output file name is " << outputName << endl;

    if(debug) {
        debugLevel = Debug;
        qInstallMessageHandler( debugOutput );
    }
    if(silent) {
        debugLevel = Mute;
        qInstallMessageHandler( debugOutput );
    }


    QFileInfo file( inputFileName );
    if ( !file.exists() ) {
        qDebug() << "File " << file.absoluteFilePath() << " does not exist. Exiting.";
        return 2;
    }

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());

    QElapsedTimer timer;
    timer.start();

    // Timeout is set to 10 min. If the file is reaaally huge, set it to something bigger.
    GeoDataDocument* map = manager.openFile(inputFileName, DocumentRole::MapDocument, 600000);
    if(map == nullptr) {
        qWarning() << "File" << inputFileName << "couldn't be loaded.";
        return -2;
    }
    qint64 parsingTime = timer.elapsed();
    qDebug() << "Parsing time:" << parsingTime << "ms";

    GeoDataDocument* mergeMap = nullptr;
    if(parser.isSet("merge")) {
        mergeMap = manager.openFile(mergeFileName, DocumentRole::MapDocument, 600000);
    }

    if(file.suffix() == QLatin1String("shp") && parser.isSet("cut-to-tiles")) {
        ShpCoastlineProcessor processor(map);
        processor.process();
        GeoDataLatLonBox world(85.0, -85.0, 180.0, -180.0, GeoDataCoordinates::Degree);
        //TileIterator iter(map->latLonAltBox(), zoomLevel);
        TileIterator iter(world, zoomLevel);
        foreach(auto const &tileId, iter) {
            GeoDataDocument* tile = processor.cutToTiles(zoomLevel, tileId.x(), tileId.y());
            if (!writeTile(parser, outputName, tile, tileId.x(), tileId.y(), zoomLevel)) {
                return 4;
            }
            qInfo() << tile->name() << " done";
            delete tile;
        }
    } else if (file.suffix() == QLatin1String("osm") && parser.isSet("cut-to-tiles") && parser.isSet("merge")) {
        TinyPlanetProcessor processor(map);
        processor.process();
        ShpCoastlineProcessor shpProcessor(mergeMap);
        shpProcessor.process();
        GeoDataLatLonBox world(85.0, -85.0, 180.0, -180.0, GeoDataCoordinates::Degree);
        //TileIterator iter(map->latLonAltBox(), zoomLevel);
        TileIterator iter(world, zoomLevel);
        foreach(auto const &tileId, iter) {
            GeoDataDocument* tile1 = processor.cutToTiles(zoomLevel, tileId.x(), tileId.y());
            GeoDataDocument* tile2 = shpProcessor.cutToTiles(zoomLevel, tileId.x(), tileId.y());
            GeoDataDocument* tile = mergeDocuments(tile1, tile2);
            if (!writeTile(parser, outputName, tile, tileId.x(), tileId.y(), zoomLevel)) {
                return 4;
            }
            qInfo() << tile->name() << " done";
            delete tile1;
            delete tile2;
            delete tile;
        }
    } else if (file.suffix() == QLatin1String("osm") && parser.isSet("cut-to-tiles")) {
        TinyPlanetProcessor processor(map);

        processor.process();

        GeoDataLatLonBox world(85.0, -85.0, 180.0, -180.0, GeoDataCoordinates::Degree);
        //TileIterator iter(map->latLonAltBox(), zoomLevel);
        TileIterator iter(world, zoomLevel);
        foreach(auto const &tileId, iter) {
            GeoDataDocument* tile = processor.cutToTiles(zoomLevel, tileId.x(), tileId.y());
            if (!writeTile(parser, outputName, tile, tileId.x(), tileId.y(), zoomLevel)) {
                return 4;
            }
            qInfo() << tile->name() << " done";
            delete tile;
        }
    } else if(parser.isSet("node-reduce")) {
        qDebug()<<"Entered Node reduce"<<endl;
        qDebug()<<"Finished Processing"<<endl;
        if(!parser.isSet("zoom-level")) {
            qDebug()<<" Zoom level not set"<<endl;
        }
        else{
            NodeReducer reducer(map, zoomLevel);
            reducer.process();
            if (!GeoDataDocumentWriter::write(outputName, *map)) {
                qDebug() << "Could not write the file " << outputName;
                return 4;
            }

        }
    } else if(parser.isSet("tags-filter") && parser.isSet("concat-ways")) {


        //Parses the tags given at command line and makes a Hash of key-value pairs
        qDebug()<<" Parsed tf value: "<<parser.value("tags-filter")<<endl;
        QStringList tagsList = parser.value("tags-filter").split(QLatin1Char(','));
        //Filters and considers only those placemarks which have all the key-value pairs given at command line 

        WayConcatenator concatenator(map, tagsList, parser.isSet("tags-and"));
        concatenator.process();

        qDebug()<<"Concatenation done, writing results to the file";
        
        if (!GeoDataDocumentWriter::write(outputName, *map)) {
            qDebug() << "Could not write the file " << outputName;
            return 4;
        }else{
            qDebug()<<"File written";
        }


    }
    else {
    }

    delete map;


    return 0;
}
