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
#include "GeonamesParser.h"

// Marble
#include "global.h"
#include "WikipediaWidget.h"

// Qt
#include <QtCore/QByteArray>
#include <QtCore/QDebug>

using namespace Marble;

GeonamesParser::GeonamesParser( QList<WikipediaWidget *> *list,
                                QObject *parent )
    : m_list( list ),
      m_parent( parent )
{
}

bool GeonamesParser::read( QByteArray data ) {
    addData( data );

    while (!atEnd()) {
        readNext();

        if ( isStartElement() ) {
            if ( name() == "geonames" )
                readGeonames();
            else
                raiseError( "The file is not an valid Geonames answer." );
        }
    }

    return !error();
}

void GeonamesParser::readUnknownElement() {
    Q_ASSERT( isStartElement() );

    while ( !atEnd() ) {
        readNext();

        if( isEndElement() )
            break;

        if( isStartElement() )
            readUnknownElement();
    }
}

void GeonamesParser::readGeonames() {
    Q_ASSERT( isStartElement()
              && name() == "geonames" );
    
    while ( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "entry" )
                readEntry();
            else
                readUnknownElement();
        }
    }
}

void GeonamesParser::readEntry() {
    Q_ASSERT( isStartElement()
              && name() == "entry" );
              
    WikipediaWidget *widget = new WikipediaWidget( m_parent );
    m_list->append( widget );
    
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
            
        if( isStartElement() ) {
            if( name() == "title" )
                readTitle( widget );
            else if( name() == "lng" )
                readLongitude( widget );
            else if( name() == "lat" )
                readLatitude( widget );
            else if( name() == "wikipediaUrl" )
                readUrl( widget );
            else if( name() == "thumbnailImg" )
                readThumbnailImage( widget );
            else
                readUnknownElement();
        }
    }
}

void GeonamesParser::readTitle( WikipediaWidget *widget ) {
    Q_ASSERT( isStartElement()
              && name() == "title" );
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isCharacters() ) {
            widget->setId( text().toString() );
        }
    }
}

void GeonamesParser::readLongitude( WikipediaWidget *widget ) {
    Q_ASSERT( isStartElement()
              && name() == "lng" );
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isCharacters() ) {
            widget->setLongitude( text().toString().toDouble() * DEG2RAD );
        }
    }
}

void GeonamesParser::readLatitude( WikipediaWidget *widget ) {
    Q_ASSERT( isStartElement()
              && name() == "lat" );
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isCharacters() ) {
            widget->setLatitude( text().toString().toDouble() * DEG2RAD );
        }
    }
}

void GeonamesParser::readUrl( WikipediaWidget *widget ) {
    Q_ASSERT( isStartElement()
              && name() == "wikipediaUrl" );
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isCharacters() ) {
            widget->setUrl( QUrl::fromEncoded( text().toString().toUtf8() ) );
        }
    }
}

void GeonamesParser::readThumbnailImage( WikipediaWidget *widget ) {
    Q_ASSERT( isStartElement()
              && name() == "thumbnailImg" );
             
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isCharacters() ) {
            widget->setThumbnailImageUrl( QUrl( text().toString() ) );
        }
    }
}
