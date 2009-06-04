//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WeatherData.h"

// Marble
#include "global.h"
#include "MarbleDirs.h"

// Qt
#include <QtCore/QAtomicInt>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtGui/QIcon>

namespace Marble {
    
// Factors
// m/s to knots
const qreal MPS2KN = 1.9437;
const qreal KN2MPS = 1 / MPS2KN;
// m/s to kilometer per hour
const qreal MPS2KPH = 3.6;
const qreal KPH2MPS = 1 / MPS2KPH;
// m/s to miles per hour
const qreal MPS2MPH = MPS2KPH * KM2MI;
const qreal MPH2MPS = 1 / MPS2MPH;
// HectoPascal to KiloPascal
const qreal HPA2KPA = 10;
const qreal KPA2HPA = 1/HPA2KPA;
// Bar to HectoPascal
const qreal BAR2HPA = 1000;
const qreal HPA2BAR = 1/BAR2HPA;
// mmHg to HectoPascal
const qreal HG2HPA = 133;
const qreal HPA2HG = 1/HG2HPA;

// Summands
// Kelvin to degree Celsius
const qreal KEL2CEL = -273.15;
const qreal CEL2KEL = -KEL2CEL;

class WeatherDataPrivate {
 public:
    WeatherDataPrivate()
        : m_dateTime( QDateTime::currentDateTime() ),
          m_condition( WeatherData::ConditionNotAvailable ),
          m_windDirection( WeatherData::DirectionNotAvailable ),
          m_windSpeed( 0 ),
          m_temperature( 0 ),
          m_maxTemperature( 0 ),
          m_minTemperature( 0 ),
          m_visibility( WeatherData::VisibilityNotAvailable ),
          m_pressure( 0 ),
          m_humidity( 0 ),
          ref( 1 )
    {
        initializeIcons();
    }
    
    WeatherDataPrivate( const WeatherDataPrivate &other )
        : m_dateTime( other.m_dateTime ),
          m_condition( other.m_condition ),
          m_windDirection( other.m_windDirection ),
          m_windSpeed( other.m_windSpeed ),
          m_temperature( other.m_temperature ),
          m_maxTemperature( other.m_maxTemperature ),
          m_minTemperature( other.m_minTemperature ),
          m_visibility( other.m_visibility ),
          m_pressure( other.m_pressure ),
          m_humidity( other.m_humidity ),
          ref( other.ref )
    {
        initializeIcons();
    }
    
    void initializeIcons() {
        if( s_iconPath.size() == 0 ) {
            // Clouds
            s_iconPath.insert( WeatherData::ConditionNotAvailable,
                            MarbleDirs::path( "weather/weather-none-available.svgz" ) );
            s_iconPath.insert( WeatherData::ClearDay,
                            MarbleDirs::path( "weather/weather-clear.svgz" ) );
            s_iconPath.insert( WeatherData::ClearNight,
                            MarbleDirs::path( "weather/weather-clear-night.svgz" ) );
            s_iconPath.insert( WeatherData::FewCloudsDay,
                            MarbleDirs::path( "weather/weather-few-clouds.svgz" ) );
            s_iconPath.insert( WeatherData::FewCloudsNight,
                            MarbleDirs::path( "weather/weather-few-clouds-night.svgz" ) );
            s_iconPath.insert( WeatherData::PartlyCloudyDay,
                            MarbleDirs::path( "weather/weather-clouds.svgz" ) );
            s_iconPath.insert( WeatherData::PartlyCloudyNight,
                            MarbleDirs::path( "weather/weather-clouds-night.svgz" ) );
            s_iconPath.insert( WeatherData::Overcast,
                            MarbleDirs::path( "weather/weather-many-clouds.svg" ) );

            // Rain
            s_iconPath.insert( WeatherData::LightShowersDay,
                            MarbleDirs::path( "weather/weather-showers-scattered-day.svgz" ) );
            s_iconPath.insert( WeatherData::LightShowersNight,
                            MarbleDirs::path( "weather/weather-showers-scattered-night.svgz" ) );
            s_iconPath.insert( WeatherData::ShowersDay,
                            MarbleDirs::path( "weather/weather-showers-day.svgz" ) );
            s_iconPath.insert( WeatherData::ShowersNight,
                            MarbleDirs::path( "weather/weather-showers-night.svgz" ) );
            s_iconPath.insert( WeatherData::LightRain,
                            MarbleDirs::path( "weather/weather-showers-scattered.svgz" ) );
            s_iconPath.insert( WeatherData::Rain,
                            MarbleDirs::path( "weather/weather-showers.svgz" ) );

            // Special
            s_iconPath.insert( WeatherData::ChanceThunderstormDay,
                            MarbleDirs::path( "weather/weather-storm-day.svgz" ) );
            s_iconPath.insert( WeatherData::ChanceThunderstormNight,
                            MarbleDirs::path( "weather/weather-storm-night.svgz" ) );
            s_iconPath.insert( WeatherData::Thunderstorm,
                            MarbleDirs::path( "weather/weather-storm.svgz" ) );
            s_iconPath.insert( WeatherData::Hail,
                            MarbleDirs::path( "weather/weather-hail.svgz" ) );
            s_iconPath.insert( WeatherData::ChanceSnowDay,
                            MarbleDirs::path( "weather/weather-snow-scattered-day.svgz" ) );
            s_iconPath.insert( WeatherData::ChanceSnowNight,
                            MarbleDirs::path( "weather/weather-snow-scattered-night.svgz" ) );
            s_iconPath.insert( WeatherData::LightSnow,
                            MarbleDirs::path( "weather/weather-snow-scattered.svgz" ) );
            s_iconPath.insert( WeatherData::Snow,
                            MarbleDirs::path( "weather/weather-snow.svgz" ) );
            s_iconPath.insert( WeatherData::RainSnow,
                            MarbleDirs::path( "weather/weather-snow-rain.svgz" ) );
            s_iconPath.insert( WeatherData::Mist,
                            MarbleDirs::path( "weather/weather-mist.svgz" ) );
        }
    }
    
    qreal fromKelvin( qreal temp, WeatherData::TemperatureFormat format ) const {
        if( WeatherData::Kelvin == format ) {
            return temp;
        }
        else if ( WeatherData::Celsius == format ) {
            return temp + KEL2CEL;
        }
        else if ( WeatherData::Fahrenheit == format ) {
            return ( temp + 459.67 ) * 5 / 9;
        }
        else {
            qDebug() << "Wrong temperature format";
            return 0;
        }
    }
    
    qreal toKelvin( qreal temp, WeatherData::TemperatureFormat format ) const {
        if( WeatherData::Kelvin == format ) {
            return temp;
        }
        else if ( WeatherData::Celsius == format ) {
            return temp + CEL2KEL;
        }
        else if ( WeatherData::Fahrenheit == format ) {
            return ( temp * 1.8 ) - 459.67;
        }
        else {
            qDebug() << "Wrong temperature format";
            return 0;
        }
    }
    
    QString generateTemperatureString( qreal temp, WeatherData::TemperatureFormat format ) const {
        QString string = QString::number( fromKelvin( temp, format ) );
        switch ( format ) {
            case WeatherData::Kelvin:
                string += " K";
                break;
            case WeatherData::Celsius:
                string += " \xb0 C";
                break;
            case WeatherData::Fahrenheit:
                string += " \xb0 F";
                break;
        }
        return string;
    }
            
    
    WeatherDataPrivate& operator=( const WeatherDataPrivate &other )
    {
        m_dateTime = other.m_dateTime;
        m_condition = other.m_condition;
        m_windDirection = other.m_windDirection;
        m_windSpeed = other.m_windSpeed;
        m_temperature = other.m_temperature;
        m_maxTemperature = other.m_maxTemperature;
        m_minTemperature = other.m_minTemperature;
        m_visibility = other.m_visibility;
        m_pressure = other.m_pressure;
        m_humidity = other.m_humidity;
        
        ref = other.ref;
        return *this;
    }
     
    QDateTime m_dateTime;
    WeatherData::WeatherCondition m_condition;
    WeatherData::WindDirection m_windDirection;
    
    // Wind speed stored in m/s
    qreal m_windSpeed;
    
    // Temperatures stored in Kelvin
    qreal m_temperature;
    qreal m_maxTemperature;
    qreal m_minTemperature;
    
    WeatherData::Visibility m_visibility;
    
    // Pressure stored in hecto pascal
    qreal m_pressure;
    
    // Relative humidity
    qreal m_humidity;
    
    QAtomicInt ref;
    
    static QHash<WeatherData::WeatherCondition, QIcon> s_icons;
    static QHash<WeatherData::WeatherCondition, QString> s_iconPath;
    static WeatherData::TemperatureFormat s_standardTemperatureFormat;
};

QHash<WeatherData::WeatherCondition, QIcon> WeatherDataPrivate::s_icons = QHash<WeatherData::WeatherCondition, QIcon>();
QHash<WeatherData::WeatherCondition, QString> WeatherDataPrivate::s_iconPath = QHash<WeatherData::WeatherCondition, QString>();
WeatherData::TemperatureFormat WeatherDataPrivate::s_standardTemperatureFormat = WeatherData::Celsius;

WeatherData::WeatherData()
    : d( new WeatherDataPrivate() )
{    
}

WeatherData::WeatherData( const WeatherData &other )
    : d( other.d )
{
    d->ref.ref();
}

WeatherData::~WeatherData() {
    if ( !d->ref.deref() )
        delete d;
}

QDateTime WeatherData::dateTime() const {
    return d->m_dateTime;
}

void WeatherData::setDateTime( const QDateTime& dateTime ) {
    detach();
    d->m_dateTime = dateTime;
}

WeatherData::WeatherCondition WeatherData::condition() const {
    return d->m_condition;
}

void WeatherData::setCondition( WeatherData::WeatherCondition condition ) {
    detach();
    d->m_condition = condition;
}

QIcon WeatherData::icon() const {
    QIcon icon = WeatherDataPrivate::s_icons.value( condition() );
    
    // If the icon is in the hash, simply return it.
    if ( !icon.isNull() ) {
        return icon;
    }
    // If it isn't in the hash, the icon will be created (from the value stored in s_iconPath).
    else {
        icon.addFile( WeatherDataPrivate::s_iconPath.value( condition() ) );
        WeatherDataPrivate::s_icons.insert( condition(), icon );
        return icon;
    }
}

WeatherData::WindDirection WeatherData::windDirection() const {
    return d->m_windDirection;
}

void WeatherData::setWindDirection( WeatherData::WindDirection direction ) {
    detach();
    d->m_windDirection = direction;
}

qreal WeatherData::windSpeed( WeatherData::SpeedFormat format ) const {
    if ( WeatherData::mps == format ) {
        return d->m_windSpeed;
    }
    if ( WeatherData::kph == format ) {
        return d->m_windSpeed * MPS2KPH;
    }
    else if ( WeatherData::mph == format ) {
        return d->m_windSpeed * MPS2MPH;
    }
    else if ( WeatherData::knots == format ) {
        return d->m_windSpeed * MPS2KN;
    }
    else if ( WeatherData::beaufort == format ) {
        if( d->m_windSpeed < 0.3 )
            return 0;
        else if( d->m_windSpeed < 1.6 )
            return 1;
        else if( d->m_windSpeed < 3.4 )
            return 2;
        else if( d->m_windSpeed < 5.5 )
            return 3;
        else if( d->m_windSpeed < 8.0 )
            return 4;
        else if( d->m_windSpeed < 10.8 )
            return 5;
        else if( d->m_windSpeed < 13.9 )
            return 6;
        else if( d->m_windSpeed < 17.2 )
            return 7;
        else if( d->m_windSpeed < 20.8 )
            return 8;
        else if( d->m_windSpeed < 24.5 )
            return 9;
        else if( d->m_windSpeed < 28.5 )
            return 10;
        else if( d->m_windSpeed < 32.7 )
            return 11;
        else 
            return 12;
    }
    else {
        qDebug() << "Wrong speed format";
        return 0;
    }
}

void WeatherData::setWindSpeed( qreal speed, WeatherData::SpeedFormat format ) {
    detach();
    if ( WeatherData::mps == format ) {
        d->m_windSpeed = speed;
    }
    if ( WeatherData::kph == format ) {
        d->m_windSpeed = speed * KPH2MPS;
    }
    else if ( WeatherData::mph == format ) {
        d->m_windSpeed = speed * MPH2MPS;
    }
    else if ( WeatherData::knots == format ) {
        d->m_windSpeed = speed * KN2MPS;
    }
    else if ( WeatherData::beaufort == format ) {
        int rounded = (int) speed;
        if( 0 == rounded )
            d->m_windSpeed = 0.15;
        else if( 1 == rounded )
            d->m_windSpeed = 0.95;
        else if( 2 == rounded )
            d->m_windSpeed = 2.5;
        else if( 3 == rounded )
            d->m_windSpeed = 4.45;
        else if( 4 == rounded )
            d->m_windSpeed = 6.75;
        else if( 5 == rounded )
            d->m_windSpeed = 9.4;
        else if( 6 == rounded )
            d->m_windSpeed = 12.35;
        else if( 7 == rounded )
            d->m_windSpeed = 15.55;
        else if( 8 == rounded )
            d->m_windSpeed = 19.0;
        else if( 9 == rounded )
            d->m_windSpeed = 22.65;
        else if( 10 == rounded )
            d->m_windSpeed = 26.5;
        else if( 11 == rounded )
            d->m_windSpeed = 30.6;
        else 
            d->m_windSpeed = 34;
    }
    else {
        qDebug() << "Wrong speed format";
    }
}

qreal WeatherData::temperature( WeatherData::TemperatureFormat format ) const {
    return d->fromKelvin( d->m_temperature, format );
}

void WeatherData::setTemperature( qreal temp, WeatherData::TemperatureFormat format ) {
    detach();
    d->m_temperature = d->toKelvin( temp, format );
}

QString WeatherData::temperatureString() const {
    return d->generateTemperatureString( d->m_temperature,
                                         WeatherDataPrivate::s_standardTemperatureFormat );
}

qreal WeatherData::maxTemperature( WeatherData::TemperatureFormat format ) const {
    return d->fromKelvin( d->m_maxTemperature, format );
}

void WeatherData::setMaxTemperature( qreal temp, WeatherData::TemperatureFormat format ) {
    detach();
    d->m_maxTemperature = d->toKelvin( temp, format );
}

QString WeatherData::maxTemperatureString() const {
    return d->generateTemperatureString( d->m_maxTemperature,
                                         WeatherDataPrivate::s_standardTemperatureFormat );
}

qreal WeatherData::minTemperature( WeatherData::TemperatureFormat format ) const {
    return d->fromKelvin( d->m_minTemperature, format );
}

QString WeatherData::minTemperatureString() const {
    return d->generateTemperatureString( d->m_minTemperature,
                                         WeatherDataPrivate::s_standardTemperatureFormat );
}

void WeatherData::setMinTemperature( qreal temp, WeatherData::TemperatureFormat format ) {
    detach();
    d->m_minTemperature = d->toKelvin( temp, format );
}

WeatherData::Visibility WeatherData::visibility() const {
    return d->m_visibility;
}

void WeatherData::setVisibilty( WeatherData::Visibility visibility ) {
    detach();
    d->m_visibility = visibility;
}

qreal WeatherData::pressure( WeatherData::PressureFormat format ) const {
    if ( WeatherData::HectoPascal == format ) {
        return d->m_pressure;
    }
    else if ( WeatherData::KiloPascal == format ) {
        return d->m_pressure * HPA2KPA;
    }
    else if ( WeatherData::Bar == format ) {
        return d->m_pressure * HPA2BAR;
    }
    else if ( WeatherData::mmHg == format ) {
        return d->m_pressure * HPA2HG;
    }
    else {
        qDebug() << "Wrong pressure format";
        return 0;
    }
}

void WeatherData::setPressure( qreal pressure, WeatherData::PressureFormat format ) {
    detach();
    if ( WeatherData::HectoPascal == format ) {
        d->m_pressure = pressure;
    }
    else if ( WeatherData::KiloPascal == format ) {
        d->m_pressure = KPA2HPA;
    }
    else if ( WeatherData::Bar == format ) {
        d->m_pressure = BAR2HPA;
    }
    else if ( WeatherData::mmHg == format ) {
        d->m_pressure = HG2HPA;
    }
    else {
        qDebug() << "Wrong pressure format";
    }
}

qreal WeatherData::humidity() const {
    return d->m_humidity;
}

void WeatherData::setHumidity( qreal humidity ) {
    detach();
    d->m_humidity = humidity;
}

WeatherData& WeatherData::operator=( const WeatherData &other )
{
    qAtomicAssign( d, other.d );
    return *this;
}

void WeatherData::detach()
{
    qAtomicDetach( d );
}

} // namespace Marble
