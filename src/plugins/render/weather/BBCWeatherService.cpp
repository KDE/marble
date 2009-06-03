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
#include "BBCWeatherService.h"

// Marble
#include "BBCWeatherItem.h"
#include "GeoDataCoordinates.h"
#include "WeatherData.h"
#include "WeatherModel.h"
#include "global.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QUrl>

using namespace Marble;

BBCWeatherService::BBCWeatherService( QObject *parent ) 
    : AbstractWeatherService( parent )
{
}

BBCWeatherService::~BBCWeatherService() {
}
    
void BBCWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                            MarbleDataFacade *facade,
                                            qint32 number )
{
    Q_UNUSED( box );
    Q_UNUSED( number );
    Q_UNUSED( facade );
    
    BBCWeatherItem *item = new BBCWeatherItem();
    item->setStationName( "Belfast" );
    item->setPriority( 0 );
    item->setCoordinate( GeoDataCoordinates( -6.217, 54.65, 0, GeoDataCoordinates::Degree ) );
    item->setTarget( "earth" );
    item->setBbcId( 1 );
    
    emit requestedDownload( item->observationUrl(), "bbcobservation", item );
}

#include "BBCWeatherService.moc"
