//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "GeoWriter.h"
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

void usage()
{
    qDebug() << "Usage: osm-simplify [options] input.osm output.osm";
    qDebug() << "\t--no-streets-smaller-than %f - eliminates streets which have realsize smaller than %f";
}

GeoDataDocument* mergeDocuments(GeoDataDocument* map1, GeoDataDocument* map2)
{
    GeoDataDocument* mergedMap = new GeoDataDocument(*map1);

    foreach (GeoDataFeature* feature, map2->featureList()) {
        mergedMap->append(feature);
    }

    return mergedMap;
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
                          {
                              {"d","debug"},
                              QCoreApplication::translate("main", "Debug output in the terminal.")
                          },

                          {
                              {"s","silent"},
                              QCoreApplication::translate("main", "Don't output to terminal.")
                          },

                          {
                              {"m","merge"},
                              QCoreApplication::translate("main", "Merge the main document with the file <file_to_merge_with>. This works together with the -c flag."),
                              QCoreApplication::translate("main", "file_to_merge_with")
                          },

                          {
                              {"c", "cut-to-tiles"},
                              QCoreApplication::translate("main", "Cuts into tiles based on the zoom level passed using -z."),
                          },

                          {
                              {"n", "node-reduce"},
                              QCoreApplication::translate("main", "Reduces the number of nodes for a given way based on zoom level"),

                          },

                          {
                              {"z", "zoom-level"},
                              QCoreApplication::translate("main", "Zoom level according to which OSM information has to be processed."),
                              QCoreApplication::translate("main", "number")
                          },

                          {
                              {"o", "output"},
                              QCoreApplication::translate("main", "Generates an output .osmfile based on other flags. If the cut-to-tiles flag is set, then this needs to be a directory."),
                              QCoreApplication::translate("main", "output_file.osm")
                          }
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

    if(file.suffix() == "shp" && parser.isSet("cut-to-tiles")) {
        ShpCoastlineProcessor processor(map);

        processor.process();

        unsigned int N = pow(2, zoomLevel);

        for(unsigned int x = 0; x < N; ++x) {
            for(unsigned int y = 0; y < N; ++y) {
                GeoDataDocument* tile = processor.cutToTiles(zoomLevel, x, y);

                GeoWriter writer;
                writer.setDocumentType("0.6");

                QFile outputFile;
                if(parser.isSet("output")) {
                    outputFile.setFileName( QString("%1/%2/%3/%4.osm").arg(outputName).arg(zoomLevel).arg(x).arg(y) );
                } else {
                    outputFile.setFileName( QString("%1/%2/%3.osm").arg(zoomLevel).arg(x).arg(y) );
                }

                QDir dir;
                if(parser.isSet("output")) {
                    if(!dir.exists(outputName)) {
                        dir.mkdir(outputName);
                    }

                    if(!dir.exists(QString("%1/%2").arg(outputName).arg(zoomLevel))) {
                        dir.mkdir(QString("%1/%2").arg(outputName).arg(zoomLevel));
                    }

                    if(!dir.exists(QString("%1/%2/%3").arg(outputName).arg(zoomLevel).arg(x))) {
                        dir.mkdir(QString("%1/%2/%3").arg(outputName).arg(zoomLevel).arg(x));
                    }
                } else {
                    if(!dir.exists(QString::number(zoomLevel))) {
                        dir.mkdir(QString::number(zoomLevel));
                    }
                    if(!dir.exists(QString("%1/%2").arg(zoomLevel).arg(x))) {
                        dir.mkdir(QString("%1/%2").arg(zoomLevel).arg(x));
                    }
                }

                outputFile.open( QIODevice::WriteOnly );
                if ( !writer.write( &outputFile, tile ) ) {
                    qDebug() << "Could not write the file " << outputName;
                    return 4;
                }

                qInfo() << tile->name() << " done";

                delete tile;
            }
        }
    } else if (file.suffix() == "osm" && parser.isSet("cut-to-tiles") && parser.isSet("merge")) {
        TinyPlanetProcessor processor(map);
        processor.process();

        ShpCoastlineProcessor shpProcessor(mergeMap);
        shpProcessor.process();

        unsigned int N = pow(2, zoomLevel);

        for(unsigned int x = 0; x < N; ++x) {
            for(unsigned int y = 0; y < N; ++y) {
                GeoDataDocument* tile1 = processor.cutToTiles(zoomLevel, x, y);
                GeoDataDocument* tile2 = shpProcessor.cutToTiles(zoomLevel, x, y);

                GeoDataDocument* tile = mergeDocuments(tile1, tile2);

                GeoWriter writer;
                writer.setDocumentType("0.6");

                QFile outputFile;
                if(parser.isSet("output")) {
                    outputFile.setFileName( QString("%1/%2/%3/%4.osm").arg(outputName).arg(zoomLevel).arg(x).arg(y) );
                } else {
                    outputFile.setFileName( QString("%1/%2/%3.osm").arg(zoomLevel).arg(x).arg(y) );
                }

                QDir dir;
                if(parser.isSet("output")) {
                    if(!dir.exists(outputName)) {
                        dir.mkdir(outputName);
                    }

                    if(!dir.exists(QString("%1/%2").arg(outputName).arg(zoomLevel))) {
                        dir.mkdir(QString("%1/%2").arg(outputName).arg(zoomLevel));
                    }

                    if(!dir.exists(QString("%1/%2/%3").arg(outputName).arg(zoomLevel).arg(x))) {
                        dir.mkdir(QString("%1/%2/%3").arg(outputName).arg(zoomLevel).arg(x));
                    }
                } else {
                    if(!dir.exists(QString::number(zoomLevel))) {
                        dir.mkdir(QString::number(zoomLevel));
                    }
                    if(!dir.exists(QString("%1/%2").arg(zoomLevel).arg(x))) {
                        dir.mkdir(QString("%1/%2").arg(zoomLevel).arg(x));
                    }
                }

                outputFile.open( QIODevice::WriteOnly );
                if ( !writer.write( &outputFile, tile ) ) {
                    qDebug() << "Could not write the file " << outputName;
                    return 4;
                }

                qInfo() << tile->name() << " done";

                delete tile1;
                delete tile2;
                delete tile;
            }
        }
    } else if (file.suffix() == "osm" && parser.isSet("cut-to-tiles")) {
        TinyPlanetProcessor processor(map);

        processor.process();

        unsigned int N = pow(2, zoomLevel);

        for(unsigned int x = 0; x < N; ++x) {
            for(unsigned int y = 0; y < N; ++y) {
                GeoDataDocument* tile = processor.cutToTiles(zoomLevel, x, y);

                GeoWriter writer;
                writer.setDocumentType("0.6");

                QFile outputFile;
                if(parser.isSet("output")) {
                    outputFile.setFileName( QString("%1/%2/%3/%4.osm").arg(outputName).arg(zoomLevel).arg(x).arg(y) );
                } else {
                    outputFile.setFileName( QString("%1/%2/%3.osm").arg(zoomLevel).arg(x).arg(y) );
                }

                QDir dir;
                if(parser.isSet("output")) {
                    if(!dir.exists(outputName)) {
                        dir.mkdir(outputName);
                    }

                    if(!dir.exists(QString("%1/%2").arg(outputName).arg(zoomLevel))) {
                        dir.mkdir(QString("%1/%2").arg(outputName).arg(zoomLevel));
                    }

                    if(!dir.exists(QString("%1/%2/%3").arg(outputName).arg(zoomLevel).arg(x))) {
                        dir.mkdir(QString("%1/%2/%3").arg(outputName).arg(zoomLevel).arg(x));
                    }
                } else {
                    if(!dir.exists(QString::number(zoomLevel))) {
                        dir.mkdir(QString::number(zoomLevel));
                    }
                    if(!dir.exists(QString("%1/%2").arg(zoomLevel).arg(x))) {
                        dir.mkdir(QString("%1/%2").arg(zoomLevel).arg(x));
                    }
                }

                outputFile.open( QIODevice::WriteOnly );
                if ( !writer.write( &outputFile, tile ) ) {
                    qDebug() << "Could not write the file " << outputName;
                    return 4;
                }

                qInfo() << tile->name() << " done";

                delete tile;
            }
        }
    } else if(parser.isSet("node-reduce")) {
        qDebug()<<"Entered Node reduce"<<endl;
        qDebug()<<"Finished Processing"<<endl;
        if(!parser.isSet("zoom-level")){
            qDebug()<<" Zoom level not set"<<endl;
        }
        else{
            NodeReducer reducer(map, zoomLevel);
            reducer.process();
            QFile outputFile(outputName);
            GeoWriter writer;
            writer.setDocumentType("0.6");

            outputFile.open( QIODevice::WriteOnly );
            if ( !writer.write( &outputFile, map ) ) {
                qDebug() << "Could not write the file " << outputName;
                return 4;
            }

        }
    } else {
    }


    return 0;
}
