//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QtGlobal>
#include <QCoreApplication>

class QDate;
class QDateTime;
class QImage;

namespace Marble
{
    
class WeatherDataPrivate;

class WeatherData
{
    Q_DECLARE_TR_FUNCTIONS ( WeatherData )
 public:
    enum WeatherCondition {
        // Clouds
        ConditionNotAvailable,
        ClearDay,
        ClearNight,
        FewCloudsDay,
        FewCloudsNight,
        PartlyCloudyDay,
        PartlyCloudyNight,
        Overcast,
        // Rain
        LightShowersDay,
        LightShowersNight,
        ShowersDay,
        ShowersNight,
        LightRain,
        Rain,
        // Special
        ChanceThunderstormDay,
        ChanceThunderstormNight,
        Thunderstorm,
        Hail,
        ChanceSnowDay,
        ChanceSnowNight,
        LightSnow,
        Snow,
        RainSnow,
        Mist,
        SandStorm // no icon available
//         FreezingDrizzle, would be nice, but no icon for the moment
//         Flurries, would be nice, but no icon available
        // REMEMBER to change the implementation of conditionString() if you add items to this enum
    };
    
    enum WindDirection {
        N,
        NNE,
        NE,
        ENE,
        E,
        SSE,
        SE,
        ESE,
        S,
        NNW,
        NW,
        WNW,
        W,
        SSW,
        SW,
        WSW,
        DirectionNotAvailable
    };

    enum PressureDevelopment {
        Rising,
        NoChange,
        Falling,
        PressureDevelopmentNotAvailable
    };
    
    enum Visibility {
        VeryGood,
        Good,
        Normal,
        Poor,
        VeryPoor,
        Fog,
        VisibilityNotAvailable
    };
    
    enum SpeedUnit {
        kph,
        mph,
        mps,
        knots,
        beaufort
    };
    
    enum TemperatureUnit {
        Celsius,
        Fahrenheit,
        Kelvin
    };
    
    enum PressureUnit {
        HectoPascal,
        KiloPascal,
        Bar,
        mmHg,
        inchHg
    };
    
    WeatherData();
    WeatherData( const WeatherData &other );
    ~WeatherData();

    bool isValid() const;
    
    /**
     * Get the time when the data was published.
     */
    QDateTime publishingTime() const;

    /**
     * Set the time when the data was published.
     */
    void setPublishingTime( const QDateTime& dateTime );

    /**
     * Checks, if the data item has a valid publishing time.
     */
    bool hasValidPublishingTime() const;

    /**
     * Get the date the data is meant for.
     */
    QDate dataDate() const;

    /**
     * Set the date the data is meant for.
     */
    void setDataDate( const QDate& date );

    /**
     * Checks, if the data item has a valid data date.
     */
    bool hasValidDataDate() const;
    
    WeatherData::WeatherCondition condition() const;
    void setCondition( WeatherData::WeatherCondition condition );
    bool hasValidCondition() const;
    QString conditionString() const;
    
    /**
     * Get the icon showing the current condition.
     */
    QImage icon() const;

    QString iconSource() const;
    
    WeatherData::WindDirection windDirection() const;
    void setWindDirection( WeatherData::WindDirection direction );
    bool hasValidWindDirection() const;
    QString windDirectionString() const;
    
    qreal windSpeed( WeatherData::SpeedUnit format = WeatherData::mps ) const;
    void setWindSpeed( qreal speed, WeatherData::SpeedUnit format = WeatherData::mps );
    bool hasValidWindSpeed() const;
    QString windSpeedString( WeatherData::SpeedUnit unit = WeatherData::kph ) const;
    
    qreal temperature( WeatherData::TemperatureUnit format = WeatherData::Kelvin ) const;
    void setTemperature( qreal temp,
                         WeatherData::TemperatureUnit format = WeatherData::Kelvin );
    bool hasValidTemperature() const;
    QString temperatureString( WeatherData::TemperatureUnit format = WeatherData::Kelvin ) const;
    
    qreal maxTemperature( WeatherData::TemperatureUnit format = WeatherData::Kelvin ) const;
    void setMaxTemperature( qreal temp,
                            WeatherData::TemperatureUnit format = WeatherData::Kelvin );
    bool hasValidMaxTemperature() const;
    QString maxTemperatureString( WeatherData::TemperatureUnit format = WeatherData::Kelvin ) const;
    
    qreal minTemperature( WeatherData::TemperatureUnit format = WeatherData::Kelvin ) const;
    void setMinTemperature( qreal temp,
                            WeatherData::TemperatureUnit format = WeatherData::Kelvin );
    bool hasValidMinTemperature() const;
    QString minTemperatureString( WeatherData::TemperatureUnit format = WeatherData::Kelvin ) const;
    
    WeatherData::Visibility visibility() const;
    void setVisibilty( WeatherData::Visibility visibility );
    bool hasValidVisibility() const;
    
    qreal pressure( WeatherData::PressureUnit format = WeatherData::HectoPascal ) const;
    void setPressure( qreal pressure,
                      WeatherData::PressureUnit format = WeatherData::HectoPascal );
    bool hasValidPressure() const;
    QString pressureString( WeatherData::PressureUnit format = WeatherData::HectoPascal ) const;

    WeatherData::PressureDevelopment pressureDevelopment() const;
    void setPressureDevelopment( WeatherData::PressureDevelopment );
    bool hasValidPressureDevelopment() const;
    QString pressureDevelopmentString() const;
    
    qreal humidity() const;
    void setHumidity( qreal humidity );
    bool hasValidHumidity() const;
    QString humidityString() const;

    QString toHtml( WeatherData::TemperatureUnit temperatureUnit,
                    WeatherData::SpeedUnit speedUnit,
                    WeatherData::PressureUnit pressureUnit ) const;
    
    WeatherData& operator=( const WeatherData &other );
    
    void detach();
 private:
    WeatherDataPrivate *d;
};

} // namespace Marble

#endif // WEATHERDATA_H
