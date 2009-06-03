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

// Qt
#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QRegExp>

using namespace Marble;

QHash<QString, WeatherData::WeatherCondition> BBCParser::dayConditions = QHash<QString, WeatherData::WeatherCondition>();
QHash<QString, WeatherData::WeatherCondition> BBCParser::nightConditions = QHash<QString, WeatherData::WeatherCondition>();

BBCParser::BBCParser()
{
    BBCParser::setupWeatherConditions();
}

QList<WeatherData> BBCParser::read( QIODevice *device ) {
    m_list.clear();
    setDevice( device );

    while ( !atEnd() ) {
        readNext();

        if ( isStartElement() ) {
            if ( name() == "rss" )
                readBBC();
            else
                raiseError( "The file is not an valid BBC answer." );
        }
    }

    return m_list;
}

void BBCParser::readUnknownElement() {
    Q_ASSERT( isStartElement() );

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isStartElement() )
            readUnknownElement();
    }
}

void BBCParser::readBBC() {
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

void BBCParser::readChannel() {
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

void BBCParser::readItem() {
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
            else
                readUnknownElement();
        }
    }
    
    m_list.append( item );
}

void BBCParser::readDescription( WeatherData *data ) {
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
            regExp.setPattern( "(Temperature:\\s*)(\\d+)(.C)" );
            int pos = regExp.indexIn( description );
            if ( pos > -1 ) {
                QString value = regExp.cap( 2 );
                data->setTemperature( value.toDouble(), WeatherData::Celsius );
            }
        }
    }
}

void BBCParser::readTitle( WeatherData *data ) {
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
            regExp.setPattern( "(\\D:\\s*)([\\w ]+)(\\.\\s*)" );
            int pos = regExp.indexIn( title );
            if ( pos > -1 ) {
                QString value = regExp.cap( 2 );
                
                if( dayConditions.contains( value ) ) {
                    // TODO: Switch for day/night
                    data->setCondition( dayConditions.value( value ) );
                }
                else {
                    qDebug() << "UNHANDLED BBC WEATHER CONDITION, PLEASE REPORT: " << value;
                }
            }
        }
    }
}

void BBCParser::setupWeatherConditions() {
    if( !( ( dayConditions.size() == 0 ) || ( nightConditions.size() == 0 ) ) ) {
        return;
    }
    
    dayConditions["sunny"] = WeatherData::ClearDay;
    dayConditions["clear"] = WeatherData::ClearDay;
    dayConditions["sunny intervals"] = WeatherData::FewCloudsDay;
    dayConditions["partly cloudy"] = WeatherData::PartlyCloudyDay;
    dayConditions["white cloud"] = WeatherData::Overcast;
    dayConditions["cloudy"] = WeatherData::Overcast;
    dayConditions["drizzle"] = WeatherData::LightRain;
    dayConditions["misty"] = WeatherData::Mist;
    dayConditions["mist"] = WeatherData::Mist;
    dayConditions["fog"] = WeatherData::Mist;
    dayConditions["foggy"] = WeatherData::Mist;
    dayConditions["tropical storm"] = WeatherData::Thunderstorm;
    dayConditions["hazy"] = WeatherData::Mist;
    dayConditions["light shower"] = WeatherData::LightShowersDay;
    dayConditions["light rain shower"] = WeatherData::LightShowersDay;
    dayConditions["light showers"] = WeatherData::LightShowersDay;
    dayConditions["light rain"] = WeatherData::ShowersDay;
    dayConditions["heavy rain"] = WeatherData::Rain;
    dayConditions["heavy showers"] = WeatherData::Rain;
    dayConditions["heavy shower"] = WeatherData::Rain;
    dayConditions["thundery shower"] = WeatherData::Thunderstorm;
    dayConditions["thunderstorm"] = WeatherData::Thunderstorm;
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
    dayConditions["na"] = WeatherData::ConditionNotAvailable;
    
    nightConditions["sunny"] = WeatherData::ClearNight;
    nightConditions["clear"] = WeatherData::ClearNight;
    nightConditions["sunny intervals"] = WeatherData::FewCloudsNight;
    nightConditions["partly cloudy"] = WeatherData::PartlyCloudyNight;
    nightConditions["white cloud"] = WeatherData::Overcast;
    nightConditions["cloudy"] = WeatherData::Overcast;
    nightConditions["drizzle"] = WeatherData::LightRain;
    nightConditions["misty"] = WeatherData::Mist;
    nightConditions["mist"] = WeatherData::Mist;
    nightConditions["fog"] = WeatherData::Mist;
    nightConditions["foggy"] = WeatherData::Mist;
    nightConditions["tropical storm"] = WeatherData::Thunderstorm;
    nightConditions["hazy"] = WeatherData::Mist;
    nightConditions["light shower"] = WeatherData::LightShowersNight;
    nightConditions["light rain shower"] = WeatherData::LightShowersNight;
    nightConditions["light showers"] = WeatherData::LightShowersNight;
    nightConditions["light rain"] = WeatherData::ShowersNight;
    nightConditions["heavy rain"] = WeatherData::Rain;
    nightConditions["heavy showers"] = WeatherData::Rain;
    nightConditions["heavy shower"] = WeatherData::Rain;
    nightConditions["thundery shower"] = WeatherData::Thunderstorm;
    nightConditions["thunderstorm"] = WeatherData::Thunderstorm;
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
    nightConditions["na"] = WeatherData::ConditionNotAvailable;
}
