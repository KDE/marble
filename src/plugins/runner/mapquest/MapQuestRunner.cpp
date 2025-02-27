// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "MapQuestRunner.h"

#include "GeoDataData.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "HttpDownloadManager.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "routing/Maneuver.h"
#include "routing/RouteRequest.h"

#include <QDomDocument>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QUrl>

#include <QUrlQuery>

namespace Marble
{

MapQuestRunner::MapQuestRunner(QObject *parent)
    : RoutingRunner(parent)
    , m_networkAccessManager()
    , m_request()
{
    connect(&m_networkAccessManager, &QNetworkAccessManager::finished, this, &MapQuestRunner::retrieveData);
}

MapQuestRunner::~MapQuestRunner()
{
    // nothing to do
}

void MapQuestRunner::retrieveRoute(const RouteRequest *route)
{
    if (route->size() < 2) {
        return;
    }

    QHash<QString, QVariant> settings = route->routingProfile().pluginSettings()[QStringLiteral("mapquest")];

    if (settings.value(QStringLiteral("appKey")).toString().isEmpty()) {
        return;
    }

    QString url = QStringLiteral(
        "http://open.mapquestapi.com/directions/v1/route?callback=renderAdvancedNarrative&outFormat=xml&narrativeType=text&shapeFormat=raw&generalize=0");
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    append(&url,
           QStringLiteral("from"),
           QString::number(route->source().latitude(degree), 'f', 6) + QLatin1Char(',') + QString::number(route->source().longitude(degree), 'f', 6));
    for (int i = 1; i < route->size(); ++i) {
        append(&url,
               QStringLiteral("to"),
               QString::number(route->at(i).latitude(degree), 'f', 6) + QLatin1Char(',') + QString::number(route->at(i).longitude(degree), 'f', 6));
    }

    QString const unit = MarbleGlobal::getInstance()->locale()->measurementSystem() == MarbleLocale::MetricSystem ? QStringLiteral("k") : QStringLiteral("m");
    append(&url, QStringLiteral("units"), unit);

    if (settings[QStringLiteral("noMotorways")].toInt()) {
        append(&url, QStringLiteral("avoids"), QStringLiteral("Limited Access"));
    }
    if (settings[QStringLiteral("noTollroads")].toInt()) {
        append(&url, QStringLiteral("avoids"), QStringLiteral("Toll road"));
    }
    if (settings[QStringLiteral("noFerries")].toInt()) {
        append(&url, QStringLiteral("avoids"), QStringLiteral("Ferry"));
    }

    if (!settings[QStringLiteral("preference")].toString().isEmpty()) {
        append(&url, QStringLiteral("routeType"), settings[QStringLiteral("preference")].toString());
    }

    const QString ascendingSetting = settings[QStringLiteral("ascending")].toString();
    const QString descendingSetting = settings[QStringLiteral("descending")].toString();
    if (!ascendingSetting.isEmpty() && !descendingSetting.isEmpty()) {
        if (ascendingSetting == QLatin1StringView("AVOID_UP_HILL") && descendingSetting == QLatin1StringView("AVOID_DOWN_HILL")) {
            append(&url, QStringLiteral("roadGradeStrategy"), QStringLiteral("AVOID_ALL_HILLS"));
        } else if (ascendingSetting == QLatin1StringView("FAVOR_UP_HILL") && descendingSetting == QLatin1StringView("FAVOR_DOWN_HILL")) {
            append(&url, QStringLiteral("roadGradeStrategy"), QStringLiteral("FAVOR_ALL_HILLS"));
        } else if (ascendingSetting == QLatin1StringView("DEFAULT_STRATEGY") && descendingSetting == QLatin1StringView("DEFAULT_STRATEGY")) {
            append(&url, QStringLiteral("roadGradeStrategy"), QStringLiteral("DEFAULT_STRATEGY"));
        } else if (ascendingSetting == QLatin1StringView("DEFAULT_STRATEGY")) {
            append(&url, QStringLiteral("roadGradeStrategy"), descendingSetting);
        } else if (descendingSetting == QLatin1StringView("DEFAULT_STRATEGY")) {
            append(&url, QStringLiteral("roadGradeStrategy"), ascendingSetting);
        } else if (descendingSetting == QLatin1StringView("AVOID_DOWN_HILL")) {
            append(&url, QStringLiteral("roadGradeStrategy"), descendingSetting);
        } else if (ascendingSetting == QLatin1StringView("AVOID_UP_HILL")) {
            append(&url, QStringLiteral("roadGradeStrategy"), ascendingSetting);
        }
    }

    QUrl qurl(url);
    // FIXME: verify that this works with special characters.
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QStringLiteral("key"), QString::fromLatin1(settings.value(QStringLiteral("appKey")).toByteArray()));
    qurl.setQuery(urlQuery);
    m_request.setUrl(qurl);
    m_request.setRawHeader("User-Agent", HttpDownloadManager::userAgent(QStringLiteral("Browser"), QStringLiteral("MapQuestRunner")));

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(15000);

    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    connect(this, &RoutingRunner::routeCalculated, &eventLoop, &QEventLoop::quit);

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot(0, this, SLOT(get()));
    timer.start();

    eventLoop.exec();
}

void MapQuestRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager.get(m_request);
    connect(reply, &QNetworkReply::errorOccurred, this, &MapQuestRunner::handleError, Qt::DirectConnection);
}

void MapQuestRunner::retrieveData(QNetworkReply *reply)
{
    if (reply->isFinished()) {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        // mDebug() << "Download completed: " << data;
        GeoDataDocument *document = parse(data);

        if (!document) {
            mDebug() << "Failed to parse the downloaded route data" << data;
        }

        Q_EMIT routeCalculated(document);
    }
}

void MapQuestRunner::handleError(QNetworkReply::NetworkError error)
{
    mDebug() << " Error when retrieving mapquest.org route: " << error;
}

void MapQuestRunner::append(QString *input, const QString &key, const QString &value)
{
    *input += QLatin1Char('&') + key + QLatin1Char('=') + value;
}

int MapQuestRunner::maneuverType(int mapQuestId)
{
    /** @todo FIXME: review 10, 11 */
    switch (mapQuestId) {
    case 0:
        return Maneuver::Straight; // straight
    case 1:
        return Maneuver::SlightRight; // slight right
    case 2:
        return Maneuver::Right; // right
    case 3:
        return Maneuver::SharpRight; // sharp right
    case 4:
        return Maneuver::TurnAround; // reverse
    case 5:
        return Maneuver::SharpLeft; // sharp left
    case 6:
        return Maneuver::Left; // left
    case 7:
        return Maneuver::SlightLeft; // slight left
    case 8:
        return Maneuver::TurnAround; // right u-turn
    case 9:
        return Maneuver::TurnAround; // left u-turn
    case 10:
        return Maneuver::Merge; // right merge
    case 11:
        return Maneuver::Merge; // left merge
    case 12:
        return Maneuver::Merge; // right on ramp
    case 13:
        return Maneuver::Merge; // left on ramp
    case 14:
        return Maneuver::ExitRight; // right off ramp
    case 15:
        return Maneuver::ExitLeft; // left off ramp
    case 16:
        return Maneuver::Right; // right fork
    case 17:
        return Maneuver::Left; // left fork
    case 18:
        return Maneuver::Continue; // straight fork
    }

    return Maneuver::Unknown;
}

GeoDataDocument *MapQuestRunner::parse(const QByteArray &content) const
{
    QDomDocument xml;
    if (!xml.setContent(content)) {
        mDebug() << "Cannot parse xml file with routing instructions.";
        return nullptr;
    }

    // mDebug() << xml.toString(2);
    QDomElement root = xml.documentElement();

    auto result = new GeoDataDocument();
    result->setName(QStringLiteral("MapQuest"));
    auto routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName(QStringLiteral("Route"));

    auto routeWaypoints = new GeoDataLineString;
    QDomNodeList shapePoints = root.elementsByTagName(QStringLiteral("shapePoints"));
    if (shapePoints.size() == 1) {
        QDomNodeList geometry = shapePoints.at(0).toElement().elementsByTagName(QStringLiteral("latLng"));
        for (int i = 0; i < geometry.size(); ++i) {
            double const lat = geometry.item(i).namedItem(QStringLiteral("lat")).toElement().text().toDouble();
            double const lon = geometry.item(i).namedItem(QStringLiteral("lng")).toElement().text().toDouble();
            GeoDataCoordinates const position(lon, lat, 0.0, GeoDataCoordinates::Degree);
            routeWaypoints->append(position);
        }
    }
    routePlacemark->setGeometry(routeWaypoints);

    QTime time;
    time = time.addSecs(root.elementsByTagName(QStringLiteral("time")).at(0).toElement().text().toInt());
    qreal length = routeWaypoints->length(EARTH_RADIUS);
    const QString name = nameString(QStringLiteral("MQ"), length, time);
    const GeoDataExtendedData data = routeData(length, time);
    routePlacemark->setExtendedData(data);
    result->setName(name);
    result->append(routePlacemark);

    QMap<int, int> mapping;
    QDomNodeList maneuvers = root.elementsByTagName(QStringLiteral("maneuverIndexes"));
    if (maneuvers.size() == 1) {
        maneuvers = maneuvers.at(0).childNodes();
        for (int i = 0; i < maneuvers.size(); ++i) {
            mapping[i] = maneuvers.at(i).toElement().text().toInt();
            if (mapping[i] == routeWaypoints->size()) {
                --mapping[i];
            }
        }
    }

    QDomNodeList instructions = root.elementsByTagName(QStringLiteral("maneuver"));
    unsigned int const lastInstruction = qMax<int>(0, instructions.length() - 1); // ignore the last 'Welcome to xy' instruction
    for (unsigned int i = 0; i < lastInstruction; ++i) {
        QDomElement node = instructions.item(i).toElement();

        QDomNodeList maneuver = node.elementsByTagName(QStringLiteral("turnType"));
        QDomNodeList textNodes = node.elementsByTagName(QStringLiteral("narrative"));
        QDomNodeList points = node.elementsByTagName(QStringLiteral("startPoint"));
        QDomNodeList streets = node.elementsByTagName(QStringLiteral("streets"));

        Q_ASSERT(mapping.contains(i));
        if (textNodes.size() == 1 && maneuver.size() == 1 && points.size() == 1 && mapping.contains(i)) {
            auto instruction = new GeoDataPlacemark;
            instruction->setName(textNodes.at(0).toElement().text());

            GeoDataExtendedData extendedData;
            GeoDataData turnType;
            turnType.setName(QStringLiteral("turnType"));
            turnType.setValue(maneuverType(maneuver.at(0).toElement().text().toInt()));
            extendedData.addValue(turnType);
            if (streets.size() == 1) {
                GeoDataData roadName;
                roadName.setName(QStringLiteral("roadName"));
                roadName.setValue(streets.at(0).toElement().text());
                extendedData.addValue(roadName);
            }
            instruction->setExtendedData(extendedData);

            int const start = mapping[i];
            int const end = mapping.contains(i + 1) ? mapping[i + 1] : routeWaypoints->size() - 1;
            if (start >= 0 && start < routeWaypoints->size() && end < routeWaypoints->size()) {
                instruction->setName(textNodes.item(0).toElement().text());
                auto lineString = new GeoDataLineString;
                for (int j = start; j <= end; ++j) {
                    *lineString << GeoDataCoordinates(routeWaypoints->at(j).longitude(), routeWaypoints->at(j).latitude());
                }

                if (!lineString->isEmpty()) {
                    instruction->setGeometry(lineString);
                    result->append(instruction);
                }
            }
        }
    }

    if (routeWaypoints->isEmpty()) {
        delete result;
        result = nullptr;
    }

    return result;
}

} // namespace Marble

#include "moc_MapQuestRunner.cpp"
