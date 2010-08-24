//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GosmoreRunner.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteSkeleton.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"

#include <QtCore/QProcess>
#include <QtCore/QMap>

namespace Marble
{

class GosmoreRunnerPrivate
{
public:
    QFileInfo m_gosmoreMapFile;

    QByteArray retrieveWaypoints( const QString &query ) const;

    GeoDataDocument* createDocument( GeoDataLineString* routeWaypoints ) const;

    GeoDataLineString parseGosmoreOutput( const QByteArray &content ) const;

    void merge( GeoDataLineString* one, const GeoDataLineString& two ) const;

    /** Static to share the cache among all instances */
    static QMap<QString, QByteArray> m_partialRoutes;
};

QMap<QString, QByteArray> GosmoreRunnerPrivate::m_partialRoutes;

void GosmoreRunnerPrivate::merge( GeoDataLineString* one, const GeoDataLineString& two ) const
{
    Q_ASSERT( one );

    QVector<GeoDataCoordinates>::const_iterator iter = two.constBegin();
    for( ; iter != two.constEnd(); ++iter ) {
        /** @todo: It might be needed to cut off some points at the start or end */
        one->append( *iter );
    }
}

QByteArray GosmoreRunnerPrivate::retrieveWaypoints( const QString &query ) const
{
    if ( m_partialRoutes.contains(query) ) {
        return m_partialRoutes[query];
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QUERY_STRING", query);
    env.insert("LC_ALL", "C");
    QProcess gosmore;
    gosmore.setProcessEnvironment(env);

    gosmore.start("gosmore", QStringList() << m_gosmoreMapFile.absoluteFilePath() );
    if (!gosmore.waitForStarted(5000)) {
        mDebug() << "Couldn't start gosmore from the current PATH. Install it to retrieve routing results from gosmore.";
        return QByteArray();
    }

    if ( gosmore.waitForFinished(15000) ) {
        m_partialRoutes[query] = gosmore.readAllStandardOutput();
        return m_partialRoutes[query];
    }
    else {
        mDebug() << "Couldn't stop gosmore";
    }

    return QByteArray();
}

GeoDataLineString GosmoreRunnerPrivate::parseGosmoreOutput( const QByteArray &content ) const
{
    GeoDataLineString routeWaypoints;

    QStringList lines = QString::fromLocal8Bit( content ).split( '\r' );
    foreach( const QString &line, lines ) {
        QStringList fields = line.split(',');
        if (fields.size() >= 5) {
            qreal lon = fields.at(1).toDouble();
            qreal lat = fields.at(0).toDouble();
            GeoDataCoordinates coordinates( lon, lat, 0.0, GeoDataCoordinates::Degree );
            routeWaypoints.append( coordinates );
        }
    }

    return routeWaypoints;
}

GeoDataDocument* GosmoreRunnerPrivate::createDocument( GeoDataLineString* routeWaypoints ) const
{
    if ( !routeWaypoints || routeWaypoints->isEmpty() ) {
        return 0;
    }

    GeoDataDocument* result = new GeoDataDocument();
    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName( "Route" );
    routePlacemark->setGeometry( routeWaypoints );
    result->append( routePlacemark );

    QString name = "%1 %2 (Gosmore)";
    QString unit = "m";
    qreal length = routeWaypoints->length( EARTH_RADIUS );
    if (length >= 1000) {
        length /= 1000.0;
        unit = "km";
    }
    result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );
    return result;
}

GosmoreRunner::GosmoreRunner( QObject *parent ) :
        MarbleAbstractRunner( parent ),
        d( new GosmoreRunnerPrivate )
{
    // Check installation
    QDir mapDir( MarbleDirs::localPath() + "/maps/earth/gosmore/" );
    d->m_gosmoreMapFile = QFileInfo ( mapDir, "gosmore.pak" );
}

GosmoreRunner::~GosmoreRunner()
{
    delete d;
}

GeoDataFeature::GeoDataVisualCategory GosmoreRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void GosmoreRunner::retrieveRoute( RouteSkeleton *route )
{
    if ( !d->m_gosmoreMapFile.exists() )
    {
        emit routeCalculated( 0 );
        return;
    }

    GeoDataLineString* wayPoints = new GeoDataLineString;

    for( int i=0; i<route->size()-1; ++i )
    {
        QString queryString = "flat=%1&flon=%2&tlat=%3&tlon=%4&fastest=1&v=motorcar";
        GeoDataCoordinates source = route->at(i);
        double fLon = source.longitude( GeoDataCoordinates::Degree );
        double fLat = source.latitude( GeoDataCoordinates::Degree );
        queryString = queryString.arg(fLat, 0, 'f', 8).arg(fLon, 0, 'f', 8);
        GeoDataCoordinates destination = route->at(i+1);
        double tLon = destination.longitude( GeoDataCoordinates::Degree );
        double tLat = destination.latitude( GeoDataCoordinates::Degree );
        queryString = queryString.arg(tLat, 0, 'f', 8).arg(tLon, 0, 'f', 8);

        d->merge( wayPoints, d->parseGosmoreOutput( d->retrieveWaypoints( queryString ) ) );
    }

    GeoDataDocument* result = d->createDocument( wayPoints );
    emit routeCalculated( result );
}

void GosmoreRunner::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    QString queryString = "flat=%1&flon=%2&tlat=%1&tlon=%2&fastest=1&v=motorcar";
    double lon = coordinates.longitude( GeoDataCoordinates::Degree );
    double lat = coordinates.latitude( GeoDataCoordinates::Degree );
    queryString = queryString.arg( lat, 0, 'f', 8).arg(lon, 0, 'f', 8 );
    QByteArray output = d->retrieveWaypoints( queryString );

    GeoDataPlacemark placemark;
    placemark.setCoordinate( GeoDataPoint( coordinates ) );

    QStringList lines = QString::fromUtf8( output ).split( '\r' );
    if ( lines.size() > 2 ) {
        QStringList fields = lines.at( lines.size()-2 ).split(',');
        if ( fields.size() >= 5 ) {
            QString road = fields.last();
            placemark.setAddress( road );
            GeoDataExtendedData extendedData;
            GeoDataData data;
            data.setValue( road );
            extendedData.addValue( "road", data );
            placemark.setExtendedData( extendedData );
        }
    }

    emit reverseGeocodingFinished( coordinates, placemark );
}

} // namespace Marble
