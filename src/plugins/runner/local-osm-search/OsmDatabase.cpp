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

#include "DatabaseQuery.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "PositionTracking.h"

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

static GeoDataCoordinates s_currentPosition;
static DatabaseQuery* s_currentQuery = 0;

bool placemarkSmallerDistance( const OsmPlacemark &a, const OsmPlacemark &b )
{
    return distanceSphere( a.longitude() * DEG2RAD, a.latitude() * DEG2RAD,
                           s_currentPosition.longitude(), s_currentPosition.latitude() )
         < distanceSphere( b.longitude() * DEG2RAD, b.latitude() * DEG2RAD,
                           s_currentPosition.longitude(), s_currentPosition.latitude() );
}

bool placemarkHigherScore( const OsmPlacemark &a, const OsmPlacemark &b )
{
    return a.matchScore( s_currentQuery ) > b.matchScore( s_currentQuery );
}

}

OsmDatabase::OsmDatabase()
{
    m_database = QSqlDatabase::addDatabase( "QSQLITE", "marble/local-osm-search" );
}

void OsmDatabase::addFile( const QString &fileName )
{
    m_databases << fileName;
}

QVector<OsmPlacemark> OsmDatabase::find( MarbleModel* model, const QString &searchTerm, const GeoDataLatLonAltBox preferred )
{
    if ( m_databases.isEmpty() ) {
        return QVector<OsmPlacemark>();
    }

    DatabaseQuery userQuery( model, searchTerm, preferred );

    QVector<OsmPlacemark> result;
    QTime timer;
    timer.start();
    foreach( const QString &databaseFile, m_databases ) {
        m_database.setDatabaseName( databaseFile );
        if ( !m_database.open() ) {
            mDebug() << "Failed to connect to database " << databaseFile;
        }

        QString regionRestriction;
        if ( !userQuery.region().isEmpty() ) {
            // Nested set model to support region hierarchies, see http://en.wikipedia.org/wiki/Nested_set_model
            QSqlQuery regionsQuery( "SELECT lft, rgt FROM regions WHERE name LIKE '%" + userQuery.region() + "%';", m_database );
            if ( regionsQuery.lastError().isValid() ) {
                mDebug() << "Error when executing query" << regionsQuery.executedQuery();
                mDebug() << "Sql reports" << regionsQuery.lastError();
            }
            regionRestriction = " AND (";
            bool first = true;
            while ( regionsQuery.next() ) {
                if ( first ) {
                    first = false;
                } else {
                    regionRestriction += " OR ";
                }
                regionRestriction += " (regions.lft >= " + regionsQuery.value( 0 ).toString();
                regionRestriction += " AND regions.lft <= " + regionsQuery.value( 1 ).toString() + ")";
            }
            regionRestriction += ")";

            if ( first ) {
                continue;
            }
        }

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
                queryString += regionRestriction;
            }
        } else if ( userQuery.queryType() == DatabaseQuery::BroadSearch ) {
            queryString += " WHERE regions.id = places.region"
                    " AND places.name " + wildcardQuery( searchTerm );
        } else {
            queryString += " WHERE regions.id = places.region"
                    "   AND places.name " + wildcardQuery( userQuery.street() );
            if ( !userQuery.houseNumber().isEmpty() ) {
                queryString += " AND places.number " + wildcardQuery( userQuery.houseNumber() );
            } else {
                queryString += " AND places.number IS NULL";
            }
            queryString += regionRestriction;
        }

        queryString += " LIMIT 50;";

        /** @todo: sort/filter results from several databases */

        QSqlQuery query( m_database );
        query.setForwardOnly( true );
        if ( !query.exec( queryString ) ) {
            mDebug() << "Failed to execute query" << query.lastError();
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

        // mDebug() << "Query string: " << queryString;
    }

    mDebug() << "Offline OSM search query took " << timer.elapsed() << " ms.";

    qSort( result.begin(), result.end() );
    unique( result );

    if ( userQuery.resultFormat() == DatabaseQuery::DistanceFormat ) {
        s_currentPosition = model->positionTracking()->currentLocation();
        qSort( result.begin(), result.end(), placemarkSmallerDistance );
    } else {
        s_currentQuery = &userQuery;
        qSort( result.begin(), result.end(), placemarkHigherScore );
        s_currentQuery = 0;
    }

    if ( result.size() > 50 ) {
        result.remove( 50, result.size()-50 );
    }

    return result;
}

void OsmDatabase::unique( QVector<OsmPlacemark> &placemarks ) const
{
    for ( int i=1; i<placemarks.size(); ++i ) {
        if ( placemarks[i-1] == placemarks[i] ) {
            placemarks.remove( i );
            --i;
        }
    }
}

QString OsmDatabase::formatDistance( const GeoDataCoordinates &a, const GeoDataCoordinates &b ) const
{
    qreal distance = EARTH_RADIUS * distanceSphere( a, b);

    int precision = 0;
    QString distanceUnit = "m";

    if ( MarbleGlobal::getInstance()->locale()->measurementSystem() == QLocale::ImperialSystem ) {
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

QString OsmDatabase::wildcardQuery( const QString &term ) const
{
    QString result = term;
    if ( term.contains( '*' ) ) {
        return " LIKE '" + result.replace( '*', '%' ) + "'";
    } else {
        return " = '" + result + "'";
    }
}

void OsmDatabase::clear()
{
    m_databases.clear();
}

}
