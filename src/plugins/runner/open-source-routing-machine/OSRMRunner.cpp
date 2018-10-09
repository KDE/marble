//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include "OSRMRunner.h"

#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataLineString.h"
#include "routing/Maneuver.h"
#include "routing/RouteRequest.h"
#include "HttpDownloadManager.h"

#include <QString>
#include <QVector>
#include <QUrl>
#include <QTime>
#include <QTimer>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace Marble
{

	OSRMRunner::OSRMRunner(QObject *parent) :
		RoutingRunner(parent),
		m_networkAccessManager()
	{
		connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(retrieveData(QNetworkReply*)));
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
		// Use HTTPS!
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

		url += QStringLiteral("?alternatives=false&steps=true&overview=full&geometries=polyline6");

		m_request = QNetworkRequest(QUrl(url));
		m_request.setRawHeader("User-Agent", HttpDownloadManager::userAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:62.0) Gecko/20100101 Firefox/62.0", ""));

		QEventLoop eventLoop;

		QTimer timer;
		timer.setSingleShot(true);
		timer.setInterval(15000);

		connect(&timer, SIGNAL(timeout()),
			&eventLoop, SLOT(quit()));
		connect(this, SIGNAL(routeCalculated(GeoDataDocument*)),
			&eventLoop, SLOT(quit()));

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
			GeoDataDocument* document = parse(data);

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
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(handleError(QNetworkReply::NetworkError)), Qt::DirectConnection);
	}

	void OSRMRunner::append(QString *input, const QString &key, const QString &value)
	{
		*input += QLatin1Char('&') + key + QLatin1Char('=') + value;
	}

	GeoDataLineString *OSRMRunner::decodePolyline(const QString &geometry)
	{
		// See https://developers.google.com/maps/documentation/utilities/polylinealgorithm
		GeoDataLineString* lineString = new GeoDataLineString;
		int coordinates[2] = { 0, 0 };
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
			lineString->append(GeoDataCoordinates(double(coordinates[1]) / 1E6,
				double(coordinates[0]) / 1E6,
				0.0, GeoDataCoordinates::Degree));
		}
		return lineString;
	}

	RoutingInstruction::TurnType OSRMRunner::parseTurnType(const QString &instruction, const QString &modifier)
	{
		if (instruction == QLatin1String("turn")) {
			if (modifier == QLatin1String("left")) {
				return RoutingInstruction::Left;
			}
			else if (modifier == QLatin1String("right")) {
				return RoutingInstruction::Right;
			}
			else if (modifier == QLatin1String("sharp left")) {
				return RoutingInstruction::SharpLeft;
			}
			else if (modifier == QLatin1String("sharp right")) {
				return RoutingInstruction::SharpRight;
			}
			else if (modifier == QLatin1String("uturn")) {
				return RoutingInstruction::TurnAround;
			}
			else if (modifier == QLatin1String("slight right")) {
				return RoutingInstruction::SlightRight;
			}
			else if (modifier == QLatin1String("slight left")) {
				return RoutingInstruction::SlightLeft;
			}
			else if (modifier == QLatin1String("straight")) {
				return RoutingInstruction::Straight;
			}
		}
		else if (instruction == QLatin1String("merge")) {
			return RoutingInstruction::Merge;
		}
		else if (instruction == QLatin1String("fork")) {
			if (modifier == QLatin1String("left")) {
				return RoutingInstruction::ExitLeft;
			}
			else if (modifier == QLatin1String("left")) {
				return RoutingInstruction::ExitRight;
			}
		}
		else if (instruction == QLatin1String("end of road")) {
			if (modifier == QLatin1String("left")) {
				return RoutingInstruction::Left;
			}
			else if (modifier == QLatin1String("right")) {
				return RoutingInstruction::Right;
			}
		}
		else if (instruction == QLatin1String("exit roundabout")) {
			if (modifier == QLatin1String("left")) {
				return RoutingInstruction::RoundaboutThirdExit;
			}
			else if (modifier == QLatin1String("right")) {
				return RoutingInstruction::RoundaboutFirstExit;
			}
			else if (modifier == QLatin1String("straight")) {
				return RoutingInstruction::RoundaboutSecondExit;
			}
			else {
				return RoutingInstruction::RoundaboutExit;
			}
		}
		else if (instruction == QLatin1String("exit rotary")) {
			return RoutingInstruction::RoundaboutExit;
		}
		/*
		else if (instruction == QLatin1String("arrive") | instruction == QLatin1String("depart")) {
			if (modifier == QLatin1String("left")) {
				return RoutingInstruction::Left;
			}
			else if (modifier == QLatin1String("right")) {
				return RoutingInstruction::Right;
			}
		}
		*/
		else if (instruction == QLatin1String("continue")) {
			return RoutingInstruction::Continue;
		}
		return RoutingInstruction::Unknown;
	}

	GeoDataDocument *OSRMRunner::parse(const QByteArray &input) const
	{
		QJsonDocument jsonDoc = QJsonDocument::fromJson(input);
		QJsonObject data = jsonDoc.object();

		GeoDataDocument* result = nullptr;
		GeoDataLineString* routeWaypoints = nullptr;
		QJsonValue routeGeometryValue = data.value(QStringLiteral("routes"));

		QJsonArray wayPoints = data.value(QStringLiteral("waypoints")).toArray();

		if (routeGeometryValue.isArray()) {
			auto routes = routeGeometryValue.toArray();
			if (!routes.isEmpty()) {
				auto route = routes[0].toObject();
				auto routeGeometryValue = route.value(QStringLiteral("geometry"));
				if (routeGeometryValue.isString()) {
					result = new GeoDataDocument();
					result->setName(QStringLiteral("Open Source Routing Machine"));

					GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
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

				QJsonValue routeInstructionsValue = route.value(QStringLiteral("legs"));
				if (result && routeWaypoints && routeInstructionsValue.isArray()) {
					GeoDataPlacemark* instruction = new GeoDataPlacemark;
					int lastWaypointIndex = 0;
					int totalLegs = routeInstructionsValue.toArray().size();
					bool first = true;
					QJsonArray routeInstructionsArray = QJsonArray();

					for (int legIndex = 0; legIndex < routeInstructionsValue.toArray().size(); legIndex++) {
						// Possible fix, merge the two arrays into one
						int instructions = routeInstructionsValue.toArray()[legIndex].toObject().value(QStringLiteral("steps")).toArray().size();
						for (int instruction = 0; instruction < instructions; instruction++) {
							routeInstructionsArray.append(routeInstructionsValue.toArray()[legIndex].toObject().value(QStringLiteral("steps")).toArray()[instruction]);
						}
					}
					int currentLeg = 0;
					int geometrySize = routeWaypoints->size();
					int stepSize = routeInstructionsArray.size();
					for (int index = 0; index < stepSize; ++index) {

						//auto details = first ? wayPoints[currentLeg].toObject() : routeInstructionsArray[index].toObject();
						auto details = routeInstructionsArray[index].toObject();
						GeoDataLineString* stepGeometry = decodePolyline(details.value(QStringLiteral("geometry")).toString());

						QString const road = details.value(QStringLiteral("name")).toString();
						int const waypointIndex = index;
						int stepGeometrySize = stepGeometry->size();

						auto maneuver = details.value(QStringLiteral("maneuver")).toObject();
						const bool isLastInstruction = (index + 1 >= stepSize);

						lastWaypointIndex++;
						GeoDataExtendedData extendedData;
						GeoDataData turnTypeData;
						turnTypeData.setName(QStringLiteral("turnType"));
						QString type = maneuver.value(QStringLiteral("type")).toString();
						QString modifier = maneuver.value(QStringLiteral("modifier")).toString();
						RoutingInstruction::TurnType turnType = parseTurnType(type, modifier);
						turnTypeData.setValue(turnType);
						extendedData.addValue(turnTypeData);
						if (!road.isEmpty()) {
							GeoDataData roadName;
							roadName.setName(QStringLiteral("roadName"));
							roadName.setValue(road);
							extendedData.addValue(roadName);
						}

						if (first) {
							turnType = RoutingInstruction::Continue;
							first = false;
						}

						if (turnType == RoutingInstruction::Unknown) {
							instruction->setName(type + modifier);
						}
						else {
							instruction->setName(RoutingInstruction::generateRoadInstruction(turnType, road));
						}
						instruction->setExtendedData(extendedData);


						if (!isLastInstruction) {
							GeoDataLineString *lineString = new GeoDataLineString;

							if (first) {
								/*double lat = wayPoints[0].toObject().value(QStringLiteral("location"))[0].toDouble();
								double lon = wayPoints[0].toObject().value(QStringLiteral("location"))[1].toDouble();
								instruction->setCoordinate(lat,
									lon, GeoDataCoordinates::Degree);*/
								double lat = maneuver.value(QStringLiteral("location")).toArray()[0].toDouble(), lon = maneuver.value(QStringLiteral("location")).toArray()[1].toDouble();
								instruction->setCoordinate(lat,
									lon, GeoDataCoordinates::Degree);
							}
							else {
								double lat = maneuver.value(QStringLiteral("location")).toArray()[0].toDouble(), lon = maneuver.value(QStringLiteral("location")).toArray()[1].toDouble();
								instruction->setCoordinate(lat,
									lon, GeoDataCoordinates::Degree);

							}
							for (int i = 0; i < stepGeometrySize; i++) {
								lineString->append(stepGeometry->at(i)); // TODO TEST
							}
							instruction->setGeometry(lineString);
							result->append(instruction);
							instruction = new GeoDataPlacemark;

						}


						if (isLastInstruction && lastWaypointIndex > 0) {
							bool shouldReturn = false;
							if (shouldReturn) {
								return result;
							}
							GeoDataLineString *lineString = new GeoDataLineString;
							for (int i = 0; i < stepGeometrySize; i++) {
								lineString->append(stepGeometry->at(i)); // TODO TEST
							}
							/*
							for (int i = lastWaypointIndex; i <= lastWaypointIndex + waypointIndex; ++i) {
								lineString->append(routeWaypoints->at(i));
							}
							*/
							instruction->setGeometry(lineString);
							result->append(instruction);
						}

						/*
						if (currentLeg < routeInstructionsValue.toArray().size() && index + 1 == routeInstructionsValue.toArray()[currentLeg].toObject().value(QStringLiteral("steps")).toArray().size()) {
							currentLeg++;
							first = true;
						}
						*/


					}
				}
			}
		}

		/*

		*/

		return result;
	}

} // namespace Marble

#include "moc_OSRMRunner.cpp"
