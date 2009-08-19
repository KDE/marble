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
#include "BBCItemGetter.h"
#include "BBCWeatherItem.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDirs.h"
#include "StationListParser.h"
#include "WeatherData.h"
#include "WeatherModel.h"
#include "global.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QTime>
#include <QtCore/QUrl>

using namespace Marble;

BBCWeatherService::BBCWeatherService( QObject *parent ) 
    : AbstractWeatherService( parent ),
      m_parsingStarted( false ),
      m_parser( 0 ),
      m_itemGetter( new BBCItemGetter( this ) )
{
}

BBCWeatherService::~BBCWeatherService()
{
}
    
void BBCWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                            MarbleDataFacade *facade,
                                            qint32 number )
{
    Q_UNUSED( facade );

    if ( !m_parsingStarted ) {
        setupList();
    }

    m_itemGetter->setSchedule( box, facade, number );
}

void BBCWeatherService::fetchStationList()
{
    connect( m_itemGetter,
             SIGNAL( requestedDownload( QUrl, QString, AbstractDataPluginItem* ) ),
             this,
             SIGNAL( requestedDownload( QUrl, QString, AbstractDataPluginItem* ) ) );
    m_itemGetter->setStationList( m_parser->stationList() );
    delete m_parser;
    m_parser = 0;
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
