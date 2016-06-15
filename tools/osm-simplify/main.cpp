//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "OsmParser.h"
#include "GeoWriter.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QTime>

#include <QMessageLogContext>

#include "LineStringProcessor.h"

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
            qDebug() << "Debug: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtWarningMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Info: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtCriticalMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Warning: " << context.file << ":" << context.line << " " << msg;
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
    QCoreApplication app(argc, argv);

    QCoreApplication::setApplicationName("osm-simplify");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("A tool for Marble, which is used to reduce the details of osm maps.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input.osm", "The input .osm file.");
    parser.addPositionalArgument("output.osm", "The output .osm file.");

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
                              {"ns", "no-streets-smaller-than"},
                              QCoreApplication::translate("main", "eliminates streets which have realsize smaller than <real number>"),
                              QCoreApplication::translate("main", "real number")
                          },
                      });

    // Process the actual command line arguments given by the user
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    // input is args.at(0), output is args.at(1)

    QString inputFileName = args.at(0);
    QString outputFileName = args.at(1);

    bool debug = parser.isSet("debug");
    bool mute = parser.isSet("mute");
    QString smallStreetLimit = parser.value("no-streets-smaller-than"); // just an example

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

    GeoDataDocument* osmMap;
    if ( file.suffix() == "osm") {

        QString error;
        osmMap = OsmParser::parse(inputFileName, error);

        if(!error.isEmpty()) {
            qDebug() << error;
            return 3;
        }
    } else {
        qDebug() << "Unsupported file format: " << inputFileName;
        return 5;
    }

    LineStringProcessor processor(osmMap);

    processor.process();

    GeoWriter writer;
    writer.setDocumentType("0.6");

    QFile outputFile( outputFileName );
    outputFile.open( QIODevice::WriteOnly );
    if ( !writer.write( &outputFile, osmMap ) ) {
        qDebug() << "Could not write the file " << outputFileName;
        return 4;
    }

    qInfo() << "Done.";

    return 0;
}
