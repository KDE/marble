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
#include "WeatherModel.h"

// Marble
#include "BBCWeatherService.h"
#include "FakeWeatherService.h"
#include "AbstractDataPluginItem.h"
#include "WeatherItem.h"
#include "MarbleDebug.h"

using namespace Marble;

WeatherModel::WeatherModel( PluginManager *pluginManager,
                            QObject *parent )
    : AbstractDataPluginModel( "weather", pluginManager, parent )
{
//     addService( new FakeWeatherService( this ) );
     addService( new BBCWeatherService( this ) );
}
    
WeatherModel::~WeatherModel()
{
}

void WeatherModel::downloadItemData( const QUrl& url,
                                     const QString& type,
                                     AbstractDataPluginItem *item )
{
    AbstractDataPluginItem *existingItem = findItem( item->id() );
    if ( !existingItem ) {
        WeatherItem *weatherItem = qobject_cast<WeatherItem*>( item );
        if( weatherItem ) {
            weatherItem->request( type );
        }
        AbstractDataPluginModel::downloadItemData( url, type, item );
    } else {
        if ( existingItem != item )
            item->deleteLater();
        
        WeatherItem *existingWeatherItem = qobject_cast<WeatherItem*>( existingItem );
        if( existingWeatherItem && existingWeatherItem->request( type ) ) {
            AbstractDataPluginModel::downloadItemData( url, type, existingWeatherItem );
        }
    }
}

void WeatherModel::addItemToList( AbstractDataPluginItem *item )
{
    AbstractDataPluginItem *existingItem = findItem( item->id() );
    if ( !existingItem ) {
        AbstractDataPluginModel::addItemToList( item );
    } else {
        if ( existingItem != item )
            item->deleteLater();
    }
}

void WeatherModel::getAdditionalItems( const GeoDataLatLonAltBox& box,
                               MarbleDataFacade *facade,
                               qint32 number )
{
    emit additionalItemsRequested( box, facade, number );
}

void WeatherModel::parseFile( const QByteArray& file )
{
    // We won't request any description files so we don't need to parse it
    Q_UNUSED( file );
}

void WeatherModel::addService( AbstractWeatherService *service )
{
    connect( service, SIGNAL( createdItem( AbstractDataPluginItem * ) ),
             this, SLOT( addItemToList( AbstractDataPluginItem * ) ) );
    connect( service, SIGNAL( requestedDownload( const QUrl&,
                                                 const QString&, 
                                                 AbstractDataPluginItem * ) ),
             this, SLOT( downloadItemData( const QUrl&,
                                           const QString&,
                                           AbstractDataPluginItem * ) ) );
    connect( this, SIGNAL( additionalItemsRequested( const GeoDataLatLonAltBox &,
                                                     MarbleDataFacade *,
                                                     qint32) ),
             service, SLOT( getAdditionalItems( const GeoDataLatLonAltBox&,
                                                MarbleDataFacade *,
                                                qint32 ) ) );
}

#include "WeatherModel.moc"
