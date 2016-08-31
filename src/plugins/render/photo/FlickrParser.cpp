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
#include "FlickrParser.h"

// Marble
#include "PhotoPluginItem.h"

// Qt
#include <QByteArray>

using namespace Marble;

FlickrParser::FlickrParser( MarbleWidget *widget,
                            QList<PhotoPluginItem *> *list,
                            QObject *parent )
    : m_marbleWidget( widget ),
      m_list( list ),
      m_parent( parent )
{
}

bool FlickrParser::read( QByteArray data )
{
    addData( data );

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == QLatin1String("rsp")) {
                if (attributes().value(QLatin1String("stat")) == QLatin1String("ok")) {
                    readFlickr();
                } else {
                    raiseError(QObject::tr("Query failed"));
                }
            } else {
                raiseError(QObject::tr("The file is not a valid Flickr answer."));
            }
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
              && name() == QLatin1String("rsp")
              && attributes().value(QLatin1String("stat")) == QLatin1String("ok"));
              
    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if (name() == QLatin1String("photos"))
                readPhotos();
            else
                readUnknownElement();
        }
    }
}

void FlickrParser::readPhotos()
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("photos"));

    while( !atEnd() ) {
        readNext();
        
        if( isEndElement() )
            break;
        
        if( isStartElement() ) {
            if (name() == QLatin1String("photo"))
                readPhoto();
            else
                readUnknownElement();
        }
    }
}

void FlickrParser::readPhoto()
{
    Q_ASSERT( isStartElement()
              && name() == QLatin1String("photo"));

    if( attributes().hasAttribute(QLatin1String("id")) ) {
        PhotoPluginItem *item = new PhotoPluginItem( m_marbleWidget, m_parent );
        item->setId( attributes().value(QLatin1String("id")).toString() );
        item->setServer( attributes().value(QLatin1String("server")).toString() );
        item->setFarm( attributes().value(QLatin1String("farm")).toString() );
        item->setSecret( attributes().value(QLatin1String("secret")).toString() );
        item->setOwner( attributes().value(QLatin1String("owner")).toString() );
        item->setTitle( attributes().value(QLatin1String("title")).toString() );
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
