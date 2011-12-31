//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "SqlWriter.h"
#include "pbf/PbfParser.h"
#include "xml/XmlParser.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>

using namespace Marble;

enum DebugLevel {
    Debug,
    Info,
    Mute
};

DebugLevel debugLevel = Info;

void debugOutput( QtMsgType type, const char *msg )
{
    switch ( type ) {
    case QtDebugMsg:
        if ( debugLevel == Debug ) {
            fprintf( stderr, "Debug: %s\n", msg );
        }
        break;
    case QtWarningMsg:
        if ( debugLevel < Mute ) {
            fprintf( stderr, "Info: %s\n", msg );
        }
        break;
    case QtCriticalMsg:
        if ( debugLevel < Mute ) {
            fprintf( stderr, "Warning: %s\n", msg );
        }
        break;
    case QtFatalMsg:
        if ( debugLevel < Mute ) {
            fprintf( stderr, "Fatal: %s\n", msg );
            abort();
        }
    }
}

void usage()
{
    qDebug() << "Usage: [options] osm-addresses [options] input.osm.pbf|input.osm output.sqlite output.kml";
    qDebug() << "\tOptions affect verbosity and store additional metadata in output.kml:";
    qDebug() << "\t-q quiet";
    qDebug() << "\t-v debug output";
    qDebug() << "\t--version aVersion";
    qDebug() << "\t--name aName";
    qDebug() << "\t--date aDate";
    qDebug() << "\t--payload aFilename";
}

int main( int argc, char *argv[] )
{
    if ( argc < 4 ) {
        usage();
        return 1;
    }

    QCoreApplication app( argc, argv );

    QStringList allArguments = app.arguments();
    QStringList arguments;
    foreach( const QString & arg, allArguments ) {
        if ( arg == "-v" ) {
            debugLevel = Debug;
        } else if ( arg == "-q" ) {
            debugLevel = Mute;
        } else {
            arguments << arg;
        }
    }

    QString inputFile = argv[argc-3];
    QString outputSqlite = argv[argc-2];
    QString outputKml = argv[argc-1];
    QString name;
    QString version;
    QString date;
    QString transport;
    QString payload;
    for ( int i=1; i<argc-3; ++i ) {
        QString arg( argv[i] );
        if ( arg == "--name" ) {
            name = argv[++i];
        } else if ( arg == "--version" ) {
            version = argv[++i];
        } else if ( arg == "--date" ) {
            date = argv[++i];
        } else if ( arg == "--transport" ) {
            transport = argv[++i];
        } else if ( arg == "--payload" ) {
            payload = argv[++i];
        } else {
            usage();
            return 1;
        }
    }

    qInstallMsgHandler( debugOutput );
    QFileInfo file( inputFile );
    if ( !file.exists() ) {
        qDebug() << "File " << file.absoluteFilePath() << " does not exist. Exiting.";
        return 2;
    }

    OsmParser* parser = 0;
    if ( file.fileName().endsWith( ".osm" ) ) {
        parser = new XmlParser;
    } else if ( file.fileName().endsWith( ".pbf" ) ) {
        parser = new PbfParser;
    } else {
        qDebug() << "Unsupported file format: " << file.fileName();
        return 3;
    }

    Q_ASSERT( parser );
    SqlWriter sql( outputSqlite );
    parser->addWriter( &sql );
    parser->read( file, name );
    parser->writeKml( name, version, date, transport, payload, outputKml );
}
