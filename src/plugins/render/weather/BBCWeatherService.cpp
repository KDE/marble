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
      m_scheduledBox(),
      m_scheduledNumber( 0 ),
      m_scheduledFacade( 0 )
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

    if ( m_items.isEmpty() ) {
        m_scheduledBox = box;
        m_scheduledNumber = number;
        m_scheduledFacade = facade;
        return;
    }

    qint32 fetched = 0;
    QList<BBCWeatherItem *>::iterator it = m_items.begin();
    
    while ( fetched < number && it != m_items.end() ) {
        if ( (*it) && box.contains( (*it)->coordinate() ) ) {
            (*it)->setTarget( "earth" );
            emit requestedDownload( (*it)->observationUrl(), "bbcobservation", (*it) );
            emit requestedDownload( (*it)->forecastUrl(),    "bbcforecast",    (*it) );
            fetched++;
        }
        ++it;
    }
}

void BBCWeatherService::fetchStationList()
{
    m_items = m_parser->stationList();
    delete m_parser;
    m_parser = 0;

    if ( m_scheduledNumber
         && !m_scheduledBox.isNull()
         && m_scheduledFacade ) {
        getAdditionalItems( m_scheduledBox, m_scheduledFacade, m_scheduledNumber );
    }
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
