//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser     <earthwings@gentoo.org>
//

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

void usage( const QString &app )
{
    qDebug() << "Usage: " << app << " input.ts input.po";
}

int main( int argc, char** argv )
{
    if ( argc != 3 ) {
        usage( argv[0] );
        return 0;
    }

    QFileInfo const ts = QFileInfo( QString( argv[1] ) );
    if ( !ts.exists() ) {
        usage( argv[0] );
        return 1;
    }

    QFileInfo const po = QFileInfo( QString( argv[2] ) );
    if ( !po.exists() ) {
        usage( argv[0] );
        return 2;
    }

    QMap<QString,QString> translations;

    // Open the .po file and build a map of translations
    QFile poFile( po.absoluteFilePath() );
    poFile.open( QFile::ReadOnly );
    QTextStream poStream( &poFile );
    poStream.setCodec( "UTF-8" );
    poStream.setAutoDetectUnicode( true );
    QString source;
    bool ignore = false;
    while( !poStream.atEnd() ) {
        QString line = poStream.readLine();
        if ( line.startsWith( "#, fuzzy" ) ) {
            ignore = true;
        } else if ( line.startsWith( "msgid " ) ) {
            source = line.mid( 7, line.size() - 8 );
        } else if ( !source.isEmpty() && line.startsWith( "msgstr " ) ) {
            if ( ignore ) {
                ignore = false;
            } else {
                QString translation = line.mid( 8, line.size() - 9 );
                source.replace( "&", "&amp;" );
                translation.replace( "&", "&amp;" );
                source.replace( "<", "&lt;" );
                translation.replace( "<", "&lt;" );
                source.replace( ">", "&gt;" );
                translation.replace( ">", "&gt;" );
                if ( !translation.isEmpty() ) {
                    translations[source] = translation;
                }
            }
        }
    }

    QTextStream console( stdout );
    console.setCodec( "UTF-8" );

    // Open the .ts file and replace source strings with translations
    // The modified .to file is dumped to stdout
    QFile tsFile( ts.absoluteFilePath() );
    tsFile.open( QFile::ReadOnly );
    QTextStream tsStream( &tsFile );
    tsStream.setCodec( "UTF-8" );
    tsStream.setAutoDetectUnicode( true );
    source = QString();
    while( !tsStream.atEnd() ) {
        QString line = tsStream.readLine().trimmed();
        if ( line.startsWith( "<source>" ) ) {
            source = line.mid( 8, line.size() - 17 );
            console << line << "\n";
        } else if ( !source.isEmpty() &&
                   line == "<translation type=\"unfinished\"></translation>" &&
                   translations.contains( source ) ) {
            console << "<translation>" << translations[source] << "</translation>\n";
        } else if ( !source.isEmpty() &&
                   line == "<translation type=\"unfinished\"></translation>" ) {
            console << line << "\n";
        } else {
            console << line << "\n";
        }
    }

    return 0;
}
