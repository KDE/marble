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
#include "MarbleDebug.h"

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

GeoDataDocument *GpsbabelRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    // Check and see if the file exists
    if ( !QFileInfo( fileName ).exists() ) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
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
        error = QStringLiteral("Unsupported file extension for").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Set up temporary file to hold output KML from gpsbabel executable
    QTemporaryFile tempKmlFile(QDir::tempPath() + QLatin1String("/marble-gpsbabel-XXXXXX.kml"));
    tempKmlFile.open();
    QFile kmlFile( tempKmlFile.fileName() );

    // Set up gpsbabel command line
    const QString command =
        QLatin1String("gpsbabel -i ") + inputFileType +
        QLatin1String(" -f ") + fileName + QLatin1String(" -o kml -F ") +
        tempKmlFile.fileName();

    // Execute gpsbabel to parse the input file
    int const exitStatus = QProcess::execute( command );
    if ( exitStatus == 0 ) {
        kmlFile.open( QIODevice::ReadWrite );
        GeoDataParser parser( GeoData_KML );
        parser.read( &kmlFile );
        GeoDataDocument *document = dynamic_cast<GeoDataDocument*>( parser.releaseDocument() );
        if ( !document ) {
            error = parser.errorString();
            mDebug() << error;
            return nullptr;
        }

        document->setDocumentRole( role );
        return document;
    } else {
        error = QStringLiteral("Gpsbabel returned error code %1").arg(exitStatus);
        mDebug() << error;
        return nullptr;
    }
}

}

#include "moc_GpsbabelRunner.cpp"
