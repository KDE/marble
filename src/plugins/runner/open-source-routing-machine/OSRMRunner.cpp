// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "OSRMRunner.h"

#include "GeoDataData.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "routing/Maneuver.h"
#include "routing/RouteRequest.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QVector>

namespace Marble
{

OSRMRunner::OSRMRunner(QObject *parent)
    : RoutingRunner(parent)
    , m_networkAccessManager()
{
    connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(retrieveData(QNetworkReply *)));
}

OSRMRunner::~OSRMRunner()
{
    // nothing to do
}

void OSRMRunner::retrieveRoute(const RouteRequest *route)
{
    if (route->size() < 2) {
        return;
    }

    QString url = "http://router.project-osrm.org/route/v1/driving/";
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    for (int i = 0; i < route->size(); ++i) {
        GeoDataCoordinates const coordinates = route->at(i);
        url += QString::number(coordinates.longitude(degree), 'f', 6);
        url += ',';
        url += QString::number(coordinates.latitude(degree), 'f', 6);
        if (i + 1 < route->size()) {
            url += ';';
        }
    }

    url += QStringLiteral("?alternatives=false&overview=full&geometries=polyline6");

    m_request = QNetworkRequest(QUrl(url));
    m_request.setRawHeader("User-Agent", HttpDownloadManager::userAgent("Browser", "OSRMRunner"));

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(15000);

    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    connect(this, SIGNAL(routeCalculated(GeoDataDocument *)), &eventLoop, SLOT(quit()));

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot(0, this, SLOT(get()));
    timer.start();

    eventLoop.exec();
}

void OSRMRunner::retrieveData(QNetworkReply *reply)
{
    if (reply->isFinished()) {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        GeoDataDocument *document = parse(data);

        if (!document) {
            mDebug() << "Failed to parse the downloaded route data" << data;
        }

        emit routeCalculated(document);
    }
}

void OSRMRunner::handleError(QNetworkReply::NetworkError error)
{
    mDebug() << " Error when retrieving OSRM route: " << error;
}

void OSRMRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager.get(m_request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError(QNetworkReply::NetworkError)), Qt::DirectConnection);
}

void OSRMRunner::append(QString *input, const QString &key, const QString &value)
{
    *input += QLatin1Char('&') + key + QLatin1Char('=') + value;
}

GeoDataLineString *OSRMRunner::decodePolyline(const QString &geometry)
{
    // See https://developers.google.com/maps/documentation/utilities/polylinealgorithm
    GeoDataLineString *lineString = new GeoDataLineString;
    int coordinates[2] = {0, 0};
    int const length = geometry.length();
    for (int i = 0; i < length; /* increment happens below */) {
        for (int j = 0; j < 2; ++j) { // lat and lon
            int block(0), shift(0), result(0);
            do {
                block = geometry.at(i++ /* increment for outer loop */).toLatin1() - 63;
                result |= (block & 0x1F) << shift;
                shift += 5;
            } while (block >= 0x20);
            coordinates[j] += ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
        }
        lineString->append(GeoDataCoordinates(double(coordinates[1]) / 1E6, double(coordinates[0]) / 1E6, 0.0, GeoDataCoordinates::Degree));
    }
    return lineString;
}

RoutingInstruction::TurnType OSRMRunner::parseTurnType(const QString &instruction)
{
    if (instruction == QLatin1StringView("1")) {
        return RoutingInstruction::Straight;
    } else if (instruction == QLatin1StringView("2")) {
        return RoutingInstruction::SlightRight;
    } else if (instruction == QLatin1StringView("3")) {
        return RoutingInstruction::Right;
    } else if (instruction == QLatin1StringView("4")) {
        return RoutingInstruction::SharpRight;
    } else if (instruction == QLatin1StringView("5")) {
        return RoutingInstruction::TurnAround;
    } else if (instruction == QLatin1StringView("6")) {
        return RoutingInstruction::SharpLeft;
    } else if (instruction == QLatin1StringView("7")) {
        return RoutingInstruction::Left;
    } else if (instruction == QLatin1StringView("8")) {
        return RoutingInstruction::SlightLeft;
    } else if (instruction == QLatin1StringView("10")) {
        return RoutingInstruction::Continue;
    } else if (instruction.startsWith(QLatin1StringView("11-"))) {
        int const exit = QStringView{instruction}.mid(3).toInt();
        switch (exit) {
        case 1:
            return RoutingInstruction::RoundaboutFirstExit;
        case 2:
            return RoutingInstruction::RoundaboutSecondExit;
        case 3:
            return RoutingInstruction::RoundaboutThirdExit;
        default:
            return RoutingInstruction::RoundaboutExit;
        }
    } else if (instruction == QLatin1StringView("12")) {
        return RoutingInstruction::RoundaboutExit;
    }

    // ignoring ReachViaPoint = 9;
    // ignoring StayOnRoundAbout = 13;
    // ignoring StartAtEndOfStreet = 14;
    // ignoring ReachedYourDestination = 15;

    return RoutingInstruction::Unknown;
}

GeoDataDocument *OSRMRunner::parse(const QByteArray &input) const
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(input);
    QJsonObject data = jsonDoc.object();

    GeoDataDocument *result = nullptr;
    GeoDataLineString *routeWaypoints = nullptr;
    QJsonValue routeGeometryValue = data.value(QStringLiteral("routes"));
    if (routeGeometryValue.isArray()) {
        auto routes = routeGeometryValue.toArray();
        if (!routes.isEmpty()) {
            auto route = routes[0].toObject();
            auto routeGeometryValue = route.value(QStringLiteral("geometry"));
            if (routeGeometryValue.isString()) {
                result = new GeoDataDocument();
                result->setName(QStringLiteral("Open Source Routing Machine"));
                GeoDataPlacemark *routePlacemark = new GeoDataPlacemark;
                routePlacemark->setName(QStringLiteral("Route"));
                routeWaypoints = decodePolyline(routeGeometryValue.toString());
                routePlacemark->setGeometry(routeWaypoints);

                auto time = QTime(0, 0, 0);
                time = time.addSecs(qRound(route.value(QStringLiteral("duration")).toDouble()));
                qreal length = routeWaypoints->length(EARTH_RADIUS);
                const QString name = nameString("OSRM", length, time);
                const GeoDataExtendedData extendedData = routeData(length, time);
                routePlacemark->setExtendedData(extendedData);
                result->setName(name);
                result->append(routePlacemark);
            }
        }
    }

    /*
    QJsonValue routeInstructionsValue = data.value(QStringLiteral("route_instructions"));
    if (result && routeWaypoints && routeInstructionsValue.isArray()) {
        bool first = true;
        GeoDataPlacemark* instruction = new GeoDataPlacemark;
        int lastWaypointIndex = 0;

        const QJsonArray routeInstructionsArray = routeInstructionsValue.toArray();
        for (int index = 0; index < routeInstructionsArray.size(); ++index) {
            QVariantList details = routeInstructionsArray[index].toVariant().toList();
            if ( details.size() > 7 ) {
                QString const text = details.at( 0 ).toString();
                QString const road = details.at( 1 ).toString();
                int const waypointIndex = details.at( 3 ).toInt();

                if ( waypointIndex < routeWaypoints->size() ) {
                    const bool isLastInstruction = (index+1 >= routeInstructionsArray.size());
                    if (!isLastInstruction) {
                        GeoDataLineString *lineString = new GeoDataLineString;
                        for ( int i=lastWaypointIndex; i<=waypointIndex; ++i ) {
                            lineString->append(routeWaypoints->at( i ) );
                        }
                        instruction->setGeometry( lineString );
                        result->append( instruction );
                        instruction = new GeoDataPlacemark;
                    }
                    lastWaypointIndex = waypointIndex;
                    GeoDataExtendedData extendedData;
                    GeoDataData turnTypeData;
                    turnTypeData.setName(QStringLiteral("turnType"));
                    RoutingInstruction::TurnType turnType = parseTurnType( text );
                    turnTypeData.setValue( turnType );
                    extendedData.addValue( turnTypeData );
                    if (!road.isEmpty()) {
                        GeoDataData roadName;
                        roadName.setName(QStringLiteral("roadName"));
                        roadName.setValue( road );
                        extendedData.addValue( roadName );
                    }

                    if ( first ) {
                        turnType = RoutingInstruction::Continue;
                        first = false;
                    }

                    if ( turnType == RoutingInstruction::Unknown ) {
                        instruction->setName( text );
                    } else {
                        instruction->setName( RoutingInstruction::generateRoadInstruction( turnType, road ) );
                    }
                    instruction->setExtendedData( extendedData );

                    if (isLastInstruction && lastWaypointIndex > 0 ) {
                        GeoDataLineString *lineString = new GeoDataLineString;
                        for ( int i=lastWaypointIndex; i<waypointIndex; ++i ) {
                            lineString->append(routeWaypoints->at( i ) );
                        }
                        instruction->setGeometry( lineString );
                        result->append( instruction );
                    }
                }
            }
        }
    }
    */

    return result;
}

} // namespace Marble

#include "moc_OSRMRunner.cpp"
