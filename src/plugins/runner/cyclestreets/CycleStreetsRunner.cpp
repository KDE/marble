//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mihail Ivchenko <ematirov@gmail.com>
// Copyright 2017      Sergey Popov <sergobot@protonmail.com>
//

#include "CycleStreetsRunner.h"

#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"
#include "HttpDownloadManager.h"
#include "routing/Maneuver.h"
#include "routing/RouteRequest.h"

#include <QUrl>
#include <QTimer>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QUrlQuery>

namespace Marble
{

CycleStreetsRunner::CycleStreetsRunner( QObject *parent ) :
    RoutingRunner( parent ),
    m_networkAccessManager(),
    m_request()
{
    connect( &m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(retrieveData(QNetworkReply*)) );

    turns.insert( "", Maneuver::Continue );
    turns.insert( "straight on", Maneuver::Straight );
    turns.insert( "bear right", Maneuver::SlightRight );
    turns.insert( "bear left", Maneuver::SlightLeft );
    turns.insert( "sharp right", Maneuver::SharpRight );
    turns.insert( "sharp left", Maneuver::SharpLeft );
    turns.insert( "turn right", Maneuver::Right );
    turns.insert( "turn left", Maneuver::Left );
    turns.insert( "double-back", Maneuver::TurnAround);
    turns.insert( "first exit", Maneuver::RoundaboutFirstExit );
    turns.insert( "second exit", Maneuver::RoundaboutSecondExit );
    turns.insert( "third exit", Maneuver::RoundaboutThirdExit );
    turns.insert( "fourth exit", Maneuver::RoundaboutExit );
    turns.insert( "fifth exit", Maneuver::RoundaboutExit );
    turns.insert( "sixth exit", Maneuver::RoundaboutExit );
    turns.insert( "seventh or more exit", Maneuver::RoundaboutExit );
}

CycleStreetsRunner::~CycleStreetsRunner()
{
    // nothing to do
}

void CycleStreetsRunner::retrieveRoute( const RouteRequest *route )
{
    if ( route->size() < 2 || route->size() > 12 ) {
        return;
    }

    QHash<QString, QVariant> settings = route->routingProfile().pluginSettings()[QStringLiteral("cyclestreets")];

    QUrl url("https://www.cyclestreets.net/api/journey.json");
    QMap<QString, QString> queryStrings;
    queryStrings["key"] = "cdccf13997d59e70";
    queryStrings["reporterrors"] = QLatin1Char('1');
    queryStrings["plan"] = settings[QStringLiteral("plan")].toString();
    if (queryStrings["plan"].isEmpty()) {
        mDebug() << Q_FUNC_INFO << "Missing a value for 'plan' in the settings, falling back to 'balanced'";
        queryStrings["plan"] = QStringLiteral("balanced");
    }
    queryStrings["speed"] = settings[QStringLiteral("speed")].toString();
    if (queryStrings["speed"].isEmpty()) {
        mDebug() << Q_FUNC_INFO << "Missing a value for 'speed' in the settings, falling back to '20'";
        queryStrings["speed"] = QStringLiteral("20");
    }
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    QString itinerarypoints;
    itinerarypoints.append(QString::number(route->source().longitude(degree), 'f', 6) + QLatin1Char(',') + QString::number(route->source().latitude(degree), 'f', 6));
    for ( int i=1; i<route->size(); ++i ) {
        itinerarypoints.append(QLatin1Char('|') +  QString::number(route->at(i).longitude(degree), 'f', 6) + QLatin1Char(',') + QString::number(route->at(i).latitude(degree), 'f', 6));
    }
    queryStrings["itinerarypoints"] = itinerarypoints;

	QUrlQuery urlQuery;
    for( const QString& key: queryStrings.keys()){
		urlQuery.addQueryItem(key, queryStrings.value(key));
	}
	url.setQuery( urlQuery);

    m_request.setUrl( url );
    m_request.setRawHeader( "User-Agent", HttpDownloadManager::userAgent( "Browser", "CycleStreetsRunner" ) );

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot( true );
    timer.setInterval( 15000 );

    connect( &timer, SIGNAL(timeout()),
             &eventLoop, SLOT(quit()));
    connect( this, SIGNAL(routeCalculated(GeoDataDocument*)),
             &eventLoop, SLOT(quit()) );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT(get()) );
    timer.start();

    eventLoop.exec();
}

void CycleStreetsRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager.get( m_request );
    connect( reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this, SLOT(handleError(QNetworkReply::NetworkError)), Qt::DirectConnection );
}

void CycleStreetsRunner::retrieveData( QNetworkReply *reply )
{
    if ( reply->isFinished() ) {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        //mDebug() << "Download completed: " << data;
        GeoDataDocument *document = parse( data );

        if ( !document ) {
            mDebug() << "Failed to parse the downloaded route data" << data;
        }

        emit routeCalculated( document );
    }
}

int CycleStreetsRunner::maneuverType(QString& cycleStreetsName) const
{
    if ( turns.contains( cycleStreetsName ) ) {
        return turns[cycleStreetsName];
    }
    return Maneuver::Unknown;
}

GeoDataDocument *CycleStreetsRunner::parse( const QByteArray &content ) const
{
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(content, &error);

    if ( json.isEmpty() ) {
        mDebug() << "Cannot parse json file with routing instructions: " << error.errorString();
        return Q_NULLPTR;
    }

    // Check if CycleStreets has found any error
    if ( !json.object()["error"].isNull() ) {
        mDebug() << "CycleStreets reported an error: " << json.object()["error"].toString();
        return Q_NULLPTR;
    }

    GeoDataDocument *result = new GeoDataDocument();
    result->setName(QStringLiteral("CycleStreets"));
    GeoDataPlacemark *routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName(QStringLiteral("Route"));

    GeoDataLineString *routeWaypoints = new GeoDataLineString;
    QJsonArray features = json.object()["marker"].toArray();

    if ( features.isEmpty() ) {
        return Q_NULLPTR ;
    }
    QJsonObject route = features.first().toObject()["@attributes"].toObject();
    QJsonValue coordinates = route["coordinates"];
    QStringList coordinatesList = coordinates.toString().split(QLatin1Char(' '));

    QStringList::iterator iter = coordinatesList.begin();
    QStringList::iterator end = coordinatesList.end();

    for( ; iter != end; ++iter) {
        const QStringList coordinate =  iter->split(QLatin1Char(','));
        if ( coordinate.size() == 2 ) {
            double const lon = coordinate.at( 0 ).toDouble();
            double const lat = coordinate.at( 1 ).toDouble();
            GeoDataCoordinates const position( lon, lat, 0.0, GeoDataCoordinates::Degree );
            routeWaypoints->append( position );
        }
    }
    routePlacemark->setGeometry( routeWaypoints );

    QTime duration;
    duration = duration.addSecs( route["time"].toInt() );
    qreal length = routeWaypoints->length( EARTH_RADIUS );

    const QString name = nameString( "CS", length, duration );
    const GeoDataExtendedData data = routeData( length, duration );
    routePlacemark->setExtendedData( data );
    result->setName( name );
    result->append( routePlacemark );

    for (int i = 1; i < features.count(); ++i) {
        QJsonObject segment = features.at( i ).toObject()["@attributes"].toObject();

        QString name = segment["name"].toString();
        QString maneuver = segment["turn"].toString();
        QStringList points = segment["points"].toString().split(QLatin1Char(' '));
        QStringList const elevation = segment["elevations"].toString().split(QLatin1Char(','));

        GeoDataPlacemark *instructions = new GeoDataPlacemark;
        QString instructionName;
        if ( !maneuver.isEmpty() ) {
            instructionName = maneuver.left( 1 ).toUpper() + maneuver.mid( 1 );
        } else {
            instructionName = "Straight";
        }
        if (name != QLatin1String("Short un-named link") && name != QLatin1String("Un-named link")) {
            instructionName.append(QLatin1String(" into ") + name);
        }
        instructions->setName( instructionName );

        GeoDataExtendedData extendedData;
        GeoDataData turnType;
        turnType.setName(QStringLiteral("turnType"));
        turnType.setValue( maneuverType( maneuver ) );
        extendedData.addValue( turnType );

        instructions->setExtendedData( extendedData );
        GeoDataLineString *lineString = new GeoDataLineString;
        QStringList::iterator iter = points.begin();
        QStringList::iterator end = points.end();
        for  ( int j=0; iter != end; ++iter, ++j ) {
            const QStringList coordinate = iter->split(QLatin1Char(','));
            if ( coordinate.size() == 2 ) {
                double const lon = coordinate.at( 0 ).toDouble();
                double const lat = coordinate.at( 1 ).toDouble();
                double const alt = j < elevation.size() ? elevation[j].toDouble() : 0.0;
                lineString->append( GeoDataCoordinates( lon, lat, alt, GeoDataCoordinates::Degree ) );
            }
        }
        instructions->setGeometry( lineString );
        result->append( instructions );
    }
    return result;
}

void CycleStreetsRunner::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving cyclestreets.net route: " << error;
}

} // namespace Marble

#include "moc_CycleStreetsRunner.cpp"
