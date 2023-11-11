// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "LogRunner.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"

#include <QFile>

namespace Marble
{

LogRunner::LogRunner( QObject *parent ) :
    ParsingRunner( parent )
{
}

LogRunner::~LogRunner()
{
}

GeoDataDocument *LogRunner::parseFile(const QString &fileName, DocumentRole role, QString &errorString)
{
    QFile file( fileName );
    if ( !file.exists() ) {
        errorString = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << errorString;
        return nullptr;
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
        const QStringList list = line.split(QLatin1Char(','));

        if ( list.size() != 7 ) {
            mDebug() << "Aborting due to error in line" << count << ". Line was:" << line;
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
        document = nullptr;
        return nullptr;
    }

    document->setFileName( fileName );
    return document;
}

}

#include "moc_LogRunner.cpp"
