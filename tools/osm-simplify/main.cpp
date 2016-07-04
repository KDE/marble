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
#include <QTime>
#include <QDir>
#include <QString>

#include <QMessageLogContext>

#include "LineStringProcessor.h"
#include "ShpCoastlineProcessor.h"

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
                              {"m","mute"},
                              QCoreApplication::translate("main", "Don't output to terminal.")
                          },

                          {
                              {"c", "cut-to-tiles"},
                              QCoreApplication::translate("main", "Cuts into tiles based on the zoom level passed by <number>"),
                              QCoreApplication::translate("main", "number")
                          },

                          {
                              {"o", "output"},
                              QCoreApplication::translate("main", "Generates an output .osmfile based on other flags. This won't work together with the cut-to-tiles flag."),
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
    bool debug = parser.isSet("debug");
    bool mute = parser.isSet("mute");
    unsigned int zoomLevel = parser.value("cut-to-tiles").toInt();
    QString outputFileName = parser.value("output");

    if(debug) {
        debugLevel = Debug;
        qInstallMessageHandler( debugOutput );
    }
    if(mute) {
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
    GeoDataDocument* map = manager.openFile(inputFileName, DocumentRole::MapDocument);

    if(file.suffix() == "shp" && parser.isSet("cut-to-tiles")) {
        ShpCoastlineProcessor processor(map);

        processor.process();

        unsigned int N = pow(2, zoomLevel);

        for(unsigned int x = 0; x < N; ++x) {
            for(unsigned int y = 0; y < N; ++y) {
                GeoDataDocument* tile = processor.cutToTiles(zoomLevel, x, y);

                GeoWriter writer;
                writer.setDocumentType("0.6");

                QFile outputFile( QString("%1/%2/%3.osm").arg(zoomLevel).arg(x).arg(y) );

                QDir dir;
                if(!dir.exists(QString::number(zoomLevel))) {
                    dir.mkdir(QString::number(zoomLevel));
                }
                if(!dir.exists(QString("%1/%2").arg(zoomLevel).arg(x))) {
                    dir.mkdir(QString("%1/%2").arg(zoomLevel).arg(x));
                }

                outputFile.open( QIODevice::WriteOnly );
                if ( !writer.write( &outputFile, tile ) ) {
                    qDebug() << "Could not write the file " << outputFileName;
                    return 4;
                }

                qInfo() << tile->name() << " done";

                delete tile;
            }
        }
    } else {
        // future functionality
    }


    return 0;
}
