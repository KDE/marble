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
#include "BBCWeatherService.h"

// Marble
#include "BBCItemGetter.h"
#include "BBCStation.h"
#include "BBCWeatherItem.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDirs.h"
#include "StationListParser.h"
#include "WeatherData.h"
#include "WeatherModel.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"

// Qt
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QTime>
#include <QtCore/QUrl>

using namespace Marble;

const quint32 maxDisplayedFavoritesNumber = 100;

BBCWeatherService::BBCWeatherService( QObject *parent )
    : AbstractWeatherService( parent ),
      m_parsingStarted( false ),
      m_parser( 0 ),
      m_itemGetter( new BBCItemGetter( this ) )
{
     qRegisterMetaType<BBCStation>("BBCStation");
}

BBCWeatherService::~BBCWeatherService()
{
}

QList<BBCStation> BBCWeatherService::filterStationsList( const QStringList& favorites )
{
    QList<BBCStation> favoriteStations;

    foreach ( const QString& id, favorites ) {
        foreach ( const BBCStation& station, m_stationList ) {
            if ( station.bbcId() == id.mid( 3 ).toUInt() ) {
                favoriteStations.append( station );
            }
        }
    }

    return favoriteStations;
}

void BBCWeatherService::setFavoriteItems( const QStringList& favorite )
{
    if ( favoriteItems() != favorite ) {
        m_parsingStarted = false;

        delete m_itemGetter;
        m_itemGetter = new BBCItemGetter( this );

        AbstractWeatherService::setFavoriteItems( favorite );
    }
}

void BBCWeatherService::setFavoriteItemsOnly( bool favoriteOnly )
{
    if ( isFavoriteItemsOnly() != favoriteOnly ) {
        m_parsingStarted = false;

        delete m_itemGetter;
        m_itemGetter = new BBCItemGetter( this );

        AbstractWeatherService::setFavoriteItemsOnly( favoriteOnly );
    }
}

void BBCWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                            const MarbleModel *model,
                                            qint32 number )
{
    if ( !m_parsingStarted ) {
        setupList();
    }

    m_itemGetter->setSchedule( box, model, number );
}

void BBCWeatherService::fetchStationList()
{
    if ( !m_parser ) {
        return;
    }

    connect( m_itemGetter,
             SIGNAL( foundStation( BBCStation ) ),
             this,
             SLOT( createItem( BBCStation ) ) );

    m_stationList = m_parser->stationList();

    if ( isFavoriteItemsOnly() ) {
        m_itemGetter->setStationList( filterStationsList( favoriteItems() ) );
    } else {
        m_itemGetter->setStationList( m_stationList );
    }

    delete m_parser;
    m_parser = 0;
}

void BBCWeatherService::createItem( BBCStation station )
{
    BBCWeatherItem *item = new BBCWeatherItem( this );
    item->setBbcId( station.bbcId() );
    item->setCoordinate( station.coordinate() );
    item->setPriority( station.priority() );
    item->setStationName( station.name() );
    item->setTarget( "earth" );

    emit requestedDownload( item->observationUrl(), "bbcobservation", item );
    emit requestedDownload( item->forecastUrl(),    "bbcforecast",    item );
}

void BBCWeatherService::setupList()
{
    m_parsingStarted = true;

    m_parser = new StationListParser( this );
    m_parser->setPath( MarbleDirs::path( "weather/bbc-stations.xml" ) );
    connect( m_parser, SIGNAL( finished() ),
             this,     SLOT( fetchStationList() ) );
    if ( m_parser->wait( 100 ) ) {
        m_parser->start( QThread::IdlePriority );
    }
}

#include "BBCWeatherService.moc"
