//
// This file is part of the Marble Virtual Globe.
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
#include "MarbleDebug.h"

// Qt
#include <QtCore/QAtomicInt>
#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QLocale>
#include <QtGui/QImage>

#include <cmath>

namespace Marble
{
    
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
const qreal HG2HPA = 1.33;
const qreal HPA2HG = 1/HG2HPA;
// inchHg to HectoPascal
const qreal IHG2HPA = HG2HPA * 25.4;
const qreal HPA2IHG = HPA2HG / 25.4;

// Summands
// Kelvin to degree Celsius
const qreal KEL2CEL = -273.15;
const qreal CEL2KEL = -KEL2CEL;

class WeatherDataPrivate
{
    Q_DECLARE_TR_FUNCTIONS ( WeatherDataPrivate )

 public:
    WeatherDataPrivate()
        : m_pubTime(),
          m_dataDate(),
          m_condition( WeatherData::ConditionNotAvailable ),
          m_windDirection( WeatherData::DirectionNotAvailable ),
          m_windSpeed( -1.0 ),
          m_temperature( -1.0 ),
          m_maxTemperature( -1.0 ),
          m_minTemperature( -1.0 ),
          m_visibility( WeatherData::VisibilityNotAvailable ),
          m_pressure( -1.0 ),
          m_pressureDevelopment( WeatherData::PressureDevelopmentNotAvailable ),
          m_humidity( -1.0 ),
          ref( 1 )
    {
        initializeIcons();
    }
    
    WeatherDataPrivate( const WeatherDataPrivate &other )
        : m_pubTime( other.m_pubTime ),
          m_dataDate( other.m_dataDate ),
          m_condition( other.m_condition ),
          m_windDirection( other.m_windDirection ),
          m_windSpeed( other.m_windSpeed ),
          m_temperature( other.m_temperature ),
          m_maxTemperature( other.m_maxTemperature ),
          m_minTemperature( other.m_minTemperature ),
          m_visibility( other.m_visibility ),
          m_pressure( other.m_pressure ),
          m_pressureDevelopment( other.m_pressureDevelopment ),
          m_humidity( other.m_humidity ),
          ref( other.ref )
    {
        initializeIcons();
    }
    
    void initializeIcons()
    {
        if( s_iconPath.size() == 0 ) {
            // Clouds
            s_iconPath.insert( WeatherData::ConditionNotAvailable,
                            MarbleDirs::path( "weather/weather-none-available.png" ) );
            s_iconPath.insert( WeatherData::ClearDay,
                            MarbleDirs::path( "weather/weather-clear.png" ) );
            s_iconPath.insert( WeatherData::ClearNight,
                            MarbleDirs::path( "weather/weather-clear-night.png" ) );
            s_iconPath.insert( WeatherData::FewCloudsDay,
                            MarbleDirs::path( "weather/weather-few-clouds.png" ) );
            s_iconPath.insert( WeatherData::FewCloudsNight,
                            MarbleDirs::path( "weather/weather-few-clouds-night.png" ) );
            s_iconPath.insert( WeatherData::PartlyCloudyDay,
                            MarbleDirs::path( "weather/weather-clouds.png" ) );
            s_iconPath.insert( WeatherData::PartlyCloudyNight,
                            MarbleDirs::path( "weather/weather-clouds-night.png" ) );
            s_iconPath.insert( WeatherData::Overcast,
                            MarbleDirs::path( "weather/weather-many-clouds.png" ) );

            // Rain
            s_iconPath.insert( WeatherData::LightShowersDay,
                            MarbleDirs::path( "weather/weather-showers-scattered-day.png" ) );
            s_iconPath.insert( WeatherData::LightShowersNight,
                            MarbleDirs::path( "weather/weather-showers-scattered-night.png" ) );
            s_iconPath.insert( WeatherData::ShowersDay,
                            MarbleDirs::path( "weather/weather-showers-day.png" ) );
            s_iconPath.insert( WeatherData::ShowersNight,
                            MarbleDirs::path( "weather/weather-showers-night.png" ) );
            s_iconPath.insert( WeatherData::LightRain,
                            MarbleDirs::path( "weather/weather-showers-scattered.png" ) );
            s_iconPath.insert( WeatherData::Rain,
                            MarbleDirs::path( "weather/weather-showers.png" ) );

            // Special
            s_iconPath.insert( WeatherData::ChanceThunderstormDay,
                            MarbleDirs::path( "weather/weather-storm-day.png" ) );
            s_iconPath.insert( WeatherData::ChanceThunderstormNight,
                            MarbleDirs::path( "weather/weather-storm-night.png" ) );
            s_iconPath.insert( WeatherData::Thunderstorm,
                            MarbleDirs::path( "weather/weather-storm.png" ) );
            s_iconPath.insert( WeatherData::Hail,
                            MarbleDirs::path( "weather/weather-hail.png" ) );
            s_iconPath.insert( WeatherData::ChanceSnowDay,
                            MarbleDirs::path( "weather/weather-snow-scattered-day.png" ) );
            s_iconPath.insert( WeatherData::ChanceSnowNight,
                            MarbleDirs::path( "weather/weather-snow-scattered-night.png" ) );
            s_iconPath.insert( WeatherData::LightSnow,
                            MarbleDirs::path( "weather/weather-snow-scattered.png" ) );
            s_iconPath.insert( WeatherData::Snow,
                            MarbleDirs::path( "weather/weather-snow.png" ) );
            s_iconPath.insert( WeatherData::RainSnow,
                            MarbleDirs::path( "weather/weather-snow-rain.png" ) );
            s_iconPath.insert( WeatherData::Mist,
                            MarbleDirs::path( "weather/weather-mist.png" ) );
            s_iconPath.insert( WeatherData::SandStorm,
                            MarbleDirs::path( "weather/weather-none-available.png" ) );
        }
    }
    
    qreal fromKelvin( qreal temp, WeatherData::TemperatureUnit format ) const
    {
        if( WeatherData::Kelvin == format ) {
            return temp;
        }
        else if ( WeatherData::Celsius == format ) {
            return temp + KEL2CEL;
        }
        else if ( WeatherData::Fahrenheit == format ) {
            return ( temp * 1.8 ) - 459.67;
        }
        else {
            mDebug() << "Wrong temperature format";
            return 0;
        }
    }
    
    qreal toKelvin( qreal temp, WeatherData::TemperatureUnit format ) const
    {
        if( WeatherData::Kelvin == format ) {
            return temp;
        }
        else if ( WeatherData::Celsius == format ) {
            return temp + CEL2KEL;
        }
        else if ( WeatherData::Fahrenheit == format ) {
            return ( temp + 459.67 ) / 1.8;
        }
        else {
            mDebug() << "Wrong temperature format";
            return 0;
        }
    }

    static bool isPositiveValue( qreal value )
    {
        // A small tolerance
        if( value > -0.5 ) {
            return true;
        }
        return false;
    }
    
    QString generateTemperatureString( qreal temp, WeatherData::TemperatureUnit format ) const
    {
        QLocale locale = QLocale::system();
        // We round to integer.
        QString string = locale.toString( floor( fromKelvin( temp, format ) + 0.5 ) );
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
        m_pubTime = other.m_pubTime;
        m_dataDate = other.m_dataDate;
        m_condition = other.m_condition;
        m_windDirection = other.m_windDirection;
        m_windSpeed = other.m_windSpeed;
        m_temperature = other.m_temperature;
        m_maxTemperature = other.m_maxTemperature;
        m_minTemperature = other.m_minTemperature;
        m_visibility = other.m_visibility;
        m_pressure = other.m_pressure;
        m_pressureDevelopment = other.m_pressureDevelopment;
        m_humidity = other.m_humidity;
        
        ref = other.ref;
        return *this;
    }

    QDateTime m_pubTime;
    QDate m_dataDate;
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

    WeatherData::PressureDevelopment m_pressureDevelopment;
    
    // Relative humidity
    qreal m_humidity;
    
    QAtomicInt ref;
    
    static QHash<WeatherData::WeatherCondition, QImage> s_icons;
    static QHash<WeatherData::WeatherCondition, QString> s_iconPath;
    static WeatherData::TemperatureUnit s_standardTemperatureUnit;
};

QHash<WeatherData::WeatherCondition, QImage> WeatherDataPrivate::s_icons = QHash<WeatherData::WeatherCondition, QImage>();
QHash<WeatherData::WeatherCondition, QString> WeatherDataPrivate::s_iconPath = QHash<WeatherData::WeatherCondition, QString>();
WeatherData::TemperatureUnit WeatherDataPrivate::s_standardTemperatureUnit = WeatherData::Celsius;

WeatherData::WeatherData()
    : d( new WeatherDataPrivate() )
{    
}

WeatherData::WeatherData( const WeatherData &other )
    : d( other.d )
{
    d->ref.ref();
}

WeatherData::~WeatherData()
{
    if ( !d->ref.deref() )
        delete d;
}

bool WeatherData::isValid() const
{
    return hasValidPublishingTime()
           || hasValidDataDate()
           || hasValidCondition()
           || hasValidWindDirection()
           || hasValidWindSpeed()
           || hasValidTemperature()
           || hasValidMaxTemperature()
           || hasValidMinTemperature()
           || hasValidVisibility()
           || hasValidPressure()
           || hasValidPressureDevelopment()
           || hasValidHumidity();
}

QDateTime WeatherData::publishingTime() const
{
    return d->m_pubTime;
}

void WeatherData::setPublishingTime( const QDateTime& dateTime )
{
    detach();
    d->m_pubTime = dateTime.toUTC();
}

bool WeatherData::hasValidPublishingTime() const
{
    return d->m_pubTime.isValid();
}

QDate WeatherData::dataDate() const
{
    return d->m_dataDate;
}

void WeatherData::setDataDate( const QDate& date )
{
    detach();
    d->m_dataDate = date;
}

bool WeatherData::hasValidDataDate() const
{
    return d->m_dataDate.isValid();
}

WeatherData::WeatherCondition WeatherData::condition() const
{
    return d->m_condition;
}

void WeatherData::setCondition( WeatherData::WeatherCondition condition )
{
    detach();
    d->m_condition = condition;
}

bool WeatherData::hasValidCondition() const
{
    return d->m_condition != WeatherData::ConditionNotAvailable;
}

QString WeatherData::conditionString() const
{
    switch ( condition() ) {
        case ClearDay:
            return tr( "sunny" );
        case ClearNight:
            return tr( "clear" );
        case FewCloudsDay:
        case FewCloudsNight:
            return tr( "few clouds" );
        case PartlyCloudyDay:
        case PartlyCloudyNight:
            return tr( "partly cloudy" );
        case Overcast:
            return tr( "overcast" );
        case LightShowersDay:
        case LightShowersNight:
            return tr( "light showers" );
        case ShowersDay:
        case ShowersNight:
            return tr( "showers" );
        case LightRain:
            return tr( "light rain" );
        case Rain:
            return tr( "rain" );
        case ChanceThunderstormDay:
        case ChanceThunderstormNight:
            return tr( "occasionally thunderstorm" );
        case Thunderstorm:
            return tr( "thunderstorm" );
        case Hail:
            return tr( "hail" );
        case ChanceSnowDay:
        case ChanceSnowNight:
            return tr( "occasionally snow" );
        case LightSnow:
            return tr( "light snow" );
        case Snow:
            return tr( "snow" );
        case RainSnow:
            return tr( "rain and snow" );
        case Mist:
            return tr( "mist" );
        case SandStorm:
            return tr( "sandstorm" );
        default:
            return "Condition not available";
    }
}

QImage WeatherData::icon() const
{
    QImage icon = WeatherDataPrivate::s_icons.value( condition() );
    
    // If the icon is in the hash, simply return it.
    if ( !icon.isNull() ) {
        return icon;
    }
    // If it isn't in the hash, the icon will be created (from the value stored in s_iconPath).
    else {
        icon = QImage( WeatherDataPrivate::s_iconPath.value( condition() ) );
        WeatherDataPrivate::s_icons.insert( condition(), icon );
        return icon;
    }
}

WeatherData::WindDirection WeatherData::windDirection() const
{
    return d->m_windDirection;
}

void WeatherData::setWindDirection( WeatherData::WindDirection direction )
{
    detach();
    d->m_windDirection = direction;
}

bool WeatherData::hasValidWindDirection() const
{
    return d->m_windDirection != WeatherData::DirectionNotAvailable;
}

QString WeatherData::windDirectionString() const
{
    switch ( windDirection() ) {
        case N:
            return tr( "N" );
        case NNE:
            return tr( "NNE" );
        case NE:
            return tr( "NE" );
        case ENE:
            return tr( "ENE" );
        case E:
            return tr( "E" );
        case SSE:
            return tr( "SSE" );
        case SE:
            return tr( "SE" );
        case ESE:
            return tr( "ESE" );
        case S:
            return tr( "S" );
        case NNW:
            return tr( "NNW" );
        case NW:
            return tr( "NW" );
        case WNW:
            return tr( "WNW" );
        case W:
            return tr( "W" );
        case SSW:
            return tr( "SSW" );
        case SW:
            return tr( "SW" );
        case WSW:
            return tr( "WSW" );
        default:
            return "";
    }
}

qreal WeatherData::windSpeed( WeatherData::SpeedUnit format ) const
{
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
        mDebug() << "Wrong speed format";
        return 0;
    }
}

void WeatherData::setWindSpeed( qreal speed, WeatherData::SpeedUnit format )
{
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
        mDebug() << "Wrong speed format";
    }
}

bool WeatherData::hasValidWindSpeed() const
{
    return d->isPositiveValue( d->m_windSpeed );
}

QString WeatherData::windSpeedString( WeatherData::SpeedUnit unit ) const
{
    QLocale locale = QLocale::system();
    // We round to integer.
    QString string = locale.toString( floor( windSpeed( unit ) + 0.5 ) );
    string += ' ';
    switch ( unit ) {
        case WeatherData::kph:
            string += QObject::tr("km/h");
            break;
        case WeatherData::mph:
            string += QObject::tr("mph");
            break;
        case WeatherData::mps:
            string += QObject::tr( "m/s" );
            break;
        case WeatherData::knots:
            string += QObject::tr( "knots" );
            break;
        case WeatherData::beaufort:
            string += QObject::tr( "Beaufort" );
            break;
    }
    return string;
}

qreal WeatherData::temperature( WeatherData::TemperatureUnit format ) const
{
    return d->fromKelvin( d->m_temperature, format );
}

void WeatherData::setTemperature( qreal temp, WeatherData::TemperatureUnit format )
{
    detach();
    d->m_temperature = d->toKelvin( temp, format );
}

bool WeatherData::hasValidTemperature() const
{
    return d->isPositiveValue( d->m_temperature );
}

QString WeatherData::temperatureString( WeatherData::TemperatureUnit format ) const
{
    return d->generateTemperatureString( d->m_temperature,
                                         format );
}

qreal WeatherData::maxTemperature( WeatherData::TemperatureUnit format ) const
{
    return d->fromKelvin( d->m_maxTemperature, format );
}

void WeatherData::setMaxTemperature( qreal temp, WeatherData::TemperatureUnit format )
{
    detach();
    d->m_maxTemperature = d->toKelvin( temp, format );
}

QString WeatherData::maxTemperatureString( WeatherData::TemperatureUnit format ) const
{
    return d->generateTemperatureString( d->m_maxTemperature,
                                         format );
}

bool WeatherData::hasValidMaxTemperature() const
{
    return d->isPositiveValue( d->m_maxTemperature );
}

qreal WeatherData::minTemperature( WeatherData::TemperatureUnit format ) const
{
    return d->fromKelvin( d->m_minTemperature, format );
}

QString WeatherData::minTemperatureString( WeatherData::TemperatureUnit format ) const
{
    return d->generateTemperatureString( d->m_minTemperature,
                                         format );
}

void WeatherData::setMinTemperature( qreal temp, WeatherData::TemperatureUnit format )
{
    detach();
    d->m_minTemperature = d->toKelvin( temp, format );
}

bool WeatherData::hasValidMinTemperature() const
{
    return d->isPositiveValue( d->m_minTemperature );
}

WeatherData::Visibility WeatherData::visibility() const
{
    return d->m_visibility;
}

void WeatherData::setVisibilty( WeatherData::Visibility visibility )
{
    detach();
    d->m_visibility = visibility;
}

bool WeatherData::hasValidVisibility() const
{
    return d->m_visibility != WeatherData::VisibilityNotAvailable;
}

qreal WeatherData::pressure( WeatherData::PressureUnit format ) const
{
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
    else if ( WeatherData::inchHg == format ) {
        return d->m_pressure * HPA2IHG;
    }
    else {
        mDebug() << "Wrong pressure format";
        return 0;
    }
}

void WeatherData::setPressure( qreal pressure, WeatherData::PressureUnit format )
{
    detach();
    if ( WeatherData::HectoPascal == format ) {
        d->m_pressure = pressure;
    }
    else if ( WeatherData::KiloPascal == format ) {
        d->m_pressure = pressure * KPA2HPA;
    }
    else if ( WeatherData::Bar == format ) {
        d->m_pressure = pressure * BAR2HPA;
    }
    else if ( WeatherData::mmHg == format ) {
        d->m_pressure = pressure * HG2HPA;
    }
    else if ( WeatherData::inchHg == format ) {
        d->m_pressure = pressure * IHG2HPA;
    }
    else {
        mDebug() << "Wrong pressure format";
    }
}

bool WeatherData::hasValidPressure() const
{
    return d->isPositiveValue( d->m_pressure );
}

QString WeatherData::pressureString( WeatherData::PressureUnit unit ) const
{
    QLocale locale = QLocale::system();
    // We round to integer.
    QString string = locale.toString( pressure( unit ), 'f', 2 );
    string += ' ';
    switch ( unit ) {
        case WeatherData::HectoPascal:
            string += tr( "hPa" );
            break;
        case WeatherData::KiloPascal:
            string += tr( "kPa" );
            break;
        case WeatherData::Bar:
            string += tr( "Bar" );
            break;
        case WeatherData::mmHg:
            string += tr( "mmHg" );
            break;
        case WeatherData::inchHg:
            string += tr( "inch Hg" );
            break;
    }
    return string;
}

WeatherData::PressureDevelopment WeatherData::pressureDevelopment() const
{
    return d->m_pressureDevelopment;
}

void WeatherData::setPressureDevelopment( WeatherData::PressureDevelopment pressureDevelopment )
{
    detach();
    d->m_pressureDevelopment = pressureDevelopment;
}

bool WeatherData::hasValidPressureDevelopment() const
{
    return d->m_pressureDevelopment != WeatherData::PressureDevelopmentNotAvailable;
}

QString WeatherData::pressureDevelopmentString() const
{
    switch ( pressureDevelopment() ) {
        case Rising:
            return tr( "rising", "air pressure is rising" );
        case NoChange:
            return tr( "steady", "air pressure has no change" );
        case Falling:
            return tr( "falling", "air pressure falls" );
        default:
            return "";
    }
}

qreal WeatherData::humidity() const
{
    return d->m_humidity;
}

void WeatherData::setHumidity( qreal humidity )
{
    detach();
    d->m_humidity = humidity;
}

bool WeatherData::hasValidHumidity() const
{
    return d->isPositiveValue( d->m_humidity );
}

QString WeatherData::humidityString() const
{
    return QString( "%1 %" ).arg( humidity() );
}

QString WeatherData::toHtml( WeatherData::TemperatureUnit temperatureUnit,
                             WeatherData::SpeedUnit speedUnit,
                             WeatherData::PressureUnit pressureUnit ) const
{
    QString html;
    if ( hasValidPublishingTime() ) {
        html += tr( "Publishing time: %1<br>" )
                 .arg( publishingTime().toLocalTime().toString() );
    }
    if ( hasValidCondition() ) {
        html += tr( "Condition: %1<br>" )
                 .arg( conditionString() );
    }
    if ( hasValidTemperature() ) {
        html += tr( "Temperature: %1<br>" )
                 .arg( temperatureString( temperatureUnit ) );
    }
    if ( hasValidMaxTemperature() ) {
        html += tr( "Max temperature: %1<br>" )
                 .arg( maxTemperatureString( temperatureUnit ) );
    }
    if ( hasValidMinTemperature() ) {
        html += tr( "Min temperature: %1<br>" )
                 .arg( minTemperatureString( temperatureUnit ) );
    }
    if ( hasValidWindDirection() ) {
        html += tr( "Wind direction: %1<br>" )
                 .arg( windDirectionString() );
    }
    if ( hasValidWindSpeed() ) {
        html += tr( "Wind speed: %1<br>" )
                 .arg( windSpeedString( speedUnit ) );
    }
    if ( hasValidPressure() ) {
        html += tr( "Pressure: %1<br>" )
                 .arg( pressureString( pressureUnit ) );
    }
    if ( hasValidPressureDevelopment() ) {
        html += tr( "Pressure development: %1<br>")
                 .arg( pressureDevelopmentString() );
    }
    if ( hasValidHumidity() ) {
        html += tr( "Humidity: %1<br>" )
                 .arg( humidityString() );
    }

    return html;
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
