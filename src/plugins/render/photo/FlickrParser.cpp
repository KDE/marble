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
#include "AbstractDataPluginWidget.h"
#include "PhotoPluginWidget.h"

// Qt
#include <QtCore/QByteArray>
#include <QtCore/QDebug>

using namespace Marble;

FlickrParser::FlickrParser( QList<AbstractDataPluginWidget *> *list,
                            QObject *parent )
    : m_list( list ),
      m_parent( parent )
{
}

bool FlickrParser::read( QByteArray data ) {
    addData( data );

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if ( name() == "rsp" && attributes().value( "stat" ) == "ok" )
                readFlickr();
            else if ( name() == "rsp" )
                raiseError( "Query failed" );
            else
                raiseError( "The file is not an valid Flickr answer." );
        }
    }

    return !error();
}

void FlickrParser::readUnknownElement() {
    Q_ASSERT( isStartElement() );

    while ( !atEnd() ) {
        readNext();

        if ( isEndElement() )
            break;

        if ( isStartElement() )
            readUnknownElement();
    }
}

void FlickrParser::readFlickr() {
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

void FlickrParser::readPhotos() {
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

void FlickrParser::readPhoto() {
    Q_ASSERT( isStartElement()
              && name() == "photo" );
              
    if( attributes().hasAttribute( "id" ) ) {
        PhotoPluginWidget *widget = new PhotoPluginWidget( m_parent );
        widget->setId( attributes().value( "id" ).toString() );
        widget->setServer( attributes().value( "server" ).toString() );
        widget->setFarm( attributes().value( "farm" ).toString() );
        widget->setSecret( attributes().value( "secret" ).toString() );
        widget->setOwner( attributes().value( "owner" ).toString() );
        widget->setTitle( attributes().value( "title" ).toString() );
        m_list->append( widget );
    }
    
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() )
            break;
    }
}
