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
#include "StationListParser.h"

// Marble
#include "global.h"
#include "BBCWeatherItem.h"
#include "GeoDataCoordinates.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QString>

using namespace Marble;

StationListParser::StationListParser( QObject *parent )
    : m_parent( parent )
{
}

QList<BBCWeatherItem*> StationListParser::read( QIODevice *device ) {
    m_list.clear();
    setDevice( device );

    while ( !atEnd() ) {
        readNext();

        if ( isStartElement() ) {
            if ( name() == "StationList" )
                readStationList();
            else
                raiseError( "The file is not an valid file." );
        }
    }

    return m_list;
}

void StationListParser::readUnknownElement() {
    Q_ASSERT( isStartElement() );

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isStartElement() )
            readUnknownElement();
    }
}

void StationListParser::readStationList() {
    Q_ASSERT( isStartElement()
              && name() == "StationList" );
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "Station" )
                readStation();
            else
                readUnknownElement();
        }
    }
}
    
void StationListParser::readStation() {
    Q_ASSERT( isStartElement()
              && name() == "Station" );
    
    BBCWeatherItem *item = new BBCWeatherItem( m_parent );
    
    while ( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "name" )
                item->setStationName( readCharacters() );
            else if ( name() == "id" )
                item->setBbcId( readCharacters().toLong() );
            else if ( name() == "Point" )
                readPoint( item );
            else
                readUnknownElement();
        }
    }
    
    m_list.append( item );
}

QString StationListParser::readCharacters() {
    Q_ASSERT( isStartElement() );
    
    QString string;
    
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isStartElement() ) {
            readUnknownElement();
        }
        
        if ( isCharacters() ) {
            string = text().toString();
        }
    }
    
    return string;
}

void StationListParser::readPoint( BBCWeatherItem *item ) {
    Q_ASSERT( isStartElement()
              && name() == "Point" );
    
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isStartElement() ) {
            if ( name() == "coordinates" ) {
                QString coorString = readCharacters();
                QStringList coorList = coorString.split( ',' );
                
                if ( coorList.size() >= 2 ) {
                    GeoDataCoordinates coordinates( coorList.at( 0 ).toFloat() * DEG2RAD,
                                                    coorList.at( 1 ).toFloat() * DEG2RAD );
                    item->setCoordinate( coordinates );
                }
            }
            else
                readUnknownElement();
        }
    }
}
