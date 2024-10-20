// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GosmoreRoutingRunner.h"

#include "GeoDataData.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteRequest.h"
#include "routing/instructions/InstructionTransformation.h"
#include "routing/instructions/WaypointParser.h"

#include <QMap>
#include <QProcess>

namespace Marble
{

class GosmoreRunnerPrivate
{
public:
    QFileInfo m_gosmoreMapFile;

    WaypointParser m_parser;

    /** Static to share the cache among all instances */
    static QMap<QString, QByteArray> m_partialRoutes;

    QByteArray retrieveWaypoints(const QString &query) const;

    static GeoDataDocument *createDocument(GeoDataLineString *routeWaypoints, const QList<GeoDataPlacemark *> instructions);

    static GeoDataLineString parseGosmoreOutput(const QByteArray &content);

    static void merge(GeoDataLineString *one, const GeoDataLineString &two);

    QList<GeoDataPlacemark *> parseGosmoreInstructions(const QByteArray &content);

    GosmoreRunnerPrivate();
};

GosmoreRunnerPrivate::GosmoreRunnerPrivate()
{
    m_parser.setLineSeparator("\r");
    m_parser.setFieldSeparator(QLatin1Char(','));
    m_parser.setFieldIndex(WaypointParser::RoadName, 4);
    m_parser.addJunctionTypeMapping("Jr", RoutingWaypoint::Roundabout);
}

QMap<QString, QByteArray> GosmoreRunnerPrivate::m_partialRoutes;

void GosmoreRunnerPrivate::merge(GeoDataLineString *one, const GeoDataLineString &two)
{
    Q_ASSERT(one);

    QList<GeoDataCoordinates>::const_iterator iter = two.constBegin();
    for (; iter != two.constEnd(); ++iter) {
        /** @todo: It might be needed to cut off some points at the start or end */
        one->append(*iter);
    }
}

QByteArray GosmoreRunnerPrivate::retrieveWaypoints(const QString &query) const
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QUERY_STRING", query);
    env.insert("LC_ALL", "C");
    QProcess gosmore;
    gosmore.setProcessEnvironment(env);

    gosmore.start("gosmore", QStringList() << m_gosmoreMapFile.absoluteFilePath());
    if (!gosmore.waitForStarted(5000)) {
        mDebug() << "Couldn't start gosmore from the current PATH. Install it to retrieve routing results from gosmore.";
        return {};
    }

    if (gosmore.waitForFinished(15000)) {
        return gosmore.readAllStandardOutput();
    } else {
        mDebug() << "Couldn't stop gosmore";
    }

    return {};
}

GeoDataLineString GosmoreRunnerPrivate::parseGosmoreOutput(const QByteArray &content)
{
    GeoDataLineString routeWaypoints;

    QStringList lines = QString::fromLocal8Bit(content).split(QLatin1Char('\r'));
    for (const QString &line : std::as_const(lines)) {
        const QStringList fields = line.split(QLatin1Char(','));
        if (fields.size() >= 5) {
            qreal lon = fields.at(1).toDouble();
            qreal lat = fields.at(0).toDouble();
            GeoDataCoordinates coordinates(lon, lat, 0.0, GeoDataCoordinates::Degree);
            routeWaypoints.append(coordinates);
        }
    }

    return routeWaypoints;
}

QList<GeoDataPlacemark *> GosmoreRunnerPrivate::parseGosmoreInstructions(const QByteArray &content)
{
    // Determine gosmore version
    QStringList lines = QString::fromUtf8(content).split(QLatin1Char('\r'));
    if (lines.size() > 2) {
        const QStringList fields = lines.at(lines.size() - 2).split(QLatin1Char(','));
        m_parser.setFieldIndex(WaypointParser::RoadName, fields.size() - 1);
        if (fields.size() < 5 || fields.size() > 6) {
            // Can happen when gosmore changes the output format, returns garbage
            // or the last street name contains a comma. We may still parse it correctly, just try.
            mDebug() << "Unexpected number of fields. This gosmore version may be unsupported.";
        }
    }

    QList<GeoDataPlacemark *> result;
    QTextStream stream(content);
    stream.setAutoDetectUnicode(true);

    RoutingInstructions directions = InstructionTransformation::process(m_parser.parse(stream));
    for (int i = 0; i < directions.size(); ++i) {
        auto placemark = new GeoDataPlacemark(directions[i].instructionText());
        GeoDataExtendedData extendedData;
        GeoDataData turnType;
        turnType.setName(QStringLiteral("turnType"));
        turnType.setValue(QVariant::fromValue(int(directions[i].turnType())));
        extendedData.addValue(turnType);
        GeoDataData roadName;
        roadName.setName(QStringLiteral("roadName"));
        roadName.setValue(directions[i].roadName());
        extendedData.addValue(roadName);
        placemark->setExtendedData(extendedData);
        Q_ASSERT(!directions[i].points().isEmpty());
        auto geometry = new GeoDataLineString;
        QList<RoutingWaypoint> items = directions[i].points();
        for (int j = 0; j < items.size(); ++j) {
            RoutingPoint point = items[j].point();
            GeoDataCoordinates coordinates(point.lon(), point.lat(), 0.0, GeoDataCoordinates::Degree);
            geometry->append(coordinates);
        }
        placemark->setGeometry(geometry);
        result.push_back(placemark);
    }

    return result;
}

GeoDataDocument *GosmoreRunnerPrivate::createDocument(GeoDataLineString *routeWaypoints, const QList<GeoDataPlacemark *> instructions)
{
    if (!routeWaypoints || routeWaypoints->isEmpty()) {
        return nullptr;
    }

    auto result = new GeoDataDocument();
    auto routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName(QStringLiteral("Route"));
    routePlacemark->setGeometry(routeWaypoints);
    result->append(routePlacemark);

    QString name = QStringLiteral("%1 %2 (Gosmore)");
    QString unit = QLatin1StringView("m");
    qreal length = routeWaypoints->length(EARTH_RADIUS);
    if (length >= 1000) {
        length /= 1000.0;
        unit = QStringLiteral("km");
    }
    result->setName(name.arg(length, 0, 'f', 1).arg(unit));

    for (GeoDataPlacemark *placemark : instructions) {
        result->append(placemark);
    }

    return result;
}

GosmoreRunner::GosmoreRunner(QObject *parent)
    : RoutingRunner(parent)
    , d(new GosmoreRunnerPrivate)
{
    // Check installation
    QDir mapDir(MarbleDirs::localPath() + QLatin1StringView("/maps/earth/gosmore/"));
    d->m_gosmoreMapFile = QFileInfo(mapDir, QStringLiteral("gosmore.pak"));
}

GosmoreRunner::~GosmoreRunner()
{
    delete d;
}

void GosmoreRunner::retrieveRoute(const RouteRequest *route)
{
    if (!d->m_gosmoreMapFile.exists()) {
        Q_EMIT routeCalculated(nullptr);
        return;
    }

    auto wayPoints = new GeoDataLineString;
    QByteArray completeOutput;

    for (int i = 0; i < route->size() - 1; ++i) {
        QString queryString = QStringLiteral("flat=%1&flon=%2&tlat=%3&tlon=%4&fastest=1&v=motorcar");
        GeoDataCoordinates source = route->at(i);
        double fLon = source.longitude(GeoDataCoordinates::Degree);
        double fLat = source.latitude(GeoDataCoordinates::Degree);
        queryString = queryString.arg(fLat, 0, 'f', 8).arg(fLon, 0, 'f', 8);
        GeoDataCoordinates destination = route->at(i + 1);
        double tLon = destination.longitude(GeoDataCoordinates::Degree);
        double tLat = destination.latitude(GeoDataCoordinates::Degree);
        queryString = queryString.arg(tLat, 0, 'f', 8).arg(tLon, 0, 'f', 8);

        QByteArray output;
        if (d->m_partialRoutes.contains(queryString)) {
            output = d->m_partialRoutes[queryString];
        } else {
            output = d->retrieveWaypoints(queryString);
        }

        GeoDataLineString points = d->parseGosmoreOutput(output);
        d->merge(wayPoints, points);
        completeOutput.append(output);
    }

    QList<GeoDataPlacemark *> instructions = d->parseGosmoreInstructions(completeOutput);

    GeoDataDocument *result = d->createDocument(wayPoints, instructions);
    Q_EMIT routeCalculated(result);
}

} // namespace Marble
