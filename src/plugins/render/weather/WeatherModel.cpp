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

// Qt
#include <QtCore/QTimer>
#include <QtCore/QUrl>

// Marble
#include "BBCWeatherService.h"
#include "FakeWeatherService.h"
#include "GeoNamesWeatherService.h"
#include "AbstractDataPluginItem.h"
#include "WeatherItem.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"

using namespace Marble;

WeatherModel::WeatherModel( const PluginManager *pluginManager, QObject *parent )
    : AbstractDataPluginModel( "weather", pluginManager, parent ),
      m_initialized( false )
{
    createServices();

    m_timer = new QTimer();
    connect( m_timer, SIGNAL( timeout() ), SLOT( updateItems() ) );

    // Default interval = 3 hours
    setUpdateInterval( 3 );

    m_timer->start();
}
    
WeatherModel::~WeatherModel()
{
}

void WeatherModel::setFavoriteItems( const QStringList& list )
{
    if ( favoriteItems() != list ) {
        foreach ( AbstractWeatherService *service, m_services ) {
            service->setFavoriteItems( list );
        }

        AbstractDataPluginModel::setFavoriteItems( list );

        if ( m_initialized && isFavoriteItemsOnly() ) {
            updateItems();
        }
    }
}

void WeatherModel::setUpdateInterval( quint32 hours )
{
    quint32 msecs = hours * 60 * 60 * 1000;
    m_timer->setInterval( msecs );
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

void WeatherModel::getAdditionalItems( const GeoDataLatLonAltBox& box,
                               const MarbleModel *model,
                               qint32 number )
{
    m_lastBox = box;
    m_lastModel = model;
    m_lastNumber = number;

    m_initialized = true;

    emit additionalItemsRequested( box, model, number );
}

void WeatherModel::getItem( const QString &id, const MarbleModel *model )
{
    foreach( AbstractWeatherService* service, m_services ) {
        service->getItem( id, model );
    }
}

void WeatherModel::parseFile( const QByteArray& file )
{
    emit parseFileRequested( file );
}

void WeatherModel::updateItems()
{
    clear();
    emit additionalItemsRequested( m_lastBox, m_lastModel, m_lastNumber );
    emit itemsUpdated();
}

void WeatherModel::createServices()
{
    // addService( new FakeWeatherService( this ) );
    addService( new BBCWeatherService( this ) );
    addService( new GeoNamesWeatherService( this ) );
}

void WeatherModel::downloadDescriptionFileRequested( const QUrl& url )
{
    downloadDescriptionFile( url );
}

void WeatherModel::addService( AbstractWeatherService *service )
{
    service->setFavoriteItems( favoriteItems() );

    connect( service, SIGNAL( createdItems( QList<AbstractDataPluginItem*> ) ),
             this, SLOT( addItemsToList( QList<AbstractDataPluginItem*> ) ) );
    connect( service, SIGNAL( requestedDownload( const QUrl&,
                                                 const QString&, 
                                                 AbstractDataPluginItem * ) ),
             this, SLOT( downloadItemData( const QUrl&,
                                           const QString&,
                                           AbstractDataPluginItem * ) ) );
    connect( service, SIGNAL( downloadDescriptionFileRequested( const QUrl& ) ),
             this, SLOT( downloadDescriptionFileRequested( const QUrl& ) ) );

    connect( this, SIGNAL( additionalItemsRequested( const GeoDataLatLonAltBox &,
                                                     const MarbleModel *,
                                                     qint32 ) ),
             service, SLOT( getAdditionalItems( const GeoDataLatLonAltBox&,
                                                const MarbleModel *,
                                                qint32 ) ) );
    connect( this, SIGNAL( parseFileRequested( const QByteArray& ) ),
             service, SLOT( parseFile( const QByteArray& ) ) );

    m_services.append( service );
}

#include "WeatherModel.moc"
