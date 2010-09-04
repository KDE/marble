//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "TravelingSalesmanRunner.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteRequest.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

#include <QtCore/QProcess>
#include <QtCore/QMap>
#include <QtCore/QTemporaryFile>

namespace Marble
{

class TravelingSalesmanRunnerPrivate
{
public:
    QFileInfo m_travelingsalesmanJar;

    GeoDataLineString retrieveWaypoints( const QPair<QString, QString> &query ) const;

    GeoDataDocument* createDocument( GeoDataLineString* routeWaypoints ) const;

    GeoDataLineString parseTravelingSalesmanOutput( QFile &file ) const;

    void merge( GeoDataLineString* one, const GeoDataLineString& two ) const;

    /** Static to share the cache among all instances */
    static QMap<QString, GeoDataLineString> m_partialRoutes;
};

QMap<QString, GeoDataLineString> TravelingSalesmanRunnerPrivate::m_partialRoutes;

void TravelingSalesmanRunnerPrivate::merge( GeoDataLineString* one, const GeoDataLineString& two ) const
{
    Q_ASSERT( one );

    QVector<GeoDataCoordinates>::const_iterator iter = two.constBegin();
    for ( ; iter != two.constEnd(); ++iter ) {
        /** @todo: It might be needed to cut off some points at the start or end */
        one->append( *iter );
    }
}

GeoDataLineString TravelingSalesmanRunnerPrivate::retrieveWaypoints( const QPair<QString, QString> &query ) const
{
    QString cacheItem = query.first + query.second;
    if ( m_partialRoutes.contains( cacheItem ) ) {
        return m_partialRoutes[cacheItem];
    }

    QTemporaryFile gpxFile;
    if ( !gpxFile.open() ) {
        mDebug() << "Unable to create a temporary work file";
        return GeoDataLineString();
    }

    QProcess travelingsalesman;
    QStringList arguments = QStringList() << "-jar" << m_travelingsalesmanJar.absoluteFilePath();
    arguments << "route" << "-gpx" << gpxFile.fileName();
    arguments << query.first << query.second;
    travelingsalesman.start( "java", arguments );
    if ( !travelingsalesman.waitForStarted( 5000 ) ) {
        mDebug() << "Couldn't start travelingsalesman from the current PATH. Is java setup correctly?";
        return GeoDataLineString();
    }

    if ( travelingsalesman.waitForFinished( 60 * 1000 ) ) {
        m_partialRoutes[cacheItem] = parseTravelingSalesmanOutput( gpxFile );
        return m_partialRoutes[cacheItem];
    } else {
        mDebug() << "Couldn't stop travelingsalesman";
    }

    return GeoDataLineString();
}

GeoDataLineString TravelingSalesmanRunnerPrivate::parseTravelingSalesmanOutput( QFile &file ) const
{
    GeoDataParser parser( GeoData_GPX );
    if ( !parser.read( &file ) ) {
        mDebug() << "Could not parse gpx file " << file.fileName();
        return GeoDataLineString();
    }

    GeoDataLineString result;
    GeoDocument* document = parser.releaseDocument();
    GeoDataDocument* route = dynamic_cast<GeoDataDocument*>( document );

    if ( route ) {
        if ( route->placemarkList().size() == 1 ) {
            GeoDataPlacemark* placemark = route->placemarkList().first();
            GeoDataMultiGeometry* multi = dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );
            if ( multi && multi->size() == 1 ) {
                GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( &multi->first() );
                if ( lineString ) {
                    return *lineString;
                }
            }
        }
    }
    delete document;
    return result;
}

GeoDataDocument* TravelingSalesmanRunnerPrivate::createDocument( GeoDataLineString* routeWaypoints ) const
{
    if ( !routeWaypoints || routeWaypoints->isEmpty() ) {
        return 0;
    }

    GeoDataDocument* result = new GeoDataDocument();
    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName( "Route" );
    routePlacemark->setGeometry( routeWaypoints );
    result->append( routePlacemark );

    QString name = "%1 %2 (Traveling Salesman)";
    QString unit = "m";
    qreal length = routeWaypoints->length( EARTH_RADIUS );
    if ( length >= 1000 ) {
        length /= 1000.0;
        unit = "km";
    }
    result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );
    return result;
}

TravelingSalesmanRunner::TravelingSalesmanRunner( QObject *parent ) :
        MarbleAbstractRunner( parent ),
        d( new TravelingSalesmanRunnerPrivate )
{
    // Check installation
    QDir mapDir( MarbleDirs::localPath() + "/maps/earth/traveling-salesman/" );
    d->m_travelingsalesmanJar = QFileInfo ( mapDir, "traveling-salesman.jar" );
}

TravelingSalesmanRunner::~TravelingSalesmanRunner()
{
    delete d;
}

GeoDataFeature::GeoDataVisualCategory TravelingSalesmanRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void TravelingSalesmanRunner::retrieveRoute( RouteRequest *route )
{
    if ( !d->m_travelingsalesmanJar.exists() ) {
        emit routeCalculated( 0 );
        return;
    }

    GeoDataLineString* wayPoints = new GeoDataLineString;

    for ( int i = 0; i < route->size() - 1; ++i ) {
        QPair<QString, QString> queryString;
        GeoDataCoordinates source = route->at( i );
        double fLon = source.longitude( GeoDataCoordinates::Degree );
        double fLat = source.latitude( GeoDataCoordinates::Degree );
        queryString.first = QString( "[%1,%2]" ).arg( fLat, 0, 'f', 8 ).arg( fLon, 0, 'f', 8 );
        GeoDataCoordinates destination = route->at( i + 1 );
        double tLon = destination.longitude( GeoDataCoordinates::Degree );
        double tLat = destination.latitude( GeoDataCoordinates::Degree );
        queryString.second = QString( "[%1,%2]" ).arg( tLat, 0, 'f', 8 ).arg( tLon, 0, 'f', 8 );

        d->merge( wayPoints, d->retrieveWaypoints( queryString ) );
    }

    GeoDataDocument* result = d->createDocument( wayPoints );
    emit routeCalculated( result );
}

} // namespace Marble
