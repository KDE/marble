//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include<QtCore/QtGlobal>

class QDateTime;
class QIcon;

namespace Marble {
    
class WeatherDataPrivate;

class WeatherData {
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
        Mist
//         FreezingDrizzle, would be nice, but no icon for the moment
//         Flurries, would be nice, but no icon available
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
        VisibilityNotAvailable
    };
    
    enum SpeedFormat {
        kph,
        mph,
        mps,
        knots,
        beaufort
    };
    
    enum TemperatureFormat {
        Celsius,
        Fahrenheit,
        Kelvin
    };
    
    enum PressureFormat {
        HectoPascal,
        KiloPascal,
        Bar,
        mmHg
    };
    
    WeatherData();
    WeatherData( const WeatherData &other );
    ~WeatherData();
    
    QDateTime dateTime() const;
    void setDateTime( const QDateTime& dateTime );
    
    WeatherData::WeatherCondition condition() const;
    void setCondition( WeatherData::WeatherCondition condition );
    
    /**
     * Get the icon showing the current condition.
     */
    QIcon icon() const;
    
    WeatherData::WindDirection windDirection() const;
    void setWindDirection( WeatherData::WindDirection direction );
    
    qreal windSpeed( WeatherData::SpeedFormat format = WeatherData::mps ) const;
    void setWindSpeed( qreal speed, WeatherData::SpeedFormat format = WeatherData::mps );
    
    qreal temperature( WeatherData::TemperatureFormat format = WeatherData::Kelvin ) const;
    void setTemperature( qreal temp,
                         WeatherData::TemperatureFormat format = WeatherData::Kelvin );
    QString temperatureString() const;
    
    qreal maxTemperature( WeatherData::TemperatureFormat format = WeatherData::Kelvin ) const;
    void setMaxTemperature( qreal temp,
                            WeatherData::TemperatureFormat format = WeatherData::Kelvin );
    QString maxTemperatureString() const;
    
    qreal minTemperature( WeatherData::TemperatureFormat format = WeatherData::Kelvin ) const;
    void setMinTemperature( qreal temp,
                            WeatherData::TemperatureFormat format = WeatherData::Kelvin );
    QString minTemperatureString() const;
    
    WeatherData::Visibility visibility() const;
    void setVisibilty( WeatherData::Visibility visibility );
    
    qreal pressure( WeatherData::PressureFormat format = WeatherData::HectoPascal ) const;
    void setPressure( qreal pressure,
                      WeatherData::PressureFormat format = WeatherData::HectoPascal );

    WeatherData::PressureDevelopment pressureDevelopment() const;
    void setPressureDevelopment( WeatherData::PressureDevelopment );
    
    qreal humidity() const;
    void setHumidity( qreal humidity );
    
    WeatherData& operator=( const WeatherData &other );
    
    void detach();
 private:
    WeatherDataPrivate *d;
};

} // namespace Marble

#endif // WEATHERDATA_H
