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
#include <QTimer>
#include <QUrl>

// Marble
#include "BBCWeatherService.h"
#include "FakeWeatherService.h"
#include "GeoNamesWeatherService.h"
#include "AbstractDataPluginItem.h"
#include "WeatherItem.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"

using namespace Marble;

WeatherModel::WeatherModel( const MarbleModel *marbleModel, QObject *parent )
    : AbstractDataPluginModel( "weather", marbleModel, parent )
{
    registerItemProperties( WeatherItem::staticMetaObject );

    // addService( new FakeWeatherService( marbleModel(), this ) );
    addService( new BBCWeatherService( marbleModel, this ) );
    addService( new GeoNamesWeatherService( marbleModel, this ) );

    m_timer = new QTimer();
    connect( m_timer, SIGNAL(timeout()), SLOT(clear()) );

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

        downloadItem( url, type, item );
        addItemToList( item );
    } else {
        if ( existingItem != item )
            item->deleteLater();
        
        WeatherItem *existingWeatherItem = qobject_cast<WeatherItem*>( existingItem );
        if( existingWeatherItem && existingWeatherItem->request( type ) ) {
            downloadItem( url, type, existingItem );
            addItemToList( existingItem );
        }
    }
}

void WeatherModel::getAdditionalItems( const GeoDataLatLonAltBox& box,
                               qint32 number )
{
    foreach ( AbstractWeatherService *service, m_services ) {
        service->getAdditionalItems( box, number );
    }
}

void WeatherModel::getItem( const QString &id )
{
    foreach( AbstractWeatherService* service, m_services ) {
        service->getItem( id );
    }
}

void WeatherModel::parseFile( const QByteArray& file )
{
    foreach ( AbstractWeatherService *service, m_services ) {
        service->parseFile( file );
    }
}

void WeatherModel::downloadDescriptionFileRequested( const QUrl& url )
{
    downloadDescriptionFile( url );
}

void WeatherModel::setMarbleWidget(MarbleWidget *widget)
{
    foreach ( AbstractWeatherService* service, m_services ) {
        service->setMarbleWidget( widget );
    }
}

void WeatherModel::addService( AbstractWeatherService *service )
{
    service->setFavoriteItems( favoriteItems() );

    connect( service, SIGNAL(createdItems(QList<AbstractDataPluginItem*>)),
             this, SLOT(addItemsToList(QList<AbstractDataPluginItem*>)) );
    connect( service, SIGNAL(requestedDownload(QUrl,QString,AbstractDataPluginItem*)),
             this, SLOT(downloadItemData(QUrl,QString,AbstractDataPluginItem*)) );
    connect( service, SIGNAL(downloadDescriptionFileRequested(QUrl)),
             this, SLOT(downloadDescriptionFileRequested(QUrl)) );

    m_services.append( service );
}

#include "moc_WeatherModel.cpp"
