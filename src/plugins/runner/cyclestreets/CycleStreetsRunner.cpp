//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mihail Ivchenko <ematirov@gmail.com>
//

#include "CycleStreetsRunner.h"

#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "TinyWebBrowser.h"
#include "routing/Maneuver.h"
#include "routing/RouteRequest.h"

#include <QUrl>
#include <QTimer>
#include <QNetworkReply>
#include <QDomDocument>
#include <QVector>
#include <QPair>

#if QT_VERSION >= 0x050000
  #include <QUrlQuery>
#endif

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

    QHash<QString, QVariant> settings = route->routingProfile().pluginSettings()["cyclestreets"];

    QUrl url("http://www.cyclestreets.net/api/journey.xml");
    QMap<QString, QString> queryStrings;
    queryStrings["key"] = "cdccf13997d59e70";
    queryStrings["useDom"] = '1';
    queryStrings["plan"] = settings["plan"].toString();
    queryStrings["speed"] = settings["speed"].toString();
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    QString itinerarypoints;
    itinerarypoints.append( QString::number( route->source().longitude( degree ), 'f', 6 ) + ',' + QString::number( route->source().latitude( degree ), 'f', 6 ) );
    for ( int i=1; i<route->size(); ++i ) {
        itinerarypoints.append( '|' +  QString::number( route->at( i ).longitude( degree ), 'f', 6 ) + ',' + QString::number( route->at( i ).latitude( degree ), 'f', 6 ) );
    }
    queryStrings["itinerarypoints"] = itinerarypoints;

#if QT_VERSION >= 0x050000
	QUrlQuery urlQuery;
	Q_FOREACH( const QString& key, queryStrings.keys()){
		urlQuery.addQueryItem(key, queryStrings.value(key));
	}
	url.setQuery( urlQuery);
#else
    typedef QMap<QString, QString>::ConstIterator Iterator;
    Iterator end = queryStrings.constEnd();
    for ( Iterator iter = queryStrings.constBegin(); iter != end; ++iter ) {
        url.addQueryItem(iter.key(), iter.value());
    }
#endif

    m_request.setUrl( url );
    m_request.setRawHeader( "User-Agent", TinyWebBrowser::userAgent( "Browser", "CycleStreetsRunner" ) );

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
    QDomDocument xml;
    if ( !xml.setContent( content ) ) {
        mDebug() << "Cannot parse xml file with routing instructions.";
        return 0;
    }
    GeoDataDocument *result = new GeoDataDocument();
    result->setName( "CycleStreets" );
    GeoDataPlacemark *routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName( "Route" );

    GeoDataLineString *routeWaypoints = new GeoDataLineString;
    QDomNodeList features = xml.elementsByTagName( "gml:featureMember" );

    if ( features.isEmpty() ) {
        return 0;
    }
    QDomElement route = features.at( 0 ).toElement().firstChild().toElement();
    QDomElement lineString = route.elementsByTagName( "gml:LineString" ).at( 0 ).toElement();
    QDomElement coordinates = lineString.toElement().elementsByTagName( "gml:coordinates" ).at( 0 ).toElement();
    QStringList coordinatesList = coordinates.text().split( ' ' );

    QStringList::iterator iter = coordinatesList.begin();
    QStringList::iterator end = coordinatesList.end();

    for( ; iter != end; ++iter) {
        QStringList coordinate =  iter->split(',');
        if ( coordinate.size() == 2 ) {
            double const lon = coordinate.at( 0 ).toDouble();
            double const lat = coordinate.at( 1 ).toDouble();
            GeoDataCoordinates const position( lon, lat, 0.0, GeoDataCoordinates::Degree );
            routeWaypoints->append( position );
        }
    }
    routePlacemark->setGeometry( routeWaypoints );

    QDomElement durationElement = route.elementsByTagName( "cs:time" ).at(0).toElement();
    QTime duration;
    duration = duration.addSecs( durationElement.text().toInt() );
    qreal length = routeWaypoints->length( EARTH_RADIUS );

    const QString name = nameString( "CS", length, duration );
    const GeoDataExtendedData data = routeData( length, duration );
    routePlacemark->setExtendedData( data );
    result->setName( name );
    result->append( routePlacemark );

    int i;
    for ( i = 1; i < features.count() && features.at( i ).firstChildElement().tagName() != "cs:segment"; ++i );
    for ( ; i < features.count(); ++i) {
        QDomElement segment = features.at( i ).toElement();

        QString name = segment.elementsByTagName( "cs:name" ).at( 0 ).toElement().text();
        QString maneuver = segment.elementsByTagName( "cs:turn" ).at( 0 ).toElement().text();
        QStringList points = segment.elementsByTagName( "cs:points" ).at( 0 ).toElement().text().split( ' ' );
        QStringList const elevation = segment.elementsByTagName( "cs:elevations" ).at( 0 ).toElement().text().split( ',' );

        GeoDataPlacemark *instructions = new GeoDataPlacemark;
        QString instructionName;
        if ( !maneuver.isEmpty() ) {
            instructionName = maneuver.left( 1 ).toUpper() + maneuver.mid( 1 );
        } else {
            instructionName = "Straight";
        }
        if ( name != "Short un-named link" && name != "Un-named link" ){
            instructionName.append( " into " + name );
        }
        instructions->setName( instructionName );

        GeoDataExtendedData extendedData;
        GeoDataData turnType;
        turnType.setName( "turnType" );
        turnType.setValue( maneuverType( maneuver ) );
        extendedData.addValue( turnType );

        instructions->setExtendedData( extendedData );
        GeoDataLineString *lineString = new GeoDataLineString;
        QStringList::iterator iter = points.begin();
        QStringList::iterator end = points.end();
        for  ( int j=0; iter != end; ++iter, ++j ) {
            QStringList coordinate = iter->split( ',' );
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

#include "CycleStreetsRunner.moc"
