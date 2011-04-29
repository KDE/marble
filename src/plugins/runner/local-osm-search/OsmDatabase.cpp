//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmDatabase.h"
#include "OsmRegion.h"
#include "MarbleDebug.h"

#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QVariant>
#include <QtCore/QTime>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace Marble {

namespace {
    quint32 const osmDatabaseFileMagicNumber = 0xAEB4E6D8;
    quint32 const osmDatabaseFileVersion = 160;
}

OsmDatabase::OsmDatabase()
{
    // nothing to do
}

void OsmDatabase::addFile( const QString &fileName )
{
    QSqlDatabase database = QSqlDatabase::addDatabase( "QSQLITE" );
    database.setDatabaseName( fileName );
    if ( !database.open() ) {
        qDebug() << "Failed to connect to database " << fileName;
    }
}

void OsmDatabase::save( const QString &filename )
{
    QFile file( filename );
    file.open( QFile::WriteOnly );

    QDataStream stream( &file );

    // Write a header with a "magic number" and a version
    stream << (quint32) osmDatabaseFileMagicNumber;
    stream << (quint32) osmDatabaseFileVersion;
    stream.setVersion( QDataStream::Qt_4_0 );

    /** @todo: Use a B-tree or similar */
    stream << m_regions;
    qSort( m_placemarks );
    stream << m_placemarks;

    file.close();
}

void OsmDatabase::addOsmRegion( const OsmRegion &region )
{
    m_regions.push_back( region );
}

void OsmDatabase::addOsmPlacemark( const OsmPlacemark &placemark )
{
    m_placemarks.push_back( placemark );
}

QList<OsmPlacemark> OsmDatabase::find( const QString &searchTerm ) const
{
    QString const queryString = " SELECT regions.name,"
                                " placemarks.name, placemarks.number,"
                                " placemarks.category, placemarks.lon, placemarks.lat"
                                " FROM regions, placemarks"
                                " WHERE regions.id = placemarks.region"
                                "   AND placemarks.name LIKE '%%1%'"
                                " LIMIT 20";

    QList<OsmPlacemark> result;
    QTime timer;
    timer.start();
    QSqlQuery query;
    query.setForwardOnly( true );
    if ( !query.exec( queryString.arg( searchTerm ) ) ) {
        qDebug() << "Failed to execute query: " << query.lastError();
        return result;
    }
    qDebug() << "Query took " << timer.elapsed() << " ms.";

    while ( query.next() ) {
        OsmPlacemark placemark;
        placemark.setRegionName( query.value( 0 ).toString() );
        placemark.setName( query.value(1).toString() );
        placemark.setHouseNumber( query.value(2).toString() );
        placemark.setCategory( (OsmPlacemark::OsmCategory) query.value(3).toInt() );
        placemark.setLongitude( query.value(4).toFloat() );
        placemark.setLatitude( query.value(5).toFloat() );
        result.push_back( placemark );
        qDebug() << "found " << placemark.name();
    }

    return result;

    QStringList terms = searchTerm.split( ",", QString::SkipEmptyParts );

    QRegExp streetAndHouse( "^(.*)\\s+(\\d+\\D?)$" );
    if ( streetAndHouse.indexIn( terms.first() ) != -1 ) {
        if ( streetAndHouse.capturedTexts().size() == 3 ) {
            terms.removeFirst();
            terms.push_front( streetAndHouse.capturedTexts().at( 1 ) );
            terms.push_front( streetAndHouse.capturedTexts().at( 2 ) );
        }
    }

    if ( terms.size() == 1 ) {
        return findOsmTerm( terms.first().trimmed() );
    } else if ( terms.size() == 2 ) {
        return findStreets( terms.last().trimmed(), terms.first().trimmed() );
    } else {
        Q_ASSERT( terms.size() > 2 ); // according to split() docs
        return findHouseNumber( terms.at( 2 ).trimmed(),
                                       terms.at( 1 ).trimmed(),
                                       terms.at( 0 ).trimmed() );
    }

    /** @todo: alternative words, etc. */

    return QList<OsmPlacemark>();
}

QList<OsmPlacemark> OsmDatabase::findOsmTerm( const QString &term ) const
{
    QList<OsmPlacemark> result;
    foreach( const OsmRegion &region, m_regions ) {
        if ( region.name().startsWith( term, Qt::CaseInsensitive ) ) {
            OsmPlacemark placemark;
            placemark.setLongitude( region.longitude() );
            placemark.setLatitude( region.latitude() );
            placemark.setName( region.name() );
            result << placemark;
        }
    }

    foreach( const OsmPlacemark &placemark, m_placemarks ) {
        if ( placemark.name().startsWith( term, Qt::CaseInsensitive ) ) {
            result << placemark;
        }
    }

    return result;
}

QList<OsmPlacemark> OsmDatabase::findStreets( const QString &reg, const QString &street ) const
{
    QList<OsmPlacemark> result;
    QList<int> regions;
    foreach( const OsmRegion &region, m_regions ) {
        if ( region.name().startsWith( reg, Qt::CaseInsensitive ) ) {
            regions << region.identifier();
        }
    }

    foreach( const OsmPlacemark &placemark, m_placemarks ) {
        if ( regions.contains( placemark.regionId() ) && placemark.name().startsWith( street, Qt::CaseInsensitive ) ) {
            result.push_back( placemark );
        }
    }

    return result;
}

QList<OsmPlacemark> OsmDatabase::findHouseNumber( const QString &reg, const QString &street, const QString &houseNumber ) const
{
    QList<OsmPlacemark> result;
    QList<int> regions;
    foreach( const OsmRegion &region, m_regions ) {
        if ( region.name().startsWith( reg, Qt::CaseInsensitive ) ) {
            regions << region.identifier();
        }
    }

    foreach( const OsmPlacemark &placemark, m_placemarks ) {
        if ( regions.contains( placemark.regionId() ) &&
             placemark.name().startsWith( street, Qt::CaseInsensitive ) &&
             placemark.houseNumber() == houseNumber ) {
            result.push_back( placemark );
        }
    }

    return result;
}

}
