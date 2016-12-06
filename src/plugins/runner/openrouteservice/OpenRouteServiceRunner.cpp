//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "OpenRouteServiceRunner.h"

#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataLineString.h"
#include "routing/RouteRequest.h"

#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include <QTime>
#include <QTimer>
#include <QNetworkReply>
#include <QDomDocument>

namespace Marble
{

OpenRouteServiceRunner::OpenRouteServiceRunner( QObject *parent ) :
        RoutingRunner( parent ),
        m_networkAccessManager()
{
    connect( &m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(retrieveData(QNetworkReply*)));
}

void OpenRouteServiceRunner::retrieveRoute( const RouteRequest *route )
{
    if ( route->size() < 2 ) {
        return;
    }

    GeoDataCoordinates source = route->source();
    GeoDataCoordinates destination = route->destination();
    QHash<QString, QVariant> settings = route->routingProfile().pluginSettings()["openrouteservice"];

    QUrlQuery queries;
    queries.addQueryItem("api_key", "ee0b8233adff52ce9fd6afc2a2859a28");

    QString unit = "KM";
    QString preference = "Fastest";
    if (settings.contains(QStringLiteral("preference"))) {
        preference = settings[QStringLiteral("preference")].toString();
    }
    if (preference == QLatin1String("Pedestrian")) {
        unit = QStringLiteral("M");
    }

    queries.addQueryItem("start", formatCoordinates(source));
    QStringList via;
    for (int i = 1; i < route->size()-1; ++i) {
        via << formatCoordinates(route->at(i));
    }
    queries.addQueryItem("via", via.join(' '));
    queries.addQueryItem("end", formatCoordinates(destination));

    queries.addQueryItem("distunit", unit);
    if (preference == "Fastest" || preference == "Shortest" || preference == "Recommended") {
        queries.addQueryItem("routepref", "Car");
        queries.addQueryItem("weighting", preference);
    } else {
        queries.addQueryItem("routepref", preference);
        queries.addQueryItem("weighting", "Recommended");
    }

    QString const motorways = settings.value("noMotorways").toInt() == 0 ? "false" : "true";
    queries.addQueryItem("noMotorways", motorways);
    QString const tollways = settings.value("noTollways").toInt() == 0 ? "false" : "true";
    queries.addQueryItem("noTollways", tollways);
    queries.addQueryItem("noUnpavedroads", "false");
    queries.addQueryItem("noSteps", "false");
    QString const ferries = settings.value("noFerries").toInt() == 0 ? "false" : "true";
    queries.addQueryItem("noFerries", ferries);
    queries.addQueryItem("instructions", "true");
    queries.addQueryItem("lang", "en");

    QUrl url = QUrl( "http://openls.geog.uni-heidelberg.de/route" );
    // QUrlQuery strips empty value pairs, but OpenRouteService does not work without
    QString const trailer = route->size() == 2 ? "&via=" : QString();
    url.setQuery(queries.toString() + trailer);

    m_request = QNetworkRequest( url );

    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot( true );
    timer.setInterval( 15000 );

    connect( &timer, SIGNAL(timeout()),
             &eventLoop, SLOT(quit()));
    connect( this, SIGNAL(routeCalculated(GeoDataDocument*)),
             &eventLoop, SLOT(quit()));

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT(get()));
    timer.start();

    eventLoop.exec();
}

void OpenRouteServiceRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager.get(m_request);
    connect( reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this, SLOT(handleError(QNetworkReply::NetworkError)), Qt::DirectConnection);
}

QString OpenRouteServiceRunner::formatCoordinates(const GeoDataCoordinates &coordinates)
{
    return QStringLiteral("%1,%2")
            .arg(coordinates.longitude(GeoDataCoordinates::Degree ), 0, 'f', 8)
            .arg(coordinates.latitude(GeoDataCoordinates::Degree ), 0, 'f', 8);
}

void OpenRouteServiceRunner::retrieveData( QNetworkReply *reply )
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

void OpenRouteServiceRunner::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving openrouteservice.org route: " << error;
}

GeoDataDocument* OpenRouteServiceRunner::parse( const QByteArray &content ) const
{
    QDomDocument xml;
    if ( !xml.setContent( content ) ) {
        mDebug() << "Cannot parse xml file with routing instructions.";
        return 0;
    }

    QDomElement root = xml.documentElement();

    GeoDataDocument* result = new GeoDataDocument();
    result->setName(QStringLiteral("OpenRouteService"));

    QDomNodeList errors = root.elementsByTagName(QStringLiteral("xls:Error"));
    if ( errors.size() > 0 ) {
        return 0;
        // Returning early because fallback routing providers are used now
        // The code below can be used to parse OpenGis errors reported by ORS
        // and may be useful in the future

        for (int i=0 ; i < errors.length(); ++i ) {
            QDomNode node = errors.item( i );
            QString errorMessage = node.attributes().namedItem(QStringLiteral("message")).nodeValue();
            QRegExp regexp = QRegExp( "^(.*) Please Check your Position: (-?[0-9]+.[0-9]+) (-?[0-9]+.[0-9]+) !" );
            if ( regexp.indexIn( errorMessage ) == 0 ) {
                if ( regexp.capturedTexts().size() == 4 ) {
                    GeoDataPlacemark* placemark = new GeoDataPlacemark;
                    placemark->setName( regexp.capturedTexts().at( 1 ) );
                    GeoDataCoordinates position;
                    position.setLongitude( regexp.capturedTexts().at( 2 ).toDouble(), GeoDataCoordinates::Degree );
                    position.setLatitude( regexp.capturedTexts().at( 3 ).toDouble(), GeoDataCoordinates::Degree );
                    placemark->setCoordinate( position );
                    result->append( placemark );
                }
            } else {
                mDebug() << "Error message " << errorMessage << " not parsable.";
                /** @todo: How to handle this now with plugins? */
//                QString message = tr( "Sorry, a problem occurred when calculating the route. Try adjusting start and destination points." );
//                QPointer<QMessageBox> messageBox = new QMessageBox( QMessageBox::Warning, "Route Error", message );
//                messageBox->setDetailedText( errorMessage );
//                messageBox->exec();
//                delete messageBox;
            }
        }
    }

    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName(QStringLiteral("Route"));
    QTime time;
    QDomNodeList summary = root.elementsByTagName(QStringLiteral("xls:RouteSummary"));
    if ( summary.size() > 0 ) {
        QDomNodeList timeNodeList = summary.item(0).toElement().elementsByTagName(QStringLiteral("xls:TotalTime"));
        if ( timeNodeList.size() == 1 ) {
            QRegExp regexp = QRegExp( "^P(?:(\\d+)D)?T(?:(\\d+)H)?(?:(\\d+)M)?(\\d+)S" );
            if ( regexp.indexIn( timeNodeList.item( 0 ).toElement().text() ) == 0 ) {
                QStringList matches = regexp.capturedTexts();
                unsigned int hours( 0 ), minutes( 0 ), seconds( 0 );
                switch ( matches.size() ) {
                case 5:
                    // days    = regexp.cap( matches.size() - 4 ).toInt();
                    // Intentionally no break
                case 4:
                    hours   = regexp.cap( matches.size() - 3 ).toInt();
                    // Intentionally no break
                case 3:
                    minutes = regexp.cap( matches.size() - 2 ).toInt();
                    // Intentionally no break
                case 2:
                    seconds = regexp.cap( matches.size() - 1 ).toInt();
                    break;
                default:
                    mDebug() << "Unable to parse time string " << timeNodeList.item( 0 ).toElement().text();
                }

                time = QTime( hours, minutes, seconds, 0 );
            }
        }
    }

    GeoDataLineString* routeWaypoints = new GeoDataLineString;
    QDomNodeList geometry = root.elementsByTagName(QStringLiteral("xls:RouteGeometry"));
    if ( geometry.size() > 0 ) {
        QDomNodeList waypoints = geometry.item( 0 ).toElement().elementsByTagName( "gml:pos" );
        for (int i=0 ; i < waypoints.length(); ++i ) {
            QDomNode node = waypoints.item( i );
            const QStringList content = node.toElement().text().split(QLatin1Char(' '));
            if ( content.length() == 2 ) {
                GeoDataCoordinates position;
                position.setLongitude( content.at( 0 ).toDouble(), GeoDataCoordinates::Degree );
                position.setLatitude( content.at( 1 ).toDouble(), GeoDataCoordinates::Degree );
                routeWaypoints->append( position );
            }
        }
    }
    routePlacemark->setGeometry( routeWaypoints );

    qreal length = routeWaypoints->length( EARTH_RADIUS );
    const QString name = nameString( "ORS", length, time );
    const GeoDataExtendedData data = routeData( length, time );
    routePlacemark->setExtendedData( data );
    result->setName( name );

    result->append( routePlacemark );

    QDomNodeList instructionList = root.elementsByTagName(QStringLiteral("xls:RouteInstructionsList"));
    if ( instructionList.size() > 0 ) {
        QDomNodeList instructions = instructionList.item(0).toElement().elementsByTagName(QStringLiteral("xls:RouteInstruction"));
        for (int i=0 ; i < instructions.length(); ++i ) {
            QDomElement node = instructions.item( i ).toElement();

            QDomNodeList textNodes = node.elementsByTagName(QStringLiteral("xls:Instruction"));
            QDomNodeList positions = node.elementsByTagName(QStringLiteral("gml:pos"));

            if ( textNodes.size() > 0 && positions.size() > 0 ) {
                const QStringList content = positions.at(0).toElement().text().split(QLatin1Char(' '));
                if ( content.length() == 2 ) {
                    GeoDataLineString *lineString = new GeoDataLineString;

                    for( int i = 0; i < positions.count(); ++i ) {
                         const QStringList pointList = positions.at(i).toElement().text().split(QLatin1Char(' '));
                         GeoDataCoordinates position;
                         position.setLongitude( pointList.at( 0 ).toDouble(), GeoDataCoordinates::Degree );
                         position.setLatitude( pointList.at( 1 ).toDouble(), GeoDataCoordinates::Degree );
                         lineString->append( position );
                    }

                    GeoDataPlacemark* instruction = new GeoDataPlacemark;

                    QString const text = textNodes.item( 0 ).toElement().text().remove(QRegExp("<[^>]*>"));
                    GeoDataExtendedData extendedData;
                    GeoDataData turnTypeData;
                    turnTypeData.setName(QStringLiteral("turnType"));
                    QString road;
                    RoutingInstruction::TurnType turnType = parseTurnType( text, &road );
                    turnTypeData.setValue( turnType );
                    extendedData.addValue( turnTypeData );
                    if ( !road.isEmpty() ) {
                        GeoDataData roadName;
                        roadName.setName(QStringLiteral("roadName"));
                        roadName.setValue( road );
                        extendedData.addValue( roadName );
                    }

                    QString const instructionText = turnType == RoutingInstruction::Unknown ? text : RoutingInstruction::generateRoadInstruction( turnType, road );
                    instruction->setName( instructionText );
                    instruction->setExtendedData( extendedData );
                    instruction->setGeometry( lineString );
                    result->append( instruction );
                }
            }
        }
    }

    return result;
}

RoutingInstruction::TurnType OpenRouteServiceRunner::parseTurnType( const QString &text, QString *road )
{
    QRegExp syntax( "^(Go|Drive|Turn) (half left|left|sharp left|straight forward|half right|right|sharp right)( on )?(.*)?$", Qt::CaseSensitive, QRegExp::RegExp2 );
    QString instruction;
    if ( syntax.indexIn( text ) == 0 ) {
        if ( syntax.captureCount() > 1 ) {
            instruction = syntax.cap( 2 );
            if ( syntax.captureCount() == 4 ) {
                *road = syntax.cap( 4 ).remove(QLatin1String( " - Arrived at destination!"));
            }
        }
    }

    if (instruction == QLatin1String("Continue")) {
        return RoutingInstruction::Straight;
    } else if (instruction == QLatin1String("half right")) {
        return RoutingInstruction::SlightRight;
    } else if (instruction == QLatin1String("right")) {
        return RoutingInstruction::Right;
    } else if (instruction == QLatin1String("sharp right")) {
        return RoutingInstruction::SharpRight;
    } else if (instruction == QLatin1String("straight forward")) {
        return RoutingInstruction::Straight;
    } else if (instruction == QLatin1String("turn")) {
        return RoutingInstruction::TurnAround;
    } else if (instruction == QLatin1String("sharp left")) {
        return RoutingInstruction::SharpLeft;
    } else if (instruction == QLatin1String("left")) {
        return RoutingInstruction::Left;
    } else if (instruction == QLatin1String("half left")) {
        return RoutingInstruction::SlightLeft;
    }

    return RoutingInstruction::Unknown;
}

} // namespace Marble

#include "moc_OpenRouteServiceRunner.cpp"
