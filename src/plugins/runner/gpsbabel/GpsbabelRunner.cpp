//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>

#include "GpsbabelRunner.h"

#include "GeoDataParser.h"
#include "GeoDataDocument.h"

#include <QFile>
#include <QProcess>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDir>

namespace Marble
{

GpsbabelRunner::GpsbabelRunner( QObject *parent ) :
    ParsingRunner( parent )
{
}

GpsbabelRunner::~GpsbabelRunner()
{
}

void GpsbabelRunner::parseFile( const QString &fileName, DocumentRole role )
{
    if ( !QFileInfo( fileName ).exists() ) {
        qWarning( "File does not exist!" );
        emit parsingFinished( 0 );
        return;
    }

    QTemporaryFile tempKmlFile( QDir::tempPath() + "/marble-gpsbabel-XXXXXX.kml" );
    tempKmlFile.open();
    QFile kmlFile( tempKmlFile.fileName() );
    QString command = "gpsbabel -i nmea -f ";
    command += fileName + " -o kml -F ";
    command += tempKmlFile.fileName();

    if ( QProcess::execute( command ) == 0 ) {
        kmlFile.open( QIODevice::ReadWrite );
        GeoDataParser parser( GeoData_KML );
        parser.read( &kmlFile );
        GeoDataDocument *document = dynamic_cast<GeoDataDocument*>( parser.releaseDocument() );
        if ( !document ) {
            emit parsingFinished( 0, "Unable to open " + fileName );
            return;
        }

        document->setDocumentRole( role );
        emit parsingFinished( document );
    } else {
        emit parsingFinished( 0, "Failed to parse NMEA file." );
    }
}

}

#include "GpsbabelRunner.moc"
