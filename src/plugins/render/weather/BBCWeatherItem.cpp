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
#include "BBCWeatherItem.h"

// Marble
#include "BBCParser.h"
#include "WeatherData.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>

using namespace Marble;

BBCWeatherItem::BBCWeatherItem( QObject *parent )
    : WeatherItem( parent )
{
}

BBCWeatherItem::~BBCWeatherItem() {
}

QString BBCWeatherItem::service() const {
    return QString( "BBC" );
}

void BBCWeatherItem::addDownloadedFile( const QString& url, const QString& type ) {
    if( type == "bbcobservation" ) {
        QFile file( url );
        if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            return;
        }
        
        BBCParser parser;
        QList<WeatherData> data = parser.read( &file );
        if( !data.isEmpty() ) {
            setCurrentWeather( data.at( 0 ) );
        }
    }
}

quint32 BBCWeatherItem::bbcId() const {
    return m_bbcId;
}

void BBCWeatherItem::setBbcId( quint32 id ) {
    m_bbcId = id;
}

QUrl BBCWeatherItem::observationUrl() const {
    return QUrl( QString( "http://newsrss.bbc.co.uk/weather/forecast/%1/ObservationsRSS.xml" )
                    .arg( QString::number( bbcId() ) ) );
}
