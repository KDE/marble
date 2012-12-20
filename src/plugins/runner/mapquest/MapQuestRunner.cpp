//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MapQuestRunner.h"

#include "MarbleAbstractRunner.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "TinyWebBrowser.h"
#include "routing/Maneuver.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

MapQuestRunner::MapQuestRunner( QObject *parent ) :
    MarbleAbstractRunner( parent ),
    m_networkAccessManager( new QNetworkAccessManager( this ) ),
    m_request()
{
    connect( m_networkAccessManager, SIGNAL( finished( QNetworkReply * ) ),
             this, SLOT( retrieveData( QNetworkReply * ) ) );
}

MapQuestRunner::~MapQuestRunner()
{
    // nothing to do
}

GeoDataFeature::GeoDataVisualCategory MapQuestRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void MapQuestRunner::retrieveRoute( const RouteRequest *route )
{
    if ( route->size() < 2 ) {
        return;
    }

    QHash<QString, QVariant> settings = route->routingProfile().pluginSettings()["mapquest"];

    QString url = "http://open.mapquestapi.com/directions/v0/route?callback=renderAdvancedNarrative&outFormat=xml&narrativeType=text&shapeFormat=raw&generalize=0";
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    append( &url, "from", QString::number( route->source().latitude( degree ), 'f', 6 ) + "," + QString::number( route->source().longitude( degree ), 'f', 6 ) );
    for ( int i=1; i<route->size(); ++i ) {
        append( &url, "to", QString::number( route->at( i ).latitude( degree ), 'f', 6 ) + "," + QString::number( route->at( i ).longitude( degree ), 'f', 6 ) );
    }

    QString const unit = MarbleGlobal::getInstance()->locale()->measurementSystem() == QLocale::MetricSystem ? "k" : "m";
    append( &url, "units", unit );

    if ( settings["noMotorways"].toInt() ) {
        append( &url, "avoids", "Limited Access" );
    }
    if ( settings["noTollroads"].toInt() ) {
        append( &url, "avoids", "Toll road" );
    }
    if ( settings["noFerries"].toInt() ) {
        append( &url, "avoids", "Ferry" );
    }

    if ( !settings["preference"].toString().isEmpty() ) {
        append( &url, "routeType", settings["preference"].toString() );
    }

    m_request.setUrl( QUrl( url ) );
    m_request.setRawHeader( "User-Agent", TinyWebBrowser::userAgent( "Browser", "MapQuestRunner" ) );

    QEventLoop eventLoop;

    connect( this, SIGNAL( routeCalculated( GeoDataDocument* ) ),
             &eventLoop, SLOT( quit() ) );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT( get() ) );

    eventLoop.exec();
}

void MapQuestRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager->get( m_request );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( handleError( QNetworkReply::NetworkError ) ), Qt::DirectConnection );
}

void MapQuestRunner::retrieveData( QNetworkReply *reply )
{
    if ( reply->isFinished() ) {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        //mDebug() << "Download completed: " << data;
        GeoDataDocument* document = parse( data );

        if ( !document ) {
            mDebug() << "Failed to parse the downloaded route data" << data;
        }

        emit routeCalculated( document );
    }
}

void MapQuestRunner::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving mapquest.org route: " << error;
}

void MapQuestRunner::append(QString *input, const QString &key, const QString &value)
{
    *input += "&" + key + "=" + value;
}

int MapQuestRunner::maneuverType( int mapQuestId ) const
{
    /** @todo FIXME: review 10, 11 */
    switch( mapQuestId ) {
    case 0: return Maneuver::Straight ; // straight
    case 1: return Maneuver::SlightRight ; // slight right
    case 2: return Maneuver::Right ; // right
    case 3: return Maneuver::SharpRight ; // sharp right
    case 4: return Maneuver::TurnAround ; // reverse
    case 5: return Maneuver::SharpLeft ; // sharp left
    case 6: return Maneuver::Left ; // left
    case 7: return Maneuver::SlightLeft ; // slight left
    case 8: return Maneuver::TurnAround ; // right u-turn
    case 9: return Maneuver::TurnAround ; // left u-turn
    case 10: return Maneuver::Merge ; // right merge
    case 11: return Maneuver::Merge ; // left merge
    case 12: return Maneuver::Merge ; // right on ramp
    case 13: return Maneuver::Merge ; // left on ramp
    case 14: return Maneuver::ExitRight ; // right off ramp
    case 15: return Maneuver::ExitLeft ; // left off ramp
    case 16: return Maneuver::Right ; // right fork
    case 17: return Maneuver::Left ; // left fork
    case 18: return Maneuver::Continue ; // straight fork
    }

    return Maneuver::Unknown;
}

GeoDataDocument* MapQuestRunner::parse( const QByteArray &content ) const
{
    QDomDocument xml;
    if ( !xml.setContent( content ) ) {
        mDebug() << "Cannot parse xml file with routing instructions.";
        return 0;
    }

    // mDebug() << xml.toString(2);
    QDomElement root = xml.documentElement();

    GeoDataDocument* result = new GeoDataDocument();
    result->setName( "MapQuest" );
    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName( "Route" );

    GeoDataLineString* routeWaypoints = new GeoDataLineString;
    QDomNodeList shapePoints = root.elementsByTagName( "shapePoints" );
    if ( shapePoints.size() == 1 ) {
        QDomNodeList geometry = shapePoints.at( 0 ).toElement().elementsByTagName( "latLng" );
        for ( int i=0; i<geometry.size(); ++i ) {
            double const lat = geometry.item( i ).namedItem( "lat" ).toElement().text().toDouble();
            double const lon = geometry.item( i ).namedItem( "lng" ).toElement().text().toDouble();
            GeoDataCoordinates const position( lon, lat, 0.0, GeoDataCoordinates::Degree );
            routeWaypoints->append( position );
        }
    }
    routePlacemark->setGeometry( routeWaypoints );

    QString name = "%1 %2 (MapQuest)";
    QString unit = "m";
    qreal length = routeWaypoints->length( EARTH_RADIUS );
    if (length >= 1000) {
        length /= 1000.0;
        unit = "km";
    }
    result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );
    result->append( routePlacemark );

    QMap<int,int> mapping;
    QDomNodeList maneuvers = root.elementsByTagName( "maneuverIndexes" );
    if ( maneuvers.size() == 1 ) {
        maneuvers = maneuvers.at( 0 ).childNodes();
        for ( int i=0; i<maneuvers.size(); ++i ) {
            mapping[i] = maneuvers.at( i ).toElement().text().toInt();
            if ( mapping[i] == routeWaypoints->size() ) {
                --mapping[i];
            }
        }
    }

    QDomNodeList instructions = root.elementsByTagName( "maneuver" );
    unsigned int const lastInstruction = qMax<int>( 0, instructions.length()-1 ); // ignore the last 'Welcome to xy' instruction
    for ( unsigned int i = 0; i < lastInstruction; ++i ) {
        QDomElement node = instructions.item( i ).toElement();

        QDomNodeList maneuver = node.elementsByTagName( "turnType" );
        QDomNodeList textNodes = node.elementsByTagName( "narrative" );
        QDomNodeList points = node.elementsByTagName( "startPoint" );
        QDomNodeList streets = node.elementsByTagName( "streets" );

        Q_ASSERT( mapping.contains( i ) );
        if ( textNodes.size() == 1 && maneuver.size() == 1 && points.size() == 1 && mapping.contains( i ) ) {
            GeoDataPlacemark* instruction = new GeoDataPlacemark;
            instruction->setName( textNodes.at( 0 ).toElement().text() );

            GeoDataExtendedData extendedData;
            GeoDataData turnType;
            turnType.setName( "turnType" );
            turnType.setValue( maneuverType( maneuver.at( 0 ).toElement().text().toInt() ) );
            extendedData.addValue( turnType );
            if ( streets.size() == 1 ) {
                GeoDataData roadName;
                roadName.setName( "roadName" );
                roadName.setValue( streets.at( 0 ).toElement().text() );
                extendedData.addValue( roadName );
            }
            instruction->setExtendedData( extendedData );

            int const start = mapping[i];
            int const end = mapping.contains(i+1) ? mapping[i+1] : routeWaypoints->size()-1;
            if ( start >= 0 && start < routeWaypoints->size() && end < routeWaypoints->size() ) {
                instruction->setName( textNodes.item( 0 ).toElement().text() );
                GeoDataLineString *lineString = new GeoDataLineString;
                for ( int j=start; j<=end; ++j ) {
                    *lineString << GeoDataCoordinates( routeWaypoints->at( j ).longitude(), routeWaypoints->at( j ).latitude() );
                }

                if ( !lineString->isEmpty() ) {
                    instruction->setGeometry( lineString );
                    result->append( instruction );
                }
            }
        }
    }

    if ( routeWaypoints->size() < 1 ) {
        delete result;
        result = 0;
    }

    return result;
}


} // namespace Marble

#include "MapQuestRunner.moc"
