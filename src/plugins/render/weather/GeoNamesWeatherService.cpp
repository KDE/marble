// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
//

#include "GeoNamesWeatherService.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoNamesWeatherItem.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

#include <QUrlQuery>

using namespace Marble;

QHash<QString, WeatherData::WeatherCondition> GeoNamesWeatherService::dayConditions = QHash<QString, WeatherData::WeatherCondition>();
QList<WeatherData::WindDirection> GeoNamesWeatherService::windDirections = QList<WeatherData::WindDirection>(16);

GeoNamesWeatherService::GeoNamesWeatherService(const MarbleModel *model, QObject *parent)
    : AbstractWeatherService(model, parent)
{
    GeoNamesWeatherService::setupHashes();
}

GeoNamesWeatherService::~GeoNamesWeatherService() = default;

void GeoNamesWeatherService::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    if (marbleModel()->planetId() != QLatin1StringView("earth")) {
        return;
    }

    QUrl geonamesUrl(QStringLiteral("http://api.geonames.org/weatherJSON"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QStringLiteral("north"), QString::number(box.north(GeoDataCoordinates::Degree)));
    urlQuery.addQueryItem(QStringLiteral("south"), QString::number(box.south(GeoDataCoordinates::Degree)));
    urlQuery.addQueryItem(QStringLiteral("east"), QString::number(box.east(GeoDataCoordinates::Degree)));
    urlQuery.addQueryItem(QStringLiteral("west"), QString::number(box.west(GeoDataCoordinates::Degree)));
    urlQuery.addQueryItem(QStringLiteral("maxRows"), QString::number(number));
    urlQuery.addQueryItem(QStringLiteral("username"), QStringLiteral("marble"));
    geonamesUrl.setQuery(urlQuery);

    Q_EMIT downloadDescriptionFileRequested(geonamesUrl);
}

void GeoNamesWeatherService::getItem(const QString &id)
{
    if (marbleModel()->planetId() != QLatin1StringView("earth")) {
        return;
    }

    if (id.startsWith(QLatin1StringView("geonames_"))) {
        QUrl geonamesUrl(QStringLiteral("http://api.geonames.org/weatherIcaoJSON"));
        QUrlQuery urlQuery;
        urlQuery.addQueryItem(QStringLiteral("ICAO"), id.mid(9));
        urlQuery.addQueryItem(QStringLiteral("username"), QStringLiteral("marble"));
        geonamesUrl.setQuery(urlQuery);
        Q_EMIT downloadDescriptionFileRequested(geonamesUrl);
    }
}

void GeoNamesWeatherService::parseFile(const QByteArray &file)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file);
    QJsonValue weatherObservationsValue = jsonDoc.object().value(QStringLiteral("weatherObservations"));

    // Parse if any result exists
    QList<AbstractDataPluginItem *> items;
    if (weatherObservationsValue.isArray()) {
        // Add items to the list
        QJsonArray weatherObservationsArray = weatherObservationsValue.toArray();
        for (int index = 0; index < weatherObservationsArray.size(); ++index) {
            QJsonObject weatherObservationObject = weatherObservationsArray[index].toObject();

            AbstractDataPluginItem *item = parse(weatherObservationObject);
            if (item) {
                items << item;
            }
        }
    } else {
        QJsonValue weatherObservationValue = jsonDoc.object().value(QStringLiteral("weatherObservation"));
        QJsonObject weatherObservationObject = weatherObservationValue.toObject();
        AbstractDataPluginItem *item = parse(weatherObservationObject);
        if (item) {
            items << item;
        }
    }

    Q_EMIT createdItems(items);
}

AbstractDataPluginItem *GeoNamesWeatherService::parse(const QJsonObject &weatherObservationObject)
{
    const QString condition = weatherObservationObject.value(QStringLiteral("weatherCondition")).toString();
    const QString clouds = weatherObservationObject.value(QStringLiteral("clouds")).toString();
    const int windDirection = weatherObservationObject.value(QStringLiteral("windDirection")).toInt();
    QString id = weatherObservationObject.value(QStringLiteral("ICAO")).toString();
    const double temperature = weatherObservationObject.value(QStringLiteral("temperature")).toString().toDouble(); // delivered as string
    const int windSpeed = weatherObservationObject.value(QStringLiteral("windSpeed")).toString().toInt(); // delivered as string
    const int humidity = weatherObservationObject.value(QStringLiteral("humidity")).toInt();
    const double pressure = weatherObservationObject.value(QStringLiteral("seaLevelPressure")).toDouble();
    const QString name = weatherObservationObject.value(QStringLiteral("stationName")).toString();
    const QDateTime date = QDateTime::fromString(weatherObservationObject.value(QStringLiteral("datetime")).toString(), QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    const double longitude = weatherObservationObject.value(QStringLiteral("lng")).toDouble();
    const double latitude = weatherObservationObject.value(QStringLiteral("lat")).toDouble();

    if (!id.isEmpty()) {
        WeatherData data;

        // Weather condition
        if (clouds != QLatin1StringView("n/a") && condition != QLatin1StringView("n/a")) {
            if (dayConditions.contains(condition)) {
                data.setCondition(dayConditions[condition]);
            } else {
                mDebug() << "UNHANDLED GEONAMES WEATHER CONDITION, PLEASE REPORT: " << condition;
            }
        } else {
            if (dayConditions.contains(clouds)) {
                data.setCondition(dayConditions[clouds]);
            } else {
                mDebug() << "UNHANDLED GEONAMES CLOUDS CONDITION, PLEASE REPORT: " << clouds;
            }
        }

        // Wind direction. Finds the closest direction from windDirections array.
        if (windDirection >= 0) {
            double tickSpacing = 360.0 / windDirections.size();
            data.setWindDirection(windDirections[int((windDirection / tickSpacing) + 0.5) % windDirections.size()]);
        }

        // Wind speed
        if (windSpeed != 0) {
            data.setWindSpeed(windSpeed, WeatherData::knots);
        }

        // Temperature
        data.setTemperature(temperature, WeatherData::Celsius);

        // Humidity
        data.setHumidity(humidity);

        // Pressure
        if (pressure != 0.0) {
            data.setPressure(pressure, WeatherData::HectoPascal);
        }

        // Date
        data.setDataDate(date.date());
        data.setPublishingTime(date);

        // ID
        id = QLatin1StringView("geonames_") + id;

        GeoDataCoordinates coordinates(longitude, latitude, 0.0, GeoDataCoordinates::Degree);
        auto item = new GeoNamesWeatherItem(this);
        item->setMarbleWidget(marbleWidget());
        item->setId(id);
        item->setCoordinate(coordinates);
        item->setPriority(0);
        item->setStationName(name);
        item->setCurrentWeather(data);
        return item;
    } else {
        return nullptr;
    }
}

void GeoNamesWeatherService::setupHashes()
{
    if (!((dayConditions.isEmpty()) || (windDirections.isEmpty()))) {
        return;
    }

    dayConditions[QStringLiteral("sunny")] = WeatherData::ClearDay;
    dayConditions[QStringLiteral("clear")] = WeatherData::ClearDay;
    dayConditions[QStringLiteral("clear sky")] = WeatherData::ClearDay;
    dayConditions[QStringLiteral("sunny intervals")] = WeatherData::FewCloudsDay;
    dayConditions[QStringLiteral("few clouds")] = WeatherData::FewCloudsDay;
    dayConditions[QStringLiteral("scattered clouds")] = WeatherData::FewCloudsDay;
    dayConditions[QStringLiteral("partly cloudy")] = WeatherData::PartlyCloudyDay;
    dayConditions[QStringLiteral("broken clouds")] = WeatherData::PartlyCloudyDay;
    dayConditions[QStringLiteral("white cloud")] = WeatherData::Overcast;
    dayConditions[QStringLiteral("overcast")] = WeatherData::Overcast;
    dayConditions[QStringLiteral("grey cloud")] = WeatherData::Overcast;
    dayConditions[QStringLiteral("cloudy")] = WeatherData::Overcast;
    dayConditions[QStringLiteral("drizzle")] = WeatherData::LightRain;
    dayConditions[QStringLiteral("light drizzle")] = WeatherData::LightRain;
    dayConditions[QStringLiteral("misty")] = WeatherData::Mist;
    dayConditions[QStringLiteral("mist")] = WeatherData::Mist;
    dayConditions[QStringLiteral("fog")] = WeatherData::Mist;
    dayConditions[QStringLiteral("foggy")] = WeatherData::Mist;
    dayConditions[QStringLiteral("dense fog")] = WeatherData::Mist;
    dayConditions[QStringLiteral("Thick Fog")] = WeatherData::Mist;
    dayConditions[QStringLiteral("tropical storm")] = WeatherData::Thunderstorm;
    dayConditions[QStringLiteral("thunderstorm")] = WeatherData::Thunderstorm;
    dayConditions[QStringLiteral("hazy")] = WeatherData::Mist;
    dayConditions[QStringLiteral("haze")] = WeatherData::Mist;
    dayConditions[QStringLiteral("in vicinity:  showers ")] = WeatherData::ShowersDay;
    dayConditions[QStringLiteral("light shower")] = WeatherData::LightShowersDay;
    dayConditions[QStringLiteral("light rain shower")] = WeatherData::LightShowersDay;
    dayConditions[QStringLiteral("light showers")] = WeatherData::LightShowersDay;
    dayConditions[QStringLiteral("light rain")] = WeatherData::ShowersDay;
    dayConditions[QStringLiteral("heavy rain")] = WeatherData::Rain;
    dayConditions[QStringLiteral("heavy showers")] = WeatherData::Rain;
    dayConditions[QStringLiteral("heavy shower")] = WeatherData::Rain;
    dayConditions[QStringLiteral("heavy rain shower")] = WeatherData::Rain;
    dayConditions[QStringLiteral("thundery shower")] = WeatherData::Thunderstorm;
    dayConditions[QStringLiteral("thunderstorm")] = WeatherData::Thunderstorm;
    dayConditions[QStringLiteral("thunder storm")] = WeatherData::Thunderstorm;
    dayConditions[QStringLiteral("cloudy with sleet")] = WeatherData::RainSnow;
    dayConditions[QStringLiteral("sleet shower")] = WeatherData::RainSnow;
    dayConditions[QStringLiteral("sleet showers")] = WeatherData::RainSnow;
    dayConditions[QStringLiteral("sleet")] = WeatherData::RainSnow;
    dayConditions[QStringLiteral("cloudy with hail")] = WeatherData::Hail;
    dayConditions[QStringLiteral("hail shower")] = WeatherData::Hail;
    dayConditions[QStringLiteral("hail showers")] = WeatherData::Hail;
    dayConditions[QStringLiteral("hail")] = WeatherData::Hail;
    dayConditions[QStringLiteral("light snow")] = WeatherData::LightSnow;
    dayConditions[QStringLiteral("light snow shower")] = WeatherData::ChanceSnowDay;
    dayConditions[QStringLiteral("light snow showers")] = WeatherData::ChanceSnowDay;
    dayConditions[QStringLiteral("cloudy with light snow")] = WeatherData::LightSnow;
    dayConditions[QStringLiteral("heavy snow")] = WeatherData::Snow;
    dayConditions[QStringLiteral("heavy snow shower")] = WeatherData::Snow;
    dayConditions[QStringLiteral("heavy snow showers")] = WeatherData::Snow;
    dayConditions[QStringLiteral("cloudy with heavy snow")] = WeatherData::Snow;
    dayConditions[QStringLiteral("sandstorm")] = WeatherData::SandStorm;
    dayConditions[QStringLiteral("na")] = WeatherData::ConditionNotAvailable;
    dayConditions[QStringLiteral("n/a")] = WeatherData::ConditionNotAvailable;

    windDirections[0] = WeatherData::N;
    windDirections[1] = WeatherData::NNE;
    windDirections[2] = WeatherData::NE;
    windDirections[3] = WeatherData::ENE;
    windDirections[4] = WeatherData::E;
    windDirections[5] = WeatherData::ESE;
    windDirections[6] = WeatherData::SE;
    windDirections[7] = WeatherData::SSE;
    windDirections[8] = WeatherData::S;
    windDirections[9] = WeatherData::SSW;
    windDirections[10] = WeatherData::SW;
    windDirections[11] = WeatherData::WSW;
    windDirections[12] = WeatherData::W;
    windDirections[13] = WeatherData::WNW;
    windDirections[14] = WeatherData::NW;
    windDirections[15] = WeatherData::NNW;
}

#include "moc_GeoNamesWeatherService.cpp"
