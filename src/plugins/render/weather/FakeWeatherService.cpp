//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "FakeWeatherService.h"

// Marble
#include "FakeWeatherItem.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "WeatherData.h"
#include "WeatherModel.h"

using namespace Marble;

FakeWeatherService::FakeWeatherService( const MarbleModel *model, QObject *parent )
    : AbstractWeatherService( model, parent )
{
}

FakeWeatherService::~FakeWeatherService()
{
}
    
void FakeWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                         qint32 number )
{
    Q_UNUSED( box );
    Q_UNUSED( number );

    FakeWeatherItem *item = new FakeWeatherItem( this );
    item->setStationName( "Fake" );
    item->setPriority( 0 );
    item->setCoordinate( GeoDataCoordinates( 1, 1 ) );
    item->setId( "fake1" );
    
    WeatherData data;
    data.setCondition( WeatherData::ClearDay );
    data.setTemperature( 14.0, WeatherData::Celsius );
    item->setCurrentWeather( data );
        
    emit createdItems( QList<AbstractDataPluginItem*>() << item );
}

void FakeWeatherService::getItem( const QString & )
{
    getAdditionalItems( GeoDataLatLonAltBox(), 1 );
}

#include "moc_FakeWeatherService.cpp"
