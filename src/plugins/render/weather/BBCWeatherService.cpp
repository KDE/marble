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
#include <QUrl>

using namespace Marble;

BBCWeatherService::BBCWeatherService( const MarbleModel *model, QObject *parent )
    : AbstractWeatherService( model, parent ),
      m_parsingStarted( false ),
      m_parser( 0 ),
      m_itemGetter( new BBCItemGetter( this ) )
{
     qRegisterMetaType<BBCStation>("BBCStation");
}

BBCWeatherService::~BBCWeatherService()
{
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

void BBCWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                            qint32 number )
{
    if ( !m_parsingStarted ) {
        m_parsingStarted = true;

        m_parser = new StationListParser( this );
        m_parser->setPath(MarbleDirs::path(QStringLiteral("weather/bbc-stations.xml")));
        connect( m_parser, SIGNAL(finished()),
                 this,     SLOT(fetchStationList()) );
        if ( m_parser->wait( 100 ) ) {
            m_parser->start( QThread::IdlePriority );
        }
    }

    m_itemGetter->setSchedule( box, number );
}

void BBCWeatherService::getItem( const QString &id )
{
    if ( id.startsWith( QLatin1String( "bbc" ) ) ) {
        BBCStation const station = m_itemGetter->station( id );
        if ( station.bbcId() > 0 ) {
            createItem( station );
        }
    }
}

void BBCWeatherService::fetchStationList()
{
    if ( !m_parser ) {
        return;
    }

    connect( m_itemGetter,
             SIGNAL(foundStation(BBCStation)),
             this,
             SLOT(createItem(BBCStation)) );

    m_stationList = m_parser->stationList();
    m_itemGetter->setStationList( m_stationList );

    delete m_parser;
    m_parser = 0;
}

void BBCWeatherService::createItem( const BBCStation& station )
{
    BBCWeatherItem *item = new BBCWeatherItem( this );
    item->setMarbleWidget( marbleWidget() );
    item->setBbcId( station.bbcId() );
    item->setCoordinate( station.coordinate() );
    item->setPriority( station.priority() );
    item->setStationName( station.name() );

    emit requestedDownload( item->observationUrl(), "bbcobservation", item );
    emit requestedDownload( item->forecastUrl(),    "bbcforecast",    item );
}

#include "moc_BBCWeatherService.cpp"
