//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "routing/instructions/WaypointParser.h"
#include "routing/instructions/InstructionTransformation.h"
#include "routing/instructions/RoutingInstruction.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

using namespace Marble;

QString adjustGosmoreVersion( QTextStream &stream, WaypointParser &parser )
{
    QString content = stream.readAll();
    if ( !QCoreApplication::instance()->arguments().contains( "--routino" ) ) {
        QStringList lines = content.split( '\r' );
        if ( lines.size() > 2 ) {
            QStringList fields = lines.at( lines.size()-2 ).split(',');
            parser.setFieldIndex( WaypointParser::RoadType, fields.size()-3 );
            parser.setFieldIndex( WaypointParser::TotalSecondsRemaining, fields.size()-2 );
            parser.setFieldIndex( WaypointParser::RoadName, fields.size()-1 );
        }
    }
    return content;
}

void loadTranslations( QCoreApplication &app, QTranslator &translator )
{
    const QString lang = QLocale::system().name();
    QString code;

    int index = lang.indexOf ( '_' );
    if ( lang == "C" ) {
        code = "en";
    }
    else if ( index != -1 ) {
        code = lang.left ( index );
    }
    else {
        index = lang.indexOf ( '@' );
        if ( index != -1 )
            code = lang.left ( index );
        else
            code = lang;
    }

    QString const i18nDir = "/usr/share/marble/translations";
    QString const relativeDir = app.applicationDirPath() + "/translations";
    foreach( const QString &path, QStringList() << i18nDir << relativeDir << QDir::currentPath() ) {
        foreach( const QString &lang, QStringList() << lang << code ) {
            QFileInfo translations = QFileInfo( path + "/routing-instructions_" + lang + ".qm" );
            if ( translations.exists() && translator.load( translations.absoluteFilePath() ) ) {
                app.installTranslator( &translator );
                return;
            }
        }
    }
}

void usage()
{
    QTextStream console( stderr );
    console << "Usage: routing-instructions [options] [file]\n";
    console << '\n' << "file should be a text file with gosmore or routino output.";
    console << " If file is not given, stdin is read.";
    console << "\nOptions:\n";
    console << "\t--routino\t\tParse routino output. When not given, gosmore format is assumed\n";
    console << "\t--dense\t\t\tReplicate the gosmore output format and only exchange road names with driving instructions\n";
    console << "\t--csv\t\t\tUse csv output format\n";
    console << "\t--remaining-duration\tInclude the remaining duration in the output\n";
    console << "\nTranslations:\n";
    console << "The system locale is examined to load translation files.";
    console << " Translation files must be named routing-instructions_$lang.qm,";
    console << " where $lang is a two-letter ISO 639 language code, optionally suffixed by an underscore";
    console << " and an uppercase two-letter ISO 3166 country code, e.g. nl or de_DE. Such files are searched for";
    console << " in /usr/share/marble/translations, the translations subdir of the applications installation";
    console << " directory and the current working directory.\n";
    console << "\nExamples:\n";
    console << "export QUERY_STRING=\"flat=49.0&flon=8.3&tlat=49.0&tlon=8.35&fastest=1&v=motorcar\"\n";
    console << "gosmore gosmore.pak | routing-instructions\n";
    console << "LC_ALL=\"zh_TW\" gosmore gosmore.pak | routing-instructions --dense\n";
    console << "LC_ALL=\"nl.UTF-8\" routing-instructions gosmore.txt\n";
}

int main( int argc, char* argv[] )
{
    QCoreApplication app( argc, argv );
    QTranslator translator;
    loadTranslations( app, translator );

    QStringList const arguments = QCoreApplication::instance()->arguments();
    if ( arguments.contains( "--help" ) || arguments.contains( "-h" ) ) {
        usage();
        return 0;
    }

    RoutingInstructions directions;
    WaypointParser parser;
    if ( arguments.contains( "--routino" ) )
    {
        parser.setLineSeparator( "\n" );
        parser.setFieldSeparator( '\t' );
        parser.setFieldIndex( WaypointParser::RoadName, 10 );
    }
    else
    {
        parser.addJunctionTypeMapping( "Jr", RoutingWaypoint::Roundabout );
    }

    if ( argc > 1 && !( QString( argv[argc-1] ).startsWith( "--" ) ) )
    {
        QString filename( argv[argc-1] );
        QFile input( filename );
        input.open( QIODevice::ReadOnly );
        QTextStream fileStream( &input );
        QString content = adjustGosmoreVersion( fileStream, parser );
        QTextStream stream( &content );
        directions = InstructionTransformation::process( parser.parse( stream ) );
    }
    else
    {
        QTextStream console( stdin );
        console.setCodec( "UTF-8" );
        console.setAutoDetectUnicode( true );
        QString content = adjustGosmoreVersion( console, parser );
        QTextStream stream( &content );
        directions = InstructionTransformation::process( parser.parse( stream ) );
    }

    QTextStream console( stdout );
    console.setCodec( "UTF-8" );
    if ( arguments.contains( "--dense" ) )
    {
        console << "Content-Type: text/plain\n\n";
    }

    for ( int i = 0; i < directions.size(); ++i )
    {
        console << directions[i] << '\n';
    }

    return 0;
}
