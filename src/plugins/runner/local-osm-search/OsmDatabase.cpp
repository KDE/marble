//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "OsmDatabase.h"

#include "DatabaseQuery.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "PositionTracking.h"

#include <QDataStream>
#include <QTime>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Marble {

namespace {

class PlacemarkSmallerDistance
{
public:
    PlacemarkSmallerDistance( const GeoDataCoordinates &currentPosition ) :
        m_currentPosition( currentPosition )
    {}

    bool operator()( const OsmPlacemark &a, const OsmPlacemark &b ) const
    {
        return distanceSphere( a.longitude() * DEG2RAD, a.latitude() * DEG2RAD,
                               m_currentPosition.longitude(), m_currentPosition.latitude() )
             < distanceSphere( b.longitude() * DEG2RAD, b.latitude() * DEG2RAD,
                               m_currentPosition.longitude(), m_currentPosition.latitude() );
    }

private:
    GeoDataCoordinates m_currentPosition;
};

class PlacemarkHigherScore
{
public:
    PlacemarkHigherScore( const DatabaseQuery *currentQuery ) :
        m_currentQuery( currentQuery )
    {}

    bool operator()( const OsmPlacemark &a, const OsmPlacemark &b ) const
    {
        return a.matchScore( m_currentQuery ) > b.matchScore( m_currentQuery );
    }

private:
    const DatabaseQuery *const m_currentQuery;
};

}

OsmDatabase::OsmDatabase( const QStringList &databaseFiles ) :
    m_databaseFiles( databaseFiles )
{
}

QVector<OsmPlacemark> OsmDatabase::find( const DatabaseQuery &userQuery )
{
    if ( m_databaseFiles.isEmpty() ) {
        return QVector<OsmPlacemark>();
    }

    QSqlDatabase database = QSqlDatabase::addDatabase( "QSQLITE", QString( "marble/local-osm-search-%1" ).arg( reinterpret_cast<size_t>( this ) ) );

    QVector<OsmPlacemark> result;
    QTime timer;
    timer.start();
    foreach( const QString &databaseFile, m_databaseFiles ) {
        database.setDatabaseName( databaseFile );
        if ( !database.open() ) {
            qWarning() << "Failed to connect to database" << databaseFile;
        }

        QString regionRestriction;
        if ( !userQuery.region().isEmpty() ) {
            QTime regionTimer;
            regionTimer.start();
            // Nested set model to support region hierarchies, see http://en.wikipedia.org/wiki/Nested_set_model
            const QString regionsQueryString = QLatin1String("SELECT lft, rgt FROM regions WHERE name LIKE '%") + userQuery.region() + QLatin1String("%';");
            QSqlQuery regionsQuery( regionsQueryString, database );
            if ( regionsQuery.lastError().isValid() ) {
                qWarning() << regionsQuery.lastError() << "in" << databaseFile << "with query" << regionsQuery.lastQuery();
            }
            regionRestriction = " AND (";
            int regionCount = 0;
            while ( regionsQuery.next() ) {
                if ( regionCount > 0 ) {
                    regionRestriction += QLatin1String(" OR ");
                }
                regionRestriction += QLatin1String(" (regions.lft >= ") + regionsQuery.value( 0 ).toString() +
                                     QLatin1String(" AND regions.lft <= ") + regionsQuery.value( 1 ).toString() + QLatin1Char(')');
                regionCount++;
            }
            regionRestriction += QLatin1Char(')');

            mDebug() << Q_FUNC_INFO << "region query in" << databaseFile << "with query" << regionsQueryString
                     << "took" << regionTimer.elapsed() << "ms for" << regionCount << "results";

            if ( regionCount == 0 ) {
                continue;
            }
        }

        QString queryString;

        queryString = " SELECT regions.name,"
                " places.name, places.number,"
                " places.category, places.lon, places.lat"
                " FROM regions, places";

        if ( userQuery.queryType() == DatabaseQuery::CategorySearch ) {
            queryString += QLatin1String(" WHERE regions.id = places.region");
            if( userQuery.category() == OsmPlacemark::UnknownCategory ) {
                // search for all pois which are not street nor address
                queryString += QLatin1String(" AND places.category <> 0 AND places.category <> 6");
            } else {
                // search for specific category
                queryString += QLatin1String(" AND places.category = %1");
                queryString = queryString.arg( (qint32) userQuery.category() );
            }
            if ( userQuery.position().isValid() && userQuery.region().isEmpty() ) {
                // sort by distance
                queryString += QLatin1String(" ORDER BY ((places.lat-%1)*(places.lat-%1)+(places.lon-%2)*(places.lon-%2))");
                GeoDataCoordinates position = userQuery.position();
                queryString = queryString.arg( position.latitude( GeoDataCoordinates::Degree ), 0, 'f', 8 )
                        .arg( position.longitude( GeoDataCoordinates::Degree ), 0, 'f', 8 );
            } else {
                queryString += regionRestriction;
            }
        } else if ( userQuery.queryType() == DatabaseQuery::BroadSearch ) {
            queryString += QLatin1String(" WHERE regions.id = places.region"
                    " AND places.name ") + wildcardQuery(userQuery.searchTerm());
        } else {
            queryString += QLatin1String(" WHERE regions.id = places.region"
                    "   AND places.name ") + wildcardQuery(userQuery.street());
            if ( !userQuery.houseNumber().isEmpty() ) {
                queryString += QLatin1String(" AND places.number ") + wildcardQuery(userQuery.houseNumber());
            } else {
                queryString += QLatin1String(" AND places.number IS NULL");
            }
            queryString += regionRestriction;
        }

        queryString += QLatin1String(" LIMIT 50;");

        /** @todo: sort/filter results from several databases */

        QSqlQuery query( database );
        query.setForwardOnly( true );
        QTime queryTimer;
        queryTimer.start();
        if ( !query.exec( queryString ) ) {
            qWarning() << query.lastError() << "in" << databaseFile << "with query" << query.lastQuery();
            continue;
        }

        int resultCount = 0;
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
            resultCount++;
        }

        mDebug() << Q_FUNC_INFO << "query in" << databaseFile << "with query" << queryString
                 << "took" << queryTimer.elapsed() << "ms for" << resultCount << "results";
    }

    mDebug() << "Offline OSM search query took" << timer.elapsed() << "ms for" << result.count() << "results.";

    qSort( result.begin(), result.end() );
    makeUnique( result );

    if ( userQuery.position().isValid() ) {
        const PlacemarkSmallerDistance placemarkSmallerDistance( userQuery.position() );
        qSort( result.begin(), result.end(), placemarkSmallerDistance );
    } else {
        const PlacemarkHigherScore placemarkHigherScore( &userQuery );
        qSort( result.begin(), result.end(), placemarkHigherScore );
    }

    if ( result.size() > 50 ) {
        result.remove( 50, result.size()-50 );
    }

    return result;
}

void OsmDatabase::makeUnique( QVector<OsmPlacemark> &placemarks )
{
    for ( int i=1; i<placemarks.size(); ++i ) {
        if ( placemarks[i-1] == placemarks[i] ) {
            placemarks.remove( i );
            --i;
        }
    }
}

QString OsmDatabase::formatDistance( const GeoDataCoordinates &a, const GeoDataCoordinates &b )
{
    qreal distance = EARTH_RADIUS * distanceSphere( a, b);

    int precision = 0;
    QString distanceUnit = QLatin1String( "m" );

    if ( MarbleGlobal::getInstance()->locale()->measurementSystem() == MarbleLocale::ImperialSystem ) {
        precision = 1;
        distanceUnit = "mi";
        distance *= METER2KM;
        distance *= KM2MI;
    } else if (MarbleGlobal::getInstance()->locale()->measurementSystem() ==
               MarbleLocale::MetricSystem) {
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
    } else if (MarbleGlobal::getInstance()->locale()->measurementSystem() ==
               MarbleLocale::NauticalSystem) {
        precision = 2;
        distanceUnit = "nm";
        distance *= METER2KM;
        distance *= KM2NM;
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

    return fuzzyDistance + QLatin1Char(' ') + heading;
}

qreal OsmDatabase::bearing( const GeoDataCoordinates &a, const GeoDataCoordinates &b )
{
    qreal delta = b.longitude() - a.longitude();
    qreal lat1 = a.latitude();
    qreal lat2 = b.latitude();
    return fmod( atan2( sin ( delta ) * cos ( lat2 ),
                       cos( lat1 ) * sin( lat2 ) - sin( lat1 ) * cos( lat2 ) * cos ( delta ) ), 2 * M_PI );
}

QString OsmDatabase::wildcardQuery( const QString &term )
{
    QString result = term;
    if (term.contains(QLatin1Char('*'))) {
        return QLatin1String(" LIKE '") + result.replace(QLatin1Char('*'), QLatin1Char('%')) + QLatin1Char('\'');
    } else {
        return QLatin1String(" = '") + result + QLatin1Char('\'');
    }
}

}
