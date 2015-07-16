//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın        <utkuaydin34@gmail.com>
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//


#include "OpenDesktopModel.h"
#include "OpenDesktopItem.h"
#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include <QString>
#include <QUrl>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>
 
using namespace Marble;
 
 
OpenDesktopModel::OpenDesktopModel( const MarbleModel *marbleModel, QObject *parent )
    : AbstractDataPluginModel( "opendesktop", marbleModel, parent )
{
    // Nothing to do...
}
 
OpenDesktopModel::~OpenDesktopModel()
{
    // Nothing to do...
}

void OpenDesktopModel::setMarbleWidget(MarbleWidget *widget)
{
    m_marbleWidget = widget;
}
 
void OpenDesktopModel::getAdditionalItems( const GeoDataLatLonAltBox& box, qint32 number )
{
    Q_UNUSED( number )
  
    if( marbleModel()->planetId() != "earth" )
        return;
    
    GeoDataCoordinates coords = box.center();
    
    QString openDesktopUrl( "http://api.opendesktop.org/v1/person/data" );
    openDesktopUrl += "?latitude="  + QString::number(coords.latitude() * RAD2DEG);
    openDesktopUrl += "&longitude=" + QString::number(coords.longitude() * RAD2DEG);
    openDesktopUrl += "&format=json";
    
    downloadDescriptionFile( QUrl( openDesktopUrl ) );
}

void OpenDesktopModel::parseFile( const QByteArray& file )
{
    QScriptValue data;
    QScriptEngine engine;
    data = engine.evaluate( '(' + QString(file) + ')' );
    
    // Parse if any result exists
    if ( data.property( "data" ).isArray() )
    {  
    QScriptValueIterator iterator( data.property( "data" ) );
        // Add items to the list
        QList<AbstractDataPluginItem*> items;
        while ( iterator.hasNext() )
        {
            iterator.next();
            // Convert profile's properties from QScriptValue to appropriate types
            QString personid  = iterator.value().property( "personid" ).toString();
            QString firstName = iterator.value().property( "firstname" ).toString();
            QString lastName  = iterator.value().property( "lastname" ).toString();
            QString city      = iterator.value().property( "city" ).toString();
            QString country   = iterator.value().property( "country" ).toString();
            QString role   = iterator.value().property( "communityrole" ).toString();
            double longitude  = iterator.value().property( "longitude" ).toNumber();
            double latitude   = iterator.value().property( "latitude" ).toNumber();
            QUrl avatarUrl( iterator.value().property( "avatarpic" ).toString() );
            
            if( !itemExists( personid ) )
            {
                // If it does not exists, create it
                GeoDataCoordinates coor(longitude * DEG2RAD, latitude * DEG2RAD);
                OpenDesktopItem *item = new OpenDesktopItem( this );
                item->setMarbleWidget(m_marbleWidget);
                item->setId( personid );
                item->setCoordinate( coor );
                item->setFullName( QString( "%1 %2" ).arg( firstName ).arg( lastName ) );
                item->setLocation( QString( "%1, %2" ).arg( city ).arg( country ) );
                item->setRole( !role.isEmpty() ? role : QString( "nothing" ) );
                downloadItem( avatarUrl, "avatar", item );
                items << item;
            }
        }

        addItemsToList( items );
    }
}
 
#include "moc_OpenDesktopModel.cpp"
