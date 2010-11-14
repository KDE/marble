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
#include "GeonamesParser.h"

// Marble
#include "global.h"
#include "WikipediaItem.h"
#include "MarbleDebug.h"

// Qt
#include <QtCore/QByteArray>

using namespace Marble;

GeonamesParser::GeonamesParser( QList<WikipediaItem *> *list,
                                QObject *parent )
    : m_list( list ),
      m_parent( parent )
{
}

bool GeonamesParser::read( const QByteArray& data )
{
    addData( data );

    while (!atEnd()) {
        readNext();

        if ( isStartElement() ) {
            if ( name() == "geonames" )
                readGeonames();
            else
                raiseError( QObject::tr("The file is not a valid Geonames answer.") );
        }
    }

    return !error();
}

void GeonamesParser::readUnknownElement()
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

void GeonamesParser::readGeonames()
{
    Q_ASSERT( isStartElement()
              && name() == "geonames" );
    
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isStartElement() ) {
            if ( name() == "entry" )
                readEntry();
            else
                readUnknownElement();
        }
    }
}

void GeonamesParser::readEntry()
{
    Q_ASSERT( isStartElement()
              && name() == "entry" );
              
    WikipediaItem *item = new WikipediaItem( m_parent );
    m_list->append( item );
    
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
            
        if ( isStartElement() ) {
            if ( name() == "title" )
                readTitle( item );
            else if ( name() == "lng" )
                readLongitude( item );
            else if ( name() == "lat" )
                readLatitude( item );
            else if ( name() == "wikipediaUrl" )
                readUrl( item );
            else if ( name() == "summary" )
                readSummary( item );
            else if ( name() == "thumbnailImg" )
                readThumbnailImage( item );
            else
                readUnknownElement();
        }
    }
}

void GeonamesParser::readTitle( WikipediaItem *item )
{
    Q_ASSERT( isStartElement()
              && name() == "title" );
              
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isCharacters() ) {
            item->setName( text().toString() );
        }
    }
}

void GeonamesParser::readLongitude( WikipediaItem *item )
{
    Q_ASSERT( isStartElement()
              && name() == "lng" );
              
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isCharacters() ) {
            item->setLongitude( text().toString().toDouble() * DEG2RAD );
        }
    }
}

void GeonamesParser::readLatitude( WikipediaItem *item )
{
    Q_ASSERT( isStartElement()
              && name() == "lat" );
              
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isCharacters() ) {
            item->setLatitude( text().toString().toDouble() * DEG2RAD );
        }
    }
}

void GeonamesParser::readUrl( WikipediaItem *item )
{
    Q_ASSERT( isStartElement()
              && name() == "wikipediaUrl" );
              
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isCharacters() ) {
            item->setUrl( QUrl::fromEncoded( text().toString().toUtf8() ) );
        }
    }
}

void GeonamesParser::readSummary( WikipediaItem *item )
{
    Q_ASSERT( isStartElement()
              && name() == "summary" );

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isCharacters() ) {
            item->setSummary( text().toString() );
        }
    }
}

void GeonamesParser::readThumbnailImage( WikipediaItem *item )
{
    Q_ASSERT( isStartElement()
              && name() == "thumbnailImg" );
             
    while ( !atEnd() ) {
        readNext();
        
        if ( isEndElement() )
            break;
        
        if ( isCharacters() ) {
            item->setThumbnailImageUrl( QUrl( text().toString() ) );
        }
    }
}
