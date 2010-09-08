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
#include "FlickrParser.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "PhotoPluginItem.h"

// Qt
#include <QtCore/QByteArray>

using namespace Marble;

FlickrParser::FlickrParser( QList<PhotoPluginItem *> *list,
                            QObject *parent )
    : m_list( list ),
      m_parent( parent )
{
}

bool FlickrParser::read( QByteArray data )
{
    addData( data );

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if ( name() == "rsp" && attributes().value( "stat" ) == "ok" )
                readFlickr();
            else if ( name() == "rsp" )
                raiseError( QObject::tr("Query failed") );
            else
                raiseError( QObject::tr("The file is not an valid Flickr answer.") );
        }
    }

    return !error();
}

void FlickrParser::readUnknownElement()
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

void FlickrParser::readFlickr()
{
    Q_ASSERT( isStartElement()
              && name() == "rsp" 
              && attributes().value( "stat" ) == "ok" );
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "photos" )
                readPhotos();
            else
                readUnknownElement();
        }
    }
}

void FlickrParser::readPhotos()
{
    Q_ASSERT( isStartElement()
              && name() == "photos" );

    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if( name() == "photo" )
                readPhoto();
            else
                readUnknownElement();
        }
    }
}

void FlickrParser::readPhoto()
{
    Q_ASSERT( isStartElement()
              && name() == "photo" );

    if( attributes().hasAttribute( "id" ) ) {
        PhotoPluginItem *item = new PhotoPluginItem( m_parent );
        item->setId( attributes().value( "id" ).toString() );
        item->setServer( attributes().value( "server" ).toString() );
        item->setFarm( attributes().value( "farm" ).toString() );
        item->setSecret( attributes().value( "secret" ).toString() );
        item->setOwner( attributes().value( "owner" ).toString() );
        item->setTitle( attributes().value( "title" ).toString() );
        m_list->append( item );
    }
    
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() )
            break;
    }
}
