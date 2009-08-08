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
#include "GeoDataLatLonAltBox.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QString>

using namespace Marble;

static bool lessThanByPointer( const BBCWeatherItem *item1,
                               const BBCWeatherItem *item2 )
{
    if( item1 != 0 && item2 != 0 ) {
        return item1->operator<( item2 );
    }
    else {
        return false;
    }
}

StationListParser::StationListParser( QObject *parent )
    : m_parent( parent )
{
}

QList<BBCWeatherItem*> StationListParser::read( QIODevice *device )
{
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

void StationListParser::readUnknownElement()
{
    Q_ASSERT( isStartElement() );

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isStartElement() )
            readUnknownElement();
    }
}

void StationListParser::readStationList()
{
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
    
void StationListParser::readStation()
{
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
            else if ( name() == "priority" )
                item->setPriority( readCharacters().toInt() );
            else if ( name() == "Point" )
                readPoint( item );
            else if ( name() == "Region" )
                readRegion( item );
            else
                readUnknownElement();
        }
    }

    // This find the right position in the sorted to insert the new item
    QList<BBCWeatherItem*>::iterator i = qLowerBound( m_list.begin(),
                                                      m_list.end(),
                                                      item,
                                                      lessThanByPointer );
    // Insert the item on the right position in the list
    m_list.insert( i, item );
}

QString StationListParser::readCharacters()
{
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

void StationListParser::readPoint( BBCWeatherItem *item )
{
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

void StationListParser::readRegion( BBCWeatherItem *item )
{
    Q_ASSERT( isStartElement()
              && name() == "Region" );

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isStartElement() ) {
            if ( name() == "LatLonAltBox" ) {
                GeoDataLatLonAltBox box = readLatLonAltBox();
                if ( !box.isNull() ) {
                    item->setLatLonAltBox( box );
                    item->setMinLodPixels( 10 );
                }
            }
            else {
                readUnknownElement();
            }
        }
    }
}

GeoDataLatLonAltBox StationListParser::readLatLonAltBox()
{
    Q_ASSERT( isStartElement()
              && name() == "LatLonAltBox" );

    GeoDataLatLonAltBox box;

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isStartElement() ) {
            if ( name() == "north" ) {
                QString northString = readCharacters();
                box.setNorth( northString.toDouble() * DEG2RAD );
            }
            else if ( name() == "south" ) {
                QString southString = readCharacters();
                box.setSouth( southString.toDouble() * DEG2RAD );
            }
            else if ( name() == "west" ) {
                QString westString = readCharacters();
                box.setWest( westString.toDouble() * DEG2RAD );
            }
            else if ( name() == "east" ) {
                QString eastString = readCharacters();
                box.setEast( eastString.toDouble() * DEG2RAD );
            }
            else {
                readUnknownElement();
            }
        }
    }

    return box;
}
