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
    : AbstractWeatherService( parent )
{
    QTime time;
    QFile file( MarbleDirs::path( "weather/bbc-stations.xml" ) );
    qDebug() << "Filepath: " << file.fileName();
    
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        return;
    }
    
    StationListParser parser;
    m_items = parser.read( &file );
    qDebug() << "Parsed station list in " << time.elapsed() << " ms and found " << m_items.size() << "items";
}

BBCWeatherService::~BBCWeatherService() {
}
    
void BBCWeatherService::getAdditionalItems( const GeoDataLatLonAltBox& box,
                                            MarbleDataFacade *facade,
                                            qint32 number )
{
    Q_UNUSED( facade );
    
    qint32 fetched = 0;
    QList<BBCWeatherItem *>::iterator it = m_items.begin();
    
    while ( fetched < number && it != m_items.end() ) {
        if ( box.contains( (*it)->coordinate() ) ) {
            (*it)->setTarget( "earth" );
            emit requestedDownload( (*it)->observationUrl(), "bbcobservation", (*it) );
            fetched++;
        }
        ++it;
    }
}

#include "BBCWeatherService.moc"
