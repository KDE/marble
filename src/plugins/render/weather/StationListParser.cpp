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
#include "StationListParser.h"

// Marble
#include "MarbleGlobal.h"
#include "BBCStation.h"
#include "GeoDataCoordinates.h"
#include "MarbleDebug.h"

// Qt
#include <QFile>
#include <QString>

using namespace Marble;

StationListParser::StationListParser( QObject *parent )
    : QThread( parent ),
      QXmlStreamReader()
{
}

StationListParser::~StationListParser()
{
    wait( 1000 );
}

void StationListParser::read()
{
    m_list.clear();

    while ( !atEnd() ) {
        readNext();

        if ( isStartElement() ) {
            if (name() == QLatin1String("StationList"))
                readStationList();
            else
                raiseError( QObject::tr("The file is not a valid file.") );
        }
    }
}

QList<BBCStation> StationListParser::stationList() const
{
    return m_list;
}

void StationListParser::setPath( const QString& path )
{
    m_path = path;
}

void StationListParser::run()
{
    QFile file( m_path );

    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        return;
    }

    setDevice( &file );
    read();
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
              && name() == QLatin1String("StationList"));
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if (name() == QLatin1String("Station"))
                readStation();
            else
                readUnknownElement();
        }
    }
}
    
void StationListParser::readStation()
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("Station"));
    
    BBCStation station;
    
    while ( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if (name() == QLatin1String("name"))
                station.setName( readCharacters() );
            else if (name() == QLatin1String("id"))
                station.setBbcId( readCharacters().toLong() );
            else if (name() == QLatin1String("priority"))
                station.setPriority( readCharacters().toInt() );
            else if (name() == QLatin1String("Point"))
                readPoint( &station );
            else
                readUnknownElement();
        }
    }

    // This find the right position in the sorted to insert the new item
    QList<BBCStation>::iterator i = qLowerBound( m_list.begin(),
                                                 m_list.end(),
                                                 station );
    // Insert the item on the right position in the list
    m_list.insert( i, station );
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

void StationListParser::readPoint( BBCStation *station )
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("Point"));
    
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isStartElement() ) {
            if (name() == QLatin1String("coordinates")) {
                QString coorString = readCharacters();
                QStringList coorList = coorString.split(QLatin1Char(','));
                
                if ( coorList.size() >= 2 ) {
                    GeoDataCoordinates coordinates( coorList.at( 0 ).toFloat() * DEG2RAD,
                                                    coorList.at( 1 ).toFloat() * DEG2RAD );
                    station->setCoordinate( coordinates );
                }
            }
            else
                readUnknownElement();
        }
    }
}

#include "moc_StationListParser.cpp"
