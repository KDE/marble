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
#include <QMap>

namespace Marble
{

GpsbabelRunner::GpsbabelRunner( QObject *parent ) :
    ParsingRunner( parent )
{
}

void GpsbabelRunner::parseFile( const QString &fileName, DocumentRole role )
{
    // Check and see if the file exists
    if ( !QFileInfo( fileName ).exists() ) {
        qWarning( "File does not exist!" );
        emit parsingFinished( 0 );
        return;
    }

    // Inspect the filename suffix
    QString const fileSuffix = QFileInfo( fileName ).suffix();

    // Determine if fileName suffix is supported by this plugin
    QMap<QString,QString> fileTypes;
    fileTypes["nmea"]     = "nmea";
    fileTypes["igc"]      = "igc";
    fileTypes["tiger"]    = "tiger";
    fileTypes["ov2"]      = "tomtom";
    fileTypes["garmin"]   = "garmin_txt";
    fileTypes["magellan"] = "magellan";
    fileTypes["csv"]      = "csv";
    QString const inputFileType = fileTypes[fileSuffix];
    if ( inputFileType.isEmpty() ) {
        qWarning( "File type is not supported !" );
        emit parsingFinished( 0 );
        return;
    }

    // Set up temporary file to hold output KML from gpsbabel executable
    QTemporaryFile tempKmlFile( QDir::tempPath() + "/marble-gpsbabel-XXXXXX.kml" );
    tempKmlFile.open();
    QFile kmlFile( tempKmlFile.fileName() );

    // Set up gpsbabel command line
    QString command = "gpsbabel -i " + inputFileType;
    command += " -f " + fileName + " -o kml -F ";
    command += tempKmlFile.fileName();

    // Execute gpsbabel to parse the input file
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
        emit parsingFinished( 0, "GPSBabel failed to parse the input file." );
    }
}

}

#include "GpsbabelRunner.moc"
