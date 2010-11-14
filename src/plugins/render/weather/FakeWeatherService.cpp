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
#include "FakeWeatherService.h"

// Marble
#include "FakeWeatherItem.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "WeatherData.h"
#include "WeatherModel.h"

using namespace Marble;

FakeWeatherService::FakeWeatherService( QObject *parent ) 
    : AbstractWeatherService( parent )
{
}

FakeWeatherService::~FakeWeatherService()
{
}
    
void FakeWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                         MarbleDataFacade *facade,
                         qint32 number )
{
    Q_UNUSED( box );
    Q_UNUSED( number );
    Q_UNUSED( facade );
    
    FakeWeatherItem *item = new FakeWeatherItem( this );
    item->setStationName( "Fake" );
    item->setPriority( 0 );
    item->setCoordinate( GeoDataCoordinates( 1, 1 ) );
    item->setTarget( "earth" );
    item->setId( "fake1" );
    
    WeatherData data;
    data.setCondition( WeatherData::ClearDay );
    data.setTemperature( 14.0, WeatherData::Celsius );
    item->setCurrentWeather( data );
        
    emit createdItem( item );
}

#include "FakeWeatherService.moc"
