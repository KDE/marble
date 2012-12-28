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
#include "MarbleModel.h"
#include "MarbleDebug.h"

#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

using namespace Marble;

QHash<QString, WeatherData::WeatherCondition> GeoNamesWeatherService::dayConditions
        = QHash<QString, WeatherData::WeatherCondition>();
QVector<WeatherData::WindDirection> GeoNamesWeatherService::windDirections
        = QVector<WeatherData::WindDirection>(16);

GeoNamesWeatherService::GeoNamesWeatherService( QObject *parent )
            : AbstractWeatherService( parent )
{
    GeoNamesWeatherService::setupHashes();
}

GeoNamesWeatherService::~GeoNamesWeatherService()
{
}

void GeoNamesWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                            const MarbleModel *model,
                                            qint32 number )
{
    if( model->planetId() != "earth" ) {
        return;
    }

    QUrl geonamesUrl( "http://ws.geonames.org/weatherJSON" );
    geonamesUrl.addQueryItem( "north", QString::number( box.north( GeoDataCoordinates::Degree ) ) );
    geonamesUrl.addQueryItem( "south", QString::number( box.south( GeoDataCoordinates::Degree ) ) );
    geonamesUrl.addQueryItem( "east", QString::number( box.east( GeoDataCoordinates::Degree ) ) );
    geonamesUrl.addQueryItem( "west", QString::number( box.west( GeoDataCoordinates::Degree ) ) );
    geonamesUrl.addQueryItem( "maxRows", QString::number( number ) );

    emit downloadDescriptionFileRequested( geonamesUrl );
}

void GeoNamesWeatherService::getItem( const QString &id, const MarbleModel *model )
{
    if( model->planetId() != "earth" ) {
        return;
    }

    if ( id.startsWith(QLatin1String("geonames_") ) ) {
        QUrl geonamesUrl( "http://ws.geonames.org/weatherIcaoJSON" );
        geonamesUrl.addQueryItem( "ICAO", id.mid( 9 ) );
        emit downloadDescriptionFileRequested( geonamesUrl );
    }
}

void GeoNamesWeatherService::parseFile( const QByteArray& file )
{
    QScriptValue data;
    QScriptEngine engine;

    // Qt requires parentheses around json code
    data = engine.evaluate( "(" + QString( file ) + ")" );

    // Parse if any result exists
    QList<AbstractDataPluginItem*> items;
    if ( data.property( "weatherObservations" ).isArray() ) {
        QScriptValueIterator iterator( data.property( "weatherObservations" ) );
        // Add items to the list
        while ( iterator.hasNext() ) {
            iterator.next();
            AbstractDataPluginItem* item = parse( iterator.value() );
            if ( item ) {
                items << item;
            }
        }
    } else {
        AbstractDataPluginItem* item = parse( data.property( "weatherObservation" ) );
        if ( item ) {
            items << item;
        }
    }

    emit createdItems( items );
}

AbstractDataPluginItem *GeoNamesWeatherService::parse( const QScriptValue &value )
{
    QString condition = value.property( "weatherCondition" ).toString();
    QString clouds = value.property( "clouds" ).toString();
    int windDirection = value.property( "windDirection" ).toInteger();
    QString id = value.property( "ICAO" ).toString();
    int temperature = value.property( "temperature" ).toInteger();
    int windSpeed = value.property( "windSpeed" ).toInteger();
    int humidity = value.property( "humidity" ).toInteger();
    double pressure = value.property( "seaLevelPressure" ).toNumber();
    QString name = value.property( "stationName" ).toString();
    QDateTime date = QDateTime::fromString(
                value.property( "datetime" ).toString(), "yyyy-MM-dd hh:mm:ss" );
    double longitude = value.property( "lng" ).toNumber();
    double latitude = value.property( "lat" ).toNumber();

    if ( !id.isEmpty() ) {
        WeatherData data;

        // Weather condition
        if ( clouds != "n/a" && condition != "n/a" ) {
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
        id = "geonames_" + id;

        GeoDataCoordinates coordinates( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
        GeoNamesWeatherItem *item = new GeoNamesWeatherItem( this );
        item->setId( id );
        item->setCoordinate( coordinates );
        item->setTarget( "earth" );
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

#include "GeoNamesWeatherService.moc"
