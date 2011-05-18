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

int main( int argc, char *argv[] )
{
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

    if ( arguments.size() != 4 ) {
        qDebug() << "Usage: " << argv[0] << " /path/to/input.(osm|pbf) AreaName /path/to/output.sqlite";
        return 1;
    }

    qInstallMsgHandler( debugOutput );
    QFileInfo file( arguments.at( 1 ) );
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
    SqlWriter sql( arguments.at( 3 ) );
    parser->addWriter( &sql );
    parser->read( file, arguments.at( 2 ) );
}
