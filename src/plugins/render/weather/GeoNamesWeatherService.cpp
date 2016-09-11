//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#include "GeoNamesWeatherService.h"

#include "WeatherData.h"
#include "GeoNamesWeatherItem.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"

#include <QUrl>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QUrlQuery>

using namespace Marble;

QHash<QString, WeatherData::WeatherCondition> GeoNamesWeatherService::dayConditions
        = QHash<QString, WeatherData::WeatherCondition>();
QVector<WeatherData::WindDirection> GeoNamesWeatherService::windDirections
        = QVector<WeatherData::WindDirection>(16);

GeoNamesWeatherService::GeoNamesWeatherService( const MarbleModel *model, QObject *parent ) :
    AbstractWeatherService( model, parent )
{
    GeoNamesWeatherService::setupHashes();
}

GeoNamesWeatherService::~GeoNamesWeatherService()
{
}

void GeoNamesWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                            qint32 number )
{
    if (marbleModel()->planetId() != QLatin1String("earth")) {
        return;
    }

    QUrl geonamesUrl( "http://ws.geonames.org/weatherJSON" );
    QUrlQuery urlQuery;
    urlQuery.addQueryItem( "north", QString::number( box.north( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "south", QString::number( box.south( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "east", QString::number( box.east( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "west", QString::number( box.west( GeoDataCoordinates::Degree ) ) );
    urlQuery.addQueryItem( "maxRows", QString::number( number ) );
    urlQuery.addQueryItem( "username", "marble" );
    geonamesUrl.setQuery( urlQuery );

    emit downloadDescriptionFileRequested( geonamesUrl );
}

void GeoNamesWeatherService::getItem( const QString &id )
{
    if (marbleModel()->planetId() != QLatin1String("earth")) {
        return;
    }

    if ( id.startsWith(QLatin1String("geonames_") ) ) {
        QUrl geonamesUrl( "http://ws.geonames.org/weatherIcaoJSON" );
        QUrlQuery urlQuery;
        urlQuery.addQueryItem( "ICAO", id.mid( 9 ) );
        urlQuery.addQueryItem( "username", "marble" );
        geonamesUrl.setQuery( urlQuery );
        emit downloadDescriptionFileRequested( geonamesUrl );
    }
}

void GeoNamesWeatherService::parseFile( const QByteArray& file )
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file);
    QJsonValue weatherObservationsValue = jsonDoc.object().value(QStringLiteral("weatherObservations"));

    // Parse if any result exists
    QList<AbstractDataPluginItem*> items;
    if (weatherObservationsValue.isArray()) {
        // Add items to the list
        QJsonArray weatherObservationsArray = weatherObservationsValue.toArray();
        for (int index = 0; index < weatherObservationsArray.size(); ++index) {
            QJsonObject weatherObservationObject = weatherObservationsArray[index].toObject();

            AbstractDataPluginItem* item = parse(weatherObservationObject);
            if ( item ) {
                items << item;
            }
        }
    } else {
        QJsonValue weatherObservationValue = jsonDoc.object().value(QStringLiteral("weatherObservation"));
        QJsonObject weatherObservationObject = weatherObservationValue.toObject();
        AbstractDataPluginItem* item = parse(weatherObservationObject);
        if ( item ) {
            items << item;
        }
    }

    emit createdItems( items );
}

AbstractDataPluginItem *GeoNamesWeatherService::parse(const QJsonObject &weatherObservationObject)
{
    const QString condition = weatherObservationObject.value(QStringLiteral("weatherCondition")).toString();
    const QString clouds = weatherObservationObject.value(QStringLiteral("clouds")).toString();
    const int windDirection = weatherObservationObject.value(QStringLiteral("windDirection")).toInt();
    QString id = weatherObservationObject.value(QStringLiteral("ICAO")).toString();
    const double temperature = weatherObservationObject.value(QStringLiteral("temperature")).toString().toDouble(); //delivered as string
    const int windSpeed = weatherObservationObject.value(QStringLiteral("windSpeed")).toString().toInt(); //delivered as string
    const int humidity = weatherObservationObject.value(QStringLiteral("humidity")).toInt();
    const double pressure = weatherObservationObject.value(QStringLiteral("seaLevelPressure")).toDouble();
    const QString name = weatherObservationObject.value(QStringLiteral("stationName")).toString();
    const QDateTime date = QDateTime::fromString(
                weatherObservationObject.value(QStringLiteral("datetime")).toString(), "yyyy-MM-dd hh:mm:ss" );
    const double longitude = weatherObservationObject.value(QStringLiteral("lng")).toDouble();
    const double latitude = weatherObservationObject.value(QStringLiteral("lat")).toDouble();

    if ( !id.isEmpty() ) {
        WeatherData data;

        // Weather condition
        if (clouds != QLatin1String("n/a") && condition != QLatin1String("n/a")) {
            if ( dayConditions.contains( condition ) ) {
                data.setCondition( dayConditions[condition] );
            } else {
                mDebug() << "UNHANDLED GEONAMES WEATHER CONDITION, PLEASE REPORT: " << condition;
            }
        } else {
            if ( dayConditions.contains( clouds ) ) {
                data.setCondition( dayConditions[clouds] );
            } else {
                mDebug() << "UNHANDLED GEONAMES CLOUDS CONDITION, PLEASE REPORT: " << clouds;
            }
        }

        // Wind direction. Finds the closest direction from windDirections array.
        if ( windDirection >= 0 ) {
            double tickSpacing = 360.0 / windDirections.size();
            data.setWindDirection( windDirections[int(( windDirection / tickSpacing ) + 0.5)
                                   % windDirections.size()] );
        }

        // Wind speed
        if ( windSpeed != 0 ) {
            data.setWindSpeed( windSpeed, WeatherData::knots );
        }

        // Temperature
        data.setTemperature( temperature, WeatherData::Celsius );

        // Humidity
        data.setHumidity( humidity );

        // Pressure
        if ( pressure != 0.0 ) {
            data.setPressure( pressure, WeatherData::HectoPascal );
        }

        // Date
        data.setDataDate( date.date() );
        data.setPublishingTime( date );

        // ID
        id = QLatin1String("geonames_") + id;

        GeoDataCoordinates coordinates( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
        GeoNamesWeatherItem *item = new GeoNamesWeatherItem( this );
        item->setMarbleWidget( marbleWidget() );
        item->setId( id );
        item->setCoordinate( coordinates );
        item->setPriority( 0 );
        item->setStationName( name );
        item->setCurrentWeather( data );
        return item;
    } else {
        return 0;
    }
}

void GeoNamesWeatherService::setupHashes()
{
    if( !( ( dayConditions.isEmpty() )
           || ( windDirections.isEmpty() ) ) )
    {
        return;
    }

    dayConditions["sunny"] = WeatherData::ClearDay;
    dayConditions["clear"] = WeatherData::ClearDay;
    dayConditions["clear sky"] = WeatherData::ClearDay;
    dayConditions["sunny intervals"] = WeatherData::FewCloudsDay;
    dayConditions["few clouds"] = WeatherData::FewCloudsDay;
    dayConditions["scattered clouds"] = WeatherData::FewCloudsDay;
    dayConditions["partly cloudy"] = WeatherData::PartlyCloudyDay;
    dayConditions["broken clouds"] = WeatherData::PartlyCloudyDay;
    dayConditions["white cloud"] = WeatherData::Overcast;
    dayConditions["overcast"] = WeatherData::Overcast;
    dayConditions["grey cloud"] = WeatherData::Overcast;
    dayConditions["cloudy"] = WeatherData::Overcast;
    dayConditions["drizzle"] = WeatherData::LightRain;
    dayConditions["light drizzle"] = WeatherData::LightRain;
    dayConditions["misty"] = WeatherData::Mist;
    dayConditions["mist"] = WeatherData::Mist;
    dayConditions["fog"] = WeatherData::Mist;
    dayConditions["foggy"] = WeatherData::Mist;
    dayConditions["dense fog"] = WeatherData::Mist;
    dayConditions["Thick Fog"] = WeatherData::Mist;
    dayConditions["tropical storm"] = WeatherData::Thunderstorm;
    dayConditions["thunderstorm"] = WeatherData::Thunderstorm;
    dayConditions["hazy"] = WeatherData::Mist;
    dayConditions["haze"] = WeatherData::Mist;
    dayConditions["in vicinity:  showers "] = WeatherData::ShowersDay;
    dayConditions["light shower"] = WeatherData::LightShowersDay;
    dayConditions["light rain shower"] = WeatherData::LightShowersDay;
    dayConditions["light showers"] = WeatherData::LightShowersDay;
    dayConditions["light rain"] = WeatherData::ShowersDay;
    dayConditions["heavy rain"] = WeatherData::Rain;
    dayConditions["heavy showers"] = WeatherData::Rain;
    dayConditions["heavy shower"] = WeatherData::Rain;
    dayConditions["heavy rain shower"] = WeatherData::Rain;
    dayConditions["thundery shower"] = WeatherData::Thunderstorm;
    dayConditions["thunderstorm"] = WeatherData::Thunderstorm;
    dayConditions["thunder storm"] = WeatherData::Thunderstorm;
    dayConditions["cloudy with sleet"] = WeatherData::RainSnow;
    dayConditions["sleet shower"] = WeatherData::RainSnow;
    dayConditions["sleet showers"] = WeatherData::RainSnow;
    dayConditions["sleet"] = WeatherData::RainSnow;
    dayConditions["cloudy with hail"] = WeatherData::Hail;
    dayConditions["hail shower"] = WeatherData::Hail;
    dayConditions["hail showers"] = WeatherData::Hail;
    dayConditions["hail"] = WeatherData::Hail;
    dayConditions["light snow"] = WeatherData::LightSnow;
    dayConditions["light snow shower"] = WeatherData::ChanceSnowDay;
    dayConditions["light snow showers"] = WeatherData::ChanceSnowDay;
    dayConditions["cloudy with light snow"] = WeatherData::LightSnow;
    dayConditions["heavy snow"] = WeatherData::Snow;
    dayConditions["heavy snow shower"] = WeatherData::Snow;
    dayConditions["heavy snow showers"] = WeatherData::Snow;
    dayConditions["cloudy with heavy snow"] = WeatherData::Snow;
    dayConditions["sandstorm"] = WeatherData::SandStorm;
    dayConditions["na"] = WeatherData::ConditionNotAvailable;
    dayConditions["n/a"] = WeatherData::ConditionNotAvailable;

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
