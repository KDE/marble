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
#include "MarbleMath.h"
#include "MarbleLocale.h"
#include "DatabaseQuery.h"

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

OsmDatabase::OsmDatabase()
{
    m_database = QSqlDatabase::addDatabase( "QSQLITE" );
}

void OsmDatabase::addFile( const QString &fileName )
{
    m_databases << fileName;
}

QList<OsmPlacemark> OsmDatabase::find( MarbleModel* model, const QString &searchTerm )
{
    if ( m_databases.isEmpty() ) {
        return QList<OsmPlacemark>();
    }

    DatabaseQuery userQuery( model, searchTerm );

    QString queryString;

    queryString = " SELECT regions.name,"
            " places.name, places.number,"
            " places.category, places.lon, places.lat"
            " FROM regions, places";

    if ( userQuery.queryType() == DatabaseQuery::CategorySearch ) {
        queryString += " WHERE regions.id = places.region AND places.category = %1";
        queryString = queryString.arg( (qint32) userQuery.category() );
        if ( userQuery.resultFormat() == DatabaseQuery::DistanceFormat && userQuery.region().isEmpty() ) {
            queryString += " ORDER BY ((places.lat-%1)*(places.lat-%1)+(places.lon-%2)*(places.lon-%2))";
            GeoDataCoordinates position = userQuery.position();
            queryString = queryString.arg( position.latitude( GeoDataCoordinates::Degree ), 0, 'f', 8 )
                    .arg( position.longitude( GeoDataCoordinates::Degree ), 0, 'f', 8 );
        } else {
            queryString += " AND regions.name LIKE '%%2%'";
            queryString = queryString.arg( userQuery.region() );
        }
    } else if ( userQuery.queryType() == DatabaseQuery::BroadSearch ) {
        queryString += " WHERE regions.id = places.region"
                " AND places.name LIKE '%%1%'";
        queryString = queryString.arg( searchTerm );
    } else {
        queryString += " WHERE regions.id = places.region"
                "   AND places.name LIKE '%%1%'";
        queryString = queryString.arg( userQuery.street() );
        if ( !userQuery.houseNumber().isEmpty() ) {
            queryString += " AND places.number IS '%1'";
            queryString = queryString.arg( userQuery.houseNumber() );
        }
        queryString += " AND regions.name LIKE '%%2%'";
        queryString = queryString.arg( userQuery.region() );
    }

    queryString += " LIMIT 50;";

    QList<OsmPlacemark> result;
    QTime timer;
    timer.start();
    foreach( const QString &databaseFile, m_databases ) {
        /** @todo: sort/filter results from several databases */

        m_database.setDatabaseName( databaseFile );
        if ( !m_database.open() ) {
            qDebug() << "Failed to connect to database " << databaseFile;
        }

        QSqlQuery query;
        query.setForwardOnly( true );
        if ( !query.exec( queryString ) ) {
            qDebug() << "Failed to execute query: " << query.lastError();
            return result;
        }

        while ( query.next() ) {
            OsmPlacemark placemark;
            if ( userQuery.resultFormat() == DatabaseQuery::DistanceFormat ) {
                GeoDataCoordinates coordinates( query.value(4).toFloat(), query.value(5).toFloat(), 0.0, GeoDataCoordinates::Degree );
                placemark.setAdditionalInformation( formatDistance( coordinates, userQuery.position() ) );
            } else {
                placemark.setAdditionalInformation( query.value( 0 ).toString() );
            }
            placemark.setName( query.value(1).toString() );
            placemark.setHouseNumber( query.value(2).toString() );
            placemark.setCategory( (OsmPlacemark::OsmCategory) query.value(3).toInt() );
            placemark.setLongitude( query.value(4).toFloat() );
            placemark.setLatitude( query.value(5).toFloat() );
            result.push_back( placemark );
        }
    }

    mDebug() << "Offline OSM search query took " << timer.elapsed() << " ms. Query: " << queryString;
    return result;
}

QString OsmDatabase::formatDistance( const GeoDataCoordinates &a, const GeoDataCoordinates &b ) const
{
    qreal distance = EARTH_RADIUS * distanceSphere( a, b);

    int precision = 0;
    QString distanceUnit = "m";

    if ( MarbleGlobal::getInstance()->locale()->distanceUnit() == Marble::MilesFeet ) {
        precision = 1;
        distanceUnit = "mi";
        distance *= METER2KM;
        distance *= KM2MI;
    } else {
        if ( distance >= 1000 ) {
            distance /= 1000;
            distanceUnit = "km";
            precision = 1;
        } else if ( distance >= 200 ) {
            distance = 50 * qRound( distance / 50 );
        } else if ( distance >= 100 ) {
            distance = 25 * qRound( distance / 25 );
        } else {
            distance = 10 * qRound( distance / 10 );
        }
    }

    QString const fuzzyDistance = QString( "%1 %2" ).arg( distance, 0, 'f', precision ).arg( distanceUnit );

    int direction = 180 + bearing( a, b ) * RAD2DEG;

    QString heading = QObject::tr( "north" );
    if ( direction > 337 ) {
        heading = QObject::tr( "north" );
    } else if ( direction > 292 ) {
        heading = QObject::tr( "north-west" );
    } else if ( direction > 247 ) {
        heading = QObject::tr( "west" );
    } else if ( direction > 202 ) {
        heading = QObject::tr( "south-west" );
    } else if ( direction > 157 ) {
        heading = QObject::tr( "south" );
    } else if ( direction > 112 ) {
        heading = QObject::tr( "south-east" );
    } else if ( direction > 67 ) {
        heading = QObject::tr( "east" );
    } else if ( direction > 22 ) {
        heading = QObject::tr( "north-east" );
    }

    return fuzzyDistance + " " + heading;
}

qreal OsmDatabase::bearing( const GeoDataCoordinates &a, const GeoDataCoordinates &b ) const
{
    qreal delta = b.longitude() - a.longitude();
    qreal lat1 = a.latitude();
    qreal lat2 = b.latitude();
    return fmod( atan2( sin ( delta ) * cos ( lat2 ),
                       cos( lat1 ) * sin( lat2 ) - sin( lat1 ) * cos( lat2 ) * cos ( delta ) ), 2 * M_PI );
}

}
