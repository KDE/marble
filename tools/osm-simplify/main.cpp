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
#include <QDebug>
#include <QFileInfo>
#include <QTime>

#include <QMessageLogContext>

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
    qDebug() << "\t-q quiet";
    qDebug() << "\t-v debug output";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if ( argc < 3 ) {
        usage();
        return 1;
    }

    QString inputSrc = argv[argc-2];
    QString outputSrc = argv[argc-1];

    for ( int i=1; i< argc-2; ++i ) {
        QString arg( argv[i] );
        if ( arg == "-v" ) {
            debugLevel = Debug;
        } else if ( arg == "-q" ) {
            debugLevel = Mute;
        } else {
            usage();
            return 1;
        }
    }

    qInstallMessageHandler( debugOutput );

    QFileInfo file( inputSrc );
    if ( !file.exists() ) {
        qDebug() << "File " << file.absoluteFilePath() << " does not exist. Exiting.";
        return 2;
    }

    if ( file.suffix() == "osm") {

        QString error;
        Marble::GeoDataDocument* osmMap = OsmParser::parse(inputSrc, error);

        if(!error.isEmpty()) {
            qDebug() << error;
            return 3;
        }

        Marble::GeoWriter writer;
        writer.setDocumentType("0.6");

        QFile outputfile( outputSrc );
        outputfile.open( QIODevice::WriteOnly );
        if ( !writer.write( &outputfile, osmMap ) ) {
            qDebug() << "Could not write the file " << outputSrc;
            return 4;
        }

        qDebug() << "Done.";
    } else {
        qDebug() << "Unsupported file format: " << outputSrc;
        return 5;
    }

    return 0;
}
