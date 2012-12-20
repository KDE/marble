//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "LogRunner.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

namespace Marble
{

LogRunner::LogRunner( QObject *parent ) :
    ParsingRunner( parent )
{
}

LogRunner::~LogRunner()
{
}

void LogRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFile file( fileName );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        emit parsingFinished( 0 );
        return;
    }

    file.open( QIODevice::ReadOnly );
    QTextStream stream( &file );

    GeoDataLineString *const track = new GeoDataLineString;

    GeoDataPlacemark *const placemark = new GeoDataPlacemark;
    placemark->setGeometry( track );

    GeoDataDocument *document = new GeoDataDocument();
    document->setDocumentRole( role );
    document->append( placemark );

    int count = 0;
    bool error = false;
    while( !stream.atEnd() || error ){
        const QString line = stream.readLine();
        const QStringList list = line.split( ',' );

        if ( list.size() != 7 ) {
            mDebug() << Q_FUNC_INFO << "Aborting due to error in line" << count << ". Line was:" << line;
            error = true;
            break;
        }

        const QString strLat = list[0];
        const QString strLon = list[1];
        const QString strElevation = list[2];
        const QString strSpeed = list[3];
        const QString strCourse = list[4];
        const QString strHdop = list[5];
        const QString strTime = list[6];

        if ( strLat.isEmpty() || strLon.isEmpty() || strElevation.isEmpty() ) {
            continue;
        }

        bool okLat, okLon, okAlt = false;
        const qreal lat = strLat.toDouble( &okLat );
        const qreal lon = strLon.toDouble( &okLon );
        const qreal alt = strElevation.toDouble( &okAlt );

        if ( !okLat || !okLon || !okAlt ) {
            continue;
        }

        GeoDataCoordinates coord( lon, lat, alt, GeoDataCoordinates::Degree );
        track->append( coord );
    }

    file.close();
    if ( track->size() == 0 || error ) {
        delete document;
        document = 0;
    }

    emit parsingFinished( document );
}

}

#include "LogRunner.moc"
