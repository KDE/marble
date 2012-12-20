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
#include "GeoDataPlacemark.h"

#include <QtCore/QFile>

namespace Marble
{

const quint32 MarbleMagicNumber = 0x31415926;

CacheRunner::CacheRunner(QObject *parent) :
    MarbleAbstractRunner(parent)
{
}

CacheRunner::~CacheRunner()
{
}

GeoDataFeature::GeoDataVisualCategory CacheRunner::category() const
{
    return GeoDataFeature::Folder;
}

void CacheRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFile file( fileName );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        emit parsingFinished( 0 );
        return;
    }

    file.open( QIODevice::ReadOnly );
    QDataStream in( &file );

    // Read and check the header
    quint32 magic;
    in >> magic;
    if ( magic != MarbleMagicNumber ) {
        emit parsingFinished( 0 );
        return;
    }

    // Read the version
    qint32 version;
    in >> version;
    if ( version < 015 ) {
        qDebug( "Bad Cache file - too old!" );
        emit parsingFinished( 0 );
        return;
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

    while ( !in.atEnd() ) {
        GeoDataPlacemark *mark = new GeoDataPlacemark;
        in >> tmpstr;
        mark->setName( tmpstr );
        in >> lon >> lat >> alt;
        mark->setCoordinate( (qreal)(lon), (qreal)(lat), (qreal)(alt) );
        in >> tmpstr;
        mark->setRole( tmpstr );
        in >> tmpstr;
        mark->setDescription( tmpstr );
        in >> tmpstr;
        mark->setCountryCode( tmpstr );
        in >> tmpstr;
        mark->setState( tmpstr );
        in >> area;
        mark->setArea( (qreal)(area) );
        in >> tmpint64;
        mark->setPopulation( tmpint64 );
        in >> tmpint16;
        mark->extendedData().addValue( GeoDataData( "gmt", int( tmpint16 ) ) );
        in >> tmpint8;
        mark->extendedData().addValue( GeoDataData( "dst", int( tmpint8 ) ) );

        document->append( mark );
    }

    file.close();
    emit parsingFinished( document );
}

}

#include "CacheRunner.moc"
