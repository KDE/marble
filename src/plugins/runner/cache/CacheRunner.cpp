//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "CacheRunner.h"

#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"

#include <QFile>
#include <QDataStream>
#include <QSet>

namespace Marble
{

const quint32 MarbleMagicNumber = 0x31415926;

CacheRunner::CacheRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

CacheRunner::~CacheRunner()
{
}

GeoDataDocument* CacheRunner::parseFile( const QString &fileName, DocumentRole role, QString& error )
{
    QFile file( fileName );
    if ( !file.exists() ) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    file.open( QIODevice::ReadOnly );
    QDataStream in( &file );

    // Read and check the header
    quint32 magic;
    in >> magic;
    if ( magic != MarbleMagicNumber ) {
        return nullptr;
    }

    // Read the version
    qint32 version;
    in >> version;
    if ( version < 015 ) {
        error = QStringLiteral("Bad cache file %1: Version %2 is too old, need 15 or later").arg(fileName).arg(version);
        mDebug() << error;
        return nullptr;
    }
    /*
      if (version > 002) {
      qDebug( "Bad file - too new!" );
      return;
      }
    */
    GeoDataDocument *document = new GeoDataDocument();
    document->setDocumentRole( role );

    in.setVersion( QDataStream::Qt_4_2 );

    // Read the data itself
    // Use double to provide a single cache file format across architectures
    double   lon;
    double   lat;
    double   alt;
    double   area;

    QString  tmpstr;
    qint64   tmpint64;
    qint8    tmpint8;
    qint16   tmpint16;

    // share string data on the heap at least for this file
    QSet<QString> stringPool;
    const QString gmtId = QStringLiteral("gmt");
    const QString dstId = QStringLiteral("dst");

    while ( !in.atEnd() ) {
        GeoDataPlacemark *mark = new GeoDataPlacemark;
        in >> tmpstr; tmpstr = *stringPool.insert(tmpstr);
        mark->setName( tmpstr );
        in >> lon >> lat >> alt;
        mark->setCoordinate( (qreal)(lon), (qreal)(lat), (qreal)(alt) );
        in >> tmpstr; tmpstr = *stringPool.insert(tmpstr);
        mark->setRole( tmpstr );
        in >> tmpstr; tmpstr = *stringPool.insert(tmpstr);
        mark->setDescription( tmpstr );
        in >> tmpstr; tmpstr = *stringPool.insert(tmpstr);
        mark->setCountryCode( tmpstr );
        in >> tmpstr; tmpstr = *stringPool.insert(tmpstr);
        mark->setState( tmpstr );
        in >> area;
        mark->setArea( (qreal)(area) );
        in >> tmpint64;
        mark->setPopulation( tmpint64 );
        in >> tmpint16;
        mark->extendedData().addValue(GeoDataData(gmtId, int(tmpint16)));
        in >> tmpint8;
        mark->extendedData().addValue(GeoDataData(dstId, int(tmpint8)));

        document->append( mark );
    }
    document->setFileName( fileName );

    file.close();
    return document;
}

}

#include "moc_CacheRunner.cpp"
