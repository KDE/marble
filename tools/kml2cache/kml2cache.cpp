//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2013      Dennis Nienhüser <nienhueser@kde.org>
//

// A simple tool to read a .kml file and write it back to a .cache file

#include <ParsingRunnerManager.h>
#include <PluginManager.h>
#include <MarbleClock.h>
#include <GeoDataDocument.h>
#include <GeoDataFolder.h>
#include <GeoDataPlacemark.h>
#include <GeoDataExtendedData.h>
#include <GeoDataData.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <iostream>
#include <QDataStream>

using namespace std;
using namespace Marble;

const quint32 MarbleMagicNumber = 0x31415926;

void savePlacemarks( QDataStream &out, const GeoDataContainer *container, MarbleClock* clock )
{
    qreal lon;
    qreal lat;
    qreal alt;

    const QVector<GeoDataPlacemark*> placemarks = container->placemarkList();
    QVector<GeoDataPlacemark*>::const_iterator it = placemarks.constBegin();
    QVector<GeoDataPlacemark*>::const_iterator const end = placemarks.constEnd();
    for (; it != end; ++it ) {
        out << (*it)->name();
        (*it)->coordinate().geoCoordinates( lon, lat, alt );

        // Use double to provide a single cache file format across architectures
        out << (double)(lon) << (double)(lat) << (double)(alt);
        out << QString( (*it)->role() );
        out << QString( (*it)->description() );
        out << QString( (*it)->countryCode() );
        out << QString( (*it)->state() );
        out << (double) (*it)->area();
        out << (qint64) (*it)->population();
        out << ( qint16 ) ( (*it)->extendedData().value("gmt").value().toInt() );
        out << ( qint8 ) ( (*it)->extendedData().value("dst").value().toInt() );
    }

    const QVector<GeoDataFolder*> folders = container->folderList();
    QVector<GeoDataFolder*>::const_iterator cont = folders.constBegin();
    QVector<GeoDataFolder*>::const_iterator endcont = folders.constEnd();
    for (; cont != endcont; ++cont ) {
            savePlacemarks( out, *cont, clock );
    }
}

void saveFile( const QString& filename, GeoDataDocument* document )
{
    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        qDebug() << Q_FUNC_INFO << "Can't open" << filename << "for writing";
        return;
    }
    QDataStream out( &file );

    // Write a header with a "magic number" and a version
    // out << (quint32)0xA0B0C0D0;
    out << (quint32)MarbleMagicNumber;
    out << (qint32)015;

    out.setVersion( QDataStream::Qt_4_2 );

    savePlacemarks( out, document, new MarbleClock );
}

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    QString inputFilename;
    int inputIndex = app.arguments().indexOf( "-i" );
    if ( inputIndex > 0 && inputIndex + 1 < argc ) {
        inputFilename = app.arguments().at( inputIndex + 1 );
    } else {
        qDebug( " Syntax: kml2cache -i sourcefile [-o cache-targetfile]" );
        return 1;
    }

    QString outputFilename = "output.cache";
    int outputIndex = app.arguments().indexOf("-o");
    if ( outputIndex > 0 && outputIndex + 1 < argc )
        outputFilename = app.arguments().at( outputIndex + 1 );

    ParsingRunnerManager* manager = new ParsingRunnerManager( new PluginManager );
    GeoDataDocument* document = manager->openFile( inputFilename );
    if (!document) {
        qDebug() << "Could not parse input file. No error message available unfortunately";
        return 2;
    }

    saveFile( outputFilename, document );
}
