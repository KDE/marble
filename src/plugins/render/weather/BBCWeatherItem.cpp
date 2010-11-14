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
#include "BBCWeatherItem.h"

// Marble
#include "BBCParser.h"
#include "WeatherData.h"
#include "MarbleDebug.h"

// Qt
#include <QtCore/QFile>
#include <QtCore/QTime>
#include <QtCore/QUrl>

using namespace Marble;

BBCWeatherItem::BBCWeatherItem( QObject *parent )
    : WeatherItem( parent ),
      m_observationRequested( false ),
      m_forecastRequested( false )
{
}

BBCWeatherItem::~BBCWeatherItem()
{
}

bool BBCWeatherItem::request( const QString& type )
{
    if ( type == "bbcobservation" ) {
        if ( !m_observationRequested ) {
            m_observationRequested = true;
            return true;
        }
    }
    else if ( type == "bbcforecast" ) {
        if ( !m_forecastRequested ) {
            m_forecastRequested = true;
            return true;
        }
    }
    return false;
}

QString BBCWeatherItem::service() const
{
    return QString( "BBC" );
}

void BBCWeatherItem::addDownloadedFile( const QString& url, const QString& type )
{
    if( type == "bbcobservation" || type == "bbcforecast" ) {
        BBCParser::instance()->scheduleRead( url, this, type );
    }
}

quint32 BBCWeatherItem::bbcId() const
{
    return m_bbcId;
}

void BBCWeatherItem::setBbcId( quint32 id )
{
    m_bbcId = id;
    setId( QString( "bbc" ) + QString::number( id ) );
}

QUrl BBCWeatherItem::observationUrl() const
{
    return QUrl( QString( "http://newsrss.bbc.co.uk/weather/forecast/%1/ObservationsRSS.xml" )
                    .arg( QString::number( bbcId() ) ) );
}

QUrl BBCWeatherItem::forecastUrl() const
{
    return QUrl( QString( "http://newsrss.bbc.co.uk/weather/forecast/%1/Next3DaysRSS.xml" )
                    .arg( QString::number( bbcId() ) ) );
}

QString BBCWeatherItem::creditHtml() const
{
    return tr( "Supported by <a href=\"http://backstage.bbc.co.uk\" target=\"_BLANK\">backstage.bbc.co.uk</a>.<br>Weather data from UK MET Office" );
}
