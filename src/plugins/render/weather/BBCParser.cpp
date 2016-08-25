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
#include "BBCParser.h"

// Marble
#include "MarbleGlobal.h"
#include "WeatherData.h"
#include "BBCWeatherItem.h"
#include "MarbleDebug.h"

// Qt
#include <QDateTime>
#include <QFile>
#include <QMutexLocker>
#include <QRegExp>

using namespace Marble;

BBCParser::BBCParser( QObject *parent ) :
    AbstractWorkerThread( parent ),
    m_dayConditions(),
    m_nightConditions(),
    m_windDirections(),
    m_pressureDevelopments(),
    m_visibilityStates(),
    m_monthNames()
{
    m_dayConditions["sunny"] = WeatherData::ClearDay;
    m_dayConditions["clear"] = WeatherData::ClearDay;
    m_dayConditions["clear sky"] = WeatherData::ClearDay;
    m_dayConditions["sunny intervals"] = WeatherData::FewCloudsDay;
    m_dayConditions["partly cloudy"] = WeatherData::PartlyCloudyDay;
    m_dayConditions["white cloud"] = WeatherData::Overcast;
    m_dayConditions["grey cloud"] = WeatherData::Overcast;
    m_dayConditions["cloudy"] = WeatherData::Overcast;
    m_dayConditions["drizzle"] = WeatherData::LightRain;
    m_dayConditions["misty"] = WeatherData::Mist;
    m_dayConditions["mist"] = WeatherData::Mist;
    m_dayConditions["fog"] = WeatherData::Mist;
    m_dayConditions["foggy"] = WeatherData::Mist;
    m_dayConditions["dense fog"] = WeatherData::Mist;
    m_dayConditions["Thick Fog"] = WeatherData::Mist;
    m_dayConditions["tropical storm"] = WeatherData::Thunderstorm;
    m_dayConditions["hazy"] = WeatherData::Mist;
    m_dayConditions["light shower"] = WeatherData::LightShowersDay;
    m_dayConditions["light rain shower"] = WeatherData::LightShowersDay;
    m_dayConditions["light showers"] = WeatherData::LightShowersDay;
    m_dayConditions["light rain"] = WeatherData::ShowersDay;
    m_dayConditions["heavy rain"] = WeatherData::Rain;
    m_dayConditions["heavy showers"] = WeatherData::Rain;
    m_dayConditions["heavy shower"] = WeatherData::Rain;
    m_dayConditions["heavy rain shower"] = WeatherData::Rain;
    m_dayConditions["thundery shower"] = WeatherData::Thunderstorm;
    m_dayConditions["thunderstorm"] = WeatherData::Thunderstorm;
    m_dayConditions["thunder storm"] = WeatherData::Thunderstorm;
    m_dayConditions["cloudy with sleet"] = WeatherData::RainSnow;
    m_dayConditions["sleet shower"] = WeatherData::RainSnow;
    m_dayConditions["sleet showers"] = WeatherData::RainSnow;
    m_dayConditions["sleet"] = WeatherData::RainSnow;
    m_dayConditions["cloudy with hail"] = WeatherData::Hail;
    m_dayConditions["hail shower"] = WeatherData::Hail;
    m_dayConditions["hail showers"] = WeatherData::Hail;
    m_dayConditions["hail"] = WeatherData::Hail;
    m_dayConditions["light snow"] = WeatherData::LightSnow;
    m_dayConditions["light snow shower"] = WeatherData::ChanceSnowDay;
    m_dayConditions["light snow showers"] = WeatherData::ChanceSnowDay;
    m_dayConditions["cloudy with light snow"] = WeatherData::LightSnow;
    m_dayConditions["heavy snow"] = WeatherData::Snow;
    m_dayConditions["heavy snow shower"] = WeatherData::Snow;
    m_dayConditions["heavy snow showers"] = WeatherData::Snow;
    m_dayConditions["cloudy with heavy snow"] = WeatherData::Snow;
    m_dayConditions["sandstorm"] = WeatherData::SandStorm;
    m_dayConditions["na"] = WeatherData::ConditionNotAvailable;
    m_dayConditions["N/A"] = WeatherData::ConditionNotAvailable;

    m_nightConditions["sunny"] = WeatherData::ClearNight;
    m_nightConditions["clear"] = WeatherData::ClearNight;
    m_nightConditions["clear sky"] = WeatherData::ClearNight;
    m_nightConditions["sunny intervals"] = WeatherData::FewCloudsNight;
    m_nightConditions["partly cloudy"] = WeatherData::PartlyCloudyNight;
    m_nightConditions["white cloud"] = WeatherData::Overcast;
    m_nightConditions["grey cloud"] = WeatherData::Overcast;
    m_nightConditions["cloudy"] = WeatherData::Overcast;
    m_nightConditions["drizzle"] = WeatherData::LightRain;
    m_nightConditions["misty"] = WeatherData::Mist;
    m_nightConditions["mist"] = WeatherData::Mist;
    m_nightConditions["fog"] = WeatherData::Mist;
    m_nightConditions["foggy"] = WeatherData::Mist;
    m_nightConditions["dense fog"] = WeatherData::Mist;
    m_nightConditions["Thick Fog"] = WeatherData::Mist;
    m_nightConditions["tropical storm"] = WeatherData::Thunderstorm;
    m_nightConditions["hazy"] = WeatherData::Mist;
    m_nightConditions["light shower"] = WeatherData::LightShowersNight;
    m_nightConditions["light rain shower"] = WeatherData::LightShowersNight;
    m_nightConditions["light showers"] = WeatherData::LightShowersNight;
    m_nightConditions["light rain"] = WeatherData::ShowersNight;
    m_nightConditions["heavy rain"] = WeatherData::Rain;
    m_nightConditions["heavy showers"] = WeatherData::Rain;
    m_nightConditions["heavy shower"] = WeatherData::Rain;
    m_nightConditions["heavy rain shower"] = WeatherData::Rain;
    m_nightConditions["thundery shower"] = WeatherData::Thunderstorm;
    m_nightConditions["thunderstorm"] = WeatherData::Thunderstorm;
    m_nightConditions["thunder storm"] = WeatherData::Thunderstorm;
    m_nightConditions["cloudy with sleet"] = WeatherData::RainSnow;
    m_nightConditions["sleet shower"] = WeatherData::RainSnow;
    m_nightConditions["sleet showers"] = WeatherData::RainSnow;
    m_nightConditions["sleet"] = WeatherData::RainSnow;
    m_nightConditions["cloudy with hail"] = WeatherData::Hail;
    m_nightConditions["hail shower"] = WeatherData::Hail;
    m_nightConditions["hail showers"] = WeatherData::Hail;
    m_nightConditions["hail"] = WeatherData::Hail;
    m_nightConditions["light snow"] = WeatherData::LightSnow;
    m_nightConditions["light snow shower"] = WeatherData::ChanceSnowNight;
    m_nightConditions["light snow showers"] = WeatherData::ChanceSnowNight;
    m_nightConditions["cloudy with light snow"] = WeatherData::LightSnow;
    m_nightConditions["heavy snow"] = WeatherData::Snow;
    m_nightConditions["heavy snow shower"] = WeatherData::Snow;
    m_nightConditions["heavy snow showers"] = WeatherData::Snow;
    m_nightConditions["cloudy with heavy snow"] = WeatherData::Snow;
    m_nightConditions["sandstorm"] = WeatherData::SandStorm;
    m_nightConditions["na"] = WeatherData::ConditionNotAvailable;
    m_nightConditions["N/A"] = WeatherData::ConditionNotAvailable;

    m_windDirections["N"] = WeatherData::N;
    m_windDirections["NE"] = WeatherData::NE;
    m_windDirections["ENE"] = WeatherData::ENE;
    m_windDirections["NNE"] = WeatherData::NNE;
    m_windDirections["E"] = WeatherData::E;
    m_windDirections["SSE"] = WeatherData::SSE;
    m_windDirections["SE"] = WeatherData::SE;
    m_windDirections["ESE"] = WeatherData::ESE;
    m_windDirections["S"] = WeatherData::S;
    m_windDirections["NNW"] = WeatherData::NNW;
    m_windDirections["NW"] = WeatherData::NW;
    m_windDirections["WNW"] = WeatherData::WNW;
    m_windDirections["W"] = WeatherData::W;
    m_windDirections["SSW"] = WeatherData::SSW;
    m_windDirections["SW"] = WeatherData::SW;
    m_windDirections["WSW"] = WeatherData::WSW;
    m_windDirections["N/A"] = WeatherData::DirectionNotAvailable;

    m_pressureDevelopments["falling"] = WeatherData::Falling;
    m_pressureDevelopments["no change"] = WeatherData::NoChange;
    m_pressureDevelopments["steady"] = WeatherData::NoChange;
    m_pressureDevelopments["rising"] = WeatherData::Rising;
    m_pressureDevelopments["N/A"] = WeatherData::PressureDevelopmentNotAvailable;

    m_visibilityStates["excellent"] = WeatherData::VeryGood;
    m_visibilityStates["very good"] = WeatherData::VeryGood;
    m_visibilityStates["good"] = WeatherData::Good;
    m_visibilityStates["moderate"] = WeatherData::Normal;
    m_visibilityStates["poor"] = WeatherData::Poor;
    m_visibilityStates["very poor"] = WeatherData::VeryPoor;
    m_visibilityStates["fog"] = WeatherData::Fog;
    m_visibilityStates["n/a"] = WeatherData::VisibilityNotAvailable;

    m_monthNames["Jan"] = 1;
    m_monthNames["Feb"] = 2;
    m_monthNames["Mar"] = 3;
    m_monthNames["Apr"] = 4;
    m_monthNames["May"] = 5;
    m_monthNames["Jun"] = 6;
    m_monthNames["Jul"] = 7;
    m_monthNames["Aug"] = 8;
    m_monthNames["Sep"] = 9;
    m_monthNames["Oct"] = 10;
    m_monthNames["Nov"] = 11;
    m_monthNames["Dec"] = 12;
}

BBCParser::~BBCParser()
{
}

BBCParser *BBCParser::instance()
{
    static BBCParser parser;
    return &parser;
}

void BBCParser::scheduleRead( const QString& path,
                              BBCWeatherItem *item,
                              const QString& type )
{
    ScheduleEntry entry;
    entry.path = path;
    entry.item = item;
    entry.type = type;

    m_scheduleMutex.lock();
    m_schedule.push( entry );
    m_scheduleMutex.unlock();

    ensureRunning();
}

bool BBCParser::workAvailable()
{
    QMutexLocker locker( &m_scheduleMutex );
    return !m_schedule.isEmpty();
}

void BBCParser::work()
{
    m_scheduleMutex.lock();
    ScheduleEntry entry = m_schedule.pop();
    m_scheduleMutex.unlock();

    QFile file( entry.path );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        return;
    }

    QList<WeatherData> data = read( &file );

    if( !data.isEmpty() && !entry.item.isNull() ) {
        if (entry.type == QLatin1String("bbcobservation")) {
            entry.item->setCurrentWeather( data.at( 0 ) );
        }
        else if (entry.type == QLatin1String("bbcforecast")) {
            entry.item->addForecastWeather( data );
        }

        emit parsedFile();
    }
}

QList<WeatherData> BBCParser::read( QIODevice *device )
{
    m_list.clear();
    setDevice( device );

    while ( !atEnd() ) {
        readNext();

        if ( isStartElement() ) {
            if (name() == QLatin1String("rss"))
                readBBC();
            else
                raiseError( QObject::tr("The file is not a valid BBC answer.") );
        }
    }

    return m_list;
}

void BBCParser::readUnknownElement()
{
    Q_ASSERT( isStartElement() );

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isStartElement() )
            readUnknownElement();
    }
}

void BBCParser::readBBC()
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("rss"));
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if (name() == QLatin1String("channel"))
                readChannel();
            else
                readUnknownElement();
        }
    }
}

void BBCParser::readChannel()
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("channel"));

    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if (name() == QLatin1String("item"))
                readItem();
            else
                readUnknownElement();
        }
    }
}

void BBCParser::readItem()
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("item"));
    
    WeatherData item;
    
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if (name() == QLatin1String("description"))
                readDescription( &item );
            else if(name() == QLatin1String("title"))
                readTitle( &item );
            else if (name() == QLatin1String("pubDate"))
                readPubDate( &item );
            else
                readUnknownElement();
        }
    }
    
    m_list.append( item );
}

void BBCParser::readDescription( WeatherData *data )
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("description"));
        
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            readUnknownElement();
        }
        
        if( isCharacters() ) {
            QString description = text().toString();
            QRegExp regExp;
            
            // Temperature
            regExp.setPattern( "(Temperature:\\s*)(-?\\d+)(.C)" );
            int pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString value = regExp.cap( 2 );
                data->setTemperature( value.toDouble(), WeatherData::Celsius );
            }

            // Max Temperature
            regExp.setPattern( "(Max Temp:\\s*)(-?\\d+)(.C)" );
            pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString value = regExp.cap( 2 );
                data->setMaxTemperature( value.toDouble(), WeatherData::Celsius );
            }

            // Min Temperature
            regExp.setPattern( "(Min Temp:\\s*)(-?\\d+)(.C)" );
            pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString value = regExp.cap( 2 );
                data->setMinTemperature( value.toDouble(), WeatherData::Celsius );
            }

            // Wind direction
            regExp.setPattern( "(Wind Direction:\\s*)([NESW]+)(,)" );
            pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString wind = regExp.cap( 2 );

                if ( m_windDirections.contains( wind ) ) {
                    data->setWindDirection( m_windDirections.value( wind ) );
                }
                else {
                    mDebug() << "UNHANDLED WIND DIRECTION, PLEASE REPORT: " << wind;
                }
            }

            // Wind speed
            regExp.setPattern( "(Wind Speed:\\s*)(\\d+)(mph)" );
            pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString speed = regExp.cap( 2 );
                data->setWindSpeed( speed.toFloat(), WeatherData::mph );
            }

            // Relative Humidity
            regExp.setPattern( "(Relative Humidity:\\s*)(\\d+)(.,)" );
            pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString humidity = regExp.cap( 2 );
                data->setHumidity( humidity.toFloat() );
            }

            // Pressure
            regExp.setPattern( "(Pressure:\\s*)(\\d+mB|N/A)(, )([a-z ]+|N/A)(,)" );
            pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString pressure = regExp.cap( 2 );
                if (pressure != QLatin1String("N/A")) {
                    pressure.chop( 2 );
                    data->setPressure( pressure.toFloat()/1000, WeatherData::Bar );
                }

                QString pressureDevelopment = regExp.cap( 4 );

                if ( m_pressureDevelopments.contains( pressureDevelopment ) ) {
                    data->setPressureDevelopment( m_pressureDevelopments.value( pressureDevelopment ) );
                }
                else {
                    mDebug() << "UNHANDLED PRESSURE DEVELOPMENT, PLEASE REPORT: "
                             << pressureDevelopment;
                }
            }

            // Visibility
            regExp.setPattern( "(Visibility:\\s*)([^,]+)" );
            pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString visibility = regExp.cap( 2 );

                if ( m_visibilityStates.contains( visibility.toLower() ) ) {
                    data->setVisibilty( m_visibilityStates.value( visibility ) );
                }
                else {
                    mDebug() << "UNHANDLED VISIBILITY, PLEASE REPORT: " << visibility;
                }
            }
        }
    }
}

void BBCParser::readTitle( WeatherData *data )
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("title"));
    
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            readUnknownElement();
        }
        
        if( isCharacters() ) {
            QString title = text().toString();
            QRegExp regExp;
            
            // Condition
            regExp.setPattern( "(^.*)(:\\s*)([\\w ]+)([\\,\\.]\\s*)" );
            int pos = regExp.indexIn( title );
            if ( pos > -1 ) {
                QString value = regExp.cap( 3 );
                
                if( m_dayConditions.contains( value ) ) {
                    // TODO: Switch for day/night
                    data->setCondition( m_dayConditions.value( value ) );
                }
                else {
                    mDebug() << "UNHANDLED BBC WEATHER CONDITION, PLEASE REPORT: " << value;
                }

                QString dayString = regExp.cap( 1 );
                Qt::DayOfWeek dayOfWeek = (Qt::DayOfWeek) 0;
                if (dayString.contains(QLatin1String("Monday"))) {
                    dayOfWeek = Qt::Monday;
                } else if (dayString.contains(QLatin1String("Tuesday"))) {
                    dayOfWeek = Qt::Tuesday;
                } else if (dayString.contains(QLatin1String("Wednesday"))) {
                    dayOfWeek = Qt::Wednesday;
                } else if (dayString.contains(QLatin1String("Thursday"))) {
                    dayOfWeek = Qt::Thursday;
                } else if (dayString.contains(QLatin1String("Friday"))) {
                    dayOfWeek = Qt::Friday;
                } else if (dayString.contains(QLatin1String("Saturday"))) {
                    dayOfWeek = Qt::Saturday;
                } else if (dayString.contains(QLatin1String("Sunday"))) {
                    dayOfWeek = Qt::Sunday;
                }
                QDate date = QDate::currentDate();
                date = date.addDays( -1 );

                for ( int i = 0; i < 7; i++ ) {
                    if ( date.dayOfWeek() == dayOfWeek ) {
                        data->setDataDate( date );
                    }
                    date = date.addDays( 1 );
                }
            }
        }
    }
}

void BBCParser::readPubDate( WeatherData *data )
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("pubDate"));

    while( !atEnd() ) {
        readNext();

        if( isEndElement() )
            break;

        if( isStartElement() ) {
            readUnknownElement();
        }

        if( isCharacters() ) {
            QString pubDate = text().toString();
            QRegExp regExp;

            regExp.setPattern( "([A-Za-z]+,\\s+)(\\d+)(\\s+)([A-Za-z]+)(\\s+)(\\d{4,4})(\\s+)(\\d+)(:)(\\d+)(:)(\\d+)(\\s+)([+-])(\\d{2,2})(\\d{2,2})" );
            int pos = regExp.indexIn( pubDate );
            if ( pos > -1 ) {
                QDateTime dateTime;
                QDate date;
                QTime time;

                dateTime.setTimeSpec( Qt::UTC );
                date.setDate( regExp.cap( 6 ).toInt(),
                             m_monthNames.value( regExp.cap( 4 ) ),
                             regExp.cap( 2 ).toInt() );
                time.setHMS( regExp.cap( 8 ).toInt(),
                             regExp.cap( 10 ).toInt(),
                             regExp.cap( 12 ).toInt() );

                dateTime.setDate( date );
                dateTime.setTime( time );

                // Timezone
                if (regExp.cap(14) == QLatin1String("-")) {
                    dateTime = dateTime.addSecs( 60*60*regExp.cap( 15 ).toInt() );
                    dateTime = dateTime.addSecs( 60   *regExp.cap( 16 ).toInt() );
                }
                else {
                    dateTime = dateTime.addSecs( -60*60*regExp.cap( 15 ).toInt() );
                    dateTime = dateTime.addSecs( -60   *regExp.cap( 16 ).toInt() );
                }

                data->setPublishingTime( dateTime );
            }
        }
    }
}

#include "moc_BBCParser.cpp"
