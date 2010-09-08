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
#include "BBCParser.h"

// Marble
#include "global.h"
#include "WeatherData.h"
#include "BBCWeatherItem.h"
#include "MarbleDebug.h"

// Qt
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegExp>

using namespace Marble;

QHash<QString, WeatherData::WeatherCondition> BBCParser::dayConditions
        = QHash<QString, WeatherData::WeatherCondition>();
QHash<QString, WeatherData::WeatherCondition> BBCParser::nightConditions
        = QHash<QString, WeatherData::WeatherCondition>();
QHash<QString, WeatherData::WindDirection> BBCParser::windDirections
        = QHash<QString, WeatherData::WindDirection>();
QHash<QString, WeatherData::PressureDevelopment> BBCParser::pressureDevelopments
        = QHash<QString, WeatherData::PressureDevelopment>();
QHash<QString, WeatherData::Visibility> BBCParser::visibilityStates
        = QHash<QString, WeatherData::Visibility>();
QHash<QString, int> BBCParser::monthNames
        = QHash<QString, int>();

BBCParser::BBCParser( QObject *parent )
        : AbstractWorkerThread( parent )
{
    BBCParser::setupHashes();
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
        if ( entry.type == "bbcobservation" ) {
            entry.item->setCurrentWeather( data.at( 0 ) );
        }
        else if ( entry.type == "bbcforecast" ) {
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
            if ( name() == "rss" )
                readBBC();
            else
                raiseError( QObject::tr("The file is not an valid BBC answer.") );
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
              && name() == "rss" );
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "channel" )
                readChannel();
            else
                readUnknownElement();
        }
    }
}

void BBCParser::readChannel()
{
    Q_ASSERT( isStartElement()
              && name() == "channel" );

    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "item" )
                readItem();
            else
                readUnknownElement();
        }
    }
}

void BBCParser::readItem()
{
    Q_ASSERT( isStartElement()
              && name() == "item" );
    
    WeatherData item;
    
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "description" )
                readDescription( &item );
            else if( name() == "title" )
                readTitle( &item );
            else if( name() == "pubDate" )
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
              && name() == "description" );
        
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

                if ( windDirections.contains( wind ) ) {
                    data->setWindDirection( windDirections.value( wind ) );
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
                if ( pressure != "N/A" ) {
                    pressure.chop( 2 );
                    data->setPressure( pressure.toFloat()/1000, WeatherData::Bar );
                }

                QString pressureDevelopment = regExp.cap( 4 );

                if ( pressureDevelopments.contains( pressureDevelopment ) ) {
                    data->setPressureDevelopment( pressureDevelopments.value( pressureDevelopment ) );
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

                if ( visibilityStates.contains( visibility.toLower() ) ) {
                    data->setVisibilty( visibilityStates.value( visibility ) );
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
              && name() == "title" );
    
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
                
                if( dayConditions.contains( value ) ) {
                    // TODO: Switch for day/night
                    data->setCondition( dayConditions.value( value ) );
                }
                else {
                    mDebug() << "UNHANDLED BBC WEATHER CONDITION, PLEASE REPORT: " << value;
                }

                QString dayString = regExp.cap( 1 );
                Qt::DayOfWeek dayOfWeek = (Qt::DayOfWeek) 0;
                if ( dayString.contains( "Monday" ) )
                    dayOfWeek = Qt::Monday;
                else if ( dayString.contains( "Tuesday" ) )
                    dayOfWeek = Qt::Tuesday;
                else if ( dayString.contains( "Wednesday" ) )
                    dayOfWeek = Qt::Wednesday;
                else if ( dayString.contains( "Thursday" ) )
                    dayOfWeek = Qt::Thursday;
                else if ( dayString.contains( "Friday" ) )
                    dayOfWeek = Qt::Friday;
                else if ( dayString.contains( "Saturday" ) )
                    dayOfWeek = Qt::Saturday;
                else if ( dayString.contains( "Sunday" ) )
                    dayOfWeek = Qt::Sunday;
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
              && name() == "pubDate" );

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
                date.setYMD( regExp.cap( 6 ).toInt(),
                             monthNames.value( regExp.cap( 4 ) ),
                             regExp.cap( 2 ).toInt() );
                time.setHMS( regExp.cap( 8 ).toInt(),
                             regExp.cap( 10 ).toInt(),
                             regExp.cap( 12 ).toInt() );

                dateTime.setDate( date );
                dateTime.setTime( time );

                // Timezone
                if( regExp.cap( 14 ) == "-" ) {
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

void BBCParser::setupHashes()
{
    if( !( ( dayConditions.isEmpty() )
           || ( nightConditions.isEmpty() )
           || ( windDirections.isEmpty() )
           || ( pressureDevelopments.isEmpty() )
           || ( visibilityStates.isEmpty() )
           || ( monthNames.isEmpty() ) ) )
    {
        return;
    }

    dayConditions["sunny"] = WeatherData::ClearDay;
    dayConditions["clear"] = WeatherData::ClearDay;
    dayConditions["clear sky"] = WeatherData::ClearDay;
    dayConditions["sunny intervals"] = WeatherData::FewCloudsDay;
    dayConditions["partly cloudy"] = WeatherData::PartlyCloudyDay;
    dayConditions["white cloud"] = WeatherData::Overcast;
    dayConditions["grey cloud"] = WeatherData::Overcast;
    dayConditions["cloudy"] = WeatherData::Overcast;
    dayConditions["drizzle"] = WeatherData::LightRain;
    dayConditions["misty"] = WeatherData::Mist;
    dayConditions["mist"] = WeatherData::Mist;
    dayConditions["fog"] = WeatherData::Mist;
    dayConditions["foggy"] = WeatherData::Mist;
    dayConditions["dense fog"] = WeatherData::Mist;
    dayConditions["Thick Fog"] = WeatherData::Mist;
    dayConditions["tropical storm"] = WeatherData::Thunderstorm;
    dayConditions["hazy"] = WeatherData::Mist;
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
    dayConditions["N/A"] = WeatherData::ConditionNotAvailable;
    
    nightConditions["sunny"] = WeatherData::ClearNight;
    nightConditions["clear"] = WeatherData::ClearNight;
    nightConditions["clear sky"] = WeatherData::ClearNight;
    nightConditions["sunny intervals"] = WeatherData::FewCloudsNight;
    nightConditions["partly cloudy"] = WeatherData::PartlyCloudyNight;
    nightConditions["white cloud"] = WeatherData::Overcast;
    nightConditions["grey cloud"] = WeatherData::Overcast;
    nightConditions["cloudy"] = WeatherData::Overcast;
    nightConditions["drizzle"] = WeatherData::LightRain;
    nightConditions["misty"] = WeatherData::Mist;
    nightConditions["mist"] = WeatherData::Mist;
    nightConditions["fog"] = WeatherData::Mist;
    nightConditions["foggy"] = WeatherData::Mist;
    nightConditions["dense fog"] = WeatherData::Mist;
    nightConditions["Thick Fog"] = WeatherData::Mist;
    nightConditions["tropical storm"] = WeatherData::Thunderstorm;
    nightConditions["hazy"] = WeatherData::Mist;
    nightConditions["light shower"] = WeatherData::LightShowersNight;
    nightConditions["light rain shower"] = WeatherData::LightShowersNight;
    nightConditions["light showers"] = WeatherData::LightShowersNight;
    nightConditions["light rain"] = WeatherData::ShowersNight;
    nightConditions["heavy rain"] = WeatherData::Rain;
    nightConditions["heavy showers"] = WeatherData::Rain;
    nightConditions["heavy shower"] = WeatherData::Rain;
    nightConditions["heavy rain shower"] = WeatherData::Rain;
    nightConditions["thundery shower"] = WeatherData::Thunderstorm;
    nightConditions["thunderstorm"] = WeatherData::Thunderstorm;
    nightConditions["thunder storm"] = WeatherData::Thunderstorm;
    nightConditions["cloudy with sleet"] = WeatherData::RainSnow;
    nightConditions["sleet shower"] = WeatherData::RainSnow;
    nightConditions["sleet showers"] = WeatherData::RainSnow;
    nightConditions["sleet"] = WeatherData::RainSnow;
    nightConditions["cloudy with hail"] = WeatherData::Hail;
    nightConditions["hail shower"] = WeatherData::Hail;
    nightConditions["hail showers"] = WeatherData::Hail;
    nightConditions["hail"] = WeatherData::Hail;
    nightConditions["light snow"] = WeatherData::LightSnow;
    nightConditions["light snow shower"] = WeatherData::ChanceSnowNight;
    nightConditions["light snow showers"] = WeatherData::ChanceSnowNight;
    nightConditions["cloudy with light snow"] = WeatherData::LightSnow;
    nightConditions["heavy snow"] = WeatherData::Snow;
    nightConditions["heavy snow shower"] = WeatherData::Snow;
    nightConditions["heavy snow showers"] = WeatherData::Snow;
    nightConditions["cloudy with heavy snow"] = WeatherData::Snow;
    nightConditions["sandstorm"] = WeatherData::SandStorm;
    nightConditions["na"] = WeatherData::ConditionNotAvailable;
    nightConditions["N/A"] = WeatherData::ConditionNotAvailable;

    windDirections["N"] = WeatherData::N;
    windDirections["NE"] = WeatherData::NE;
    windDirections["ENE"] = WeatherData::ENE;
    windDirections["NNE"] = WeatherData::NNE;
    windDirections["E"] = WeatherData::E;
    windDirections["SSE"] = WeatherData::SSE;
    windDirections["SE"] = WeatherData::SE;
    windDirections["ESE"] = WeatherData::ESE;
    windDirections["S"] = WeatherData::S;
    windDirections["NNW"] = WeatherData::NNW;
    windDirections["NW"] = WeatherData::NW;
    windDirections["WNW"] = WeatherData::WNW;
    windDirections["W"] = WeatherData::W;
    windDirections["SSW"] = WeatherData::SSW;
    windDirections["SW"] = WeatherData::SW;
    windDirections["WSW"] = WeatherData::WSW;
    windDirections["N/A"] = WeatherData::DirectionNotAvailable;

    pressureDevelopments["falling"] = WeatherData::Falling;
    pressureDevelopments["no change"] = WeatherData::NoChange;
    pressureDevelopments["steady"] = WeatherData::NoChange;
    pressureDevelopments["rising"] = WeatherData::Rising;
    pressureDevelopments["N/A"] = WeatherData::PressureDevelopmentNotAvailable;

    visibilityStates["excellent"] = WeatherData::VeryGood;
    visibilityStates["very good"] = WeatherData::VeryGood;
    visibilityStates["good"] = WeatherData::Good;
    visibilityStates["moderate"] = WeatherData::Normal;
    visibilityStates["poor"] = WeatherData::Poor;
    visibilityStates["very poor"] = WeatherData::VeryPoor;
    visibilityStates["fog"] = WeatherData::Fog;
    visibilityStates["n/a"] = WeatherData::VisibilityNotAvailable;

    monthNames["Jan"] = 1;
    monthNames["Feb"] = 2;
    monthNames["Mar"] = 3;
    monthNames["Apr"] = 4;
    monthNames["May"] = 5;
    monthNames["Jun"] = 6;
    monthNames["Jul"] = 7;
    monthNames["Aug"] = 8;
    monthNames["Sep"] = 9;
    monthNames["Oct"] = 10;
    monthNames["Nov"] = 11;
    monthNames["Dec"] = 12;
}

#include "BBCParser.moc"
