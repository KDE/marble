//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın <utkuaydin34@gmail.com>
//


#include "OpenDesktopModel.h"
#include "OpenDesktopItem.h"
#include "global.h"
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
 
using namespace Marble;
 
 
OpenDesktopModel::OpenDesktopModel( const PluginManager *pluginManager, QObject *parent )
    : AbstractDataPluginModel( "opendesktop", pluginManager, parent )
{
    // Nothing to do...
}
 
OpenDesktopModel::~OpenDesktopModel()
{
    // Nothing to do...
}
 
void OpenDesktopModel::getAdditionalItems( const GeoDataLatLonAltBox& box, const MarbleModel *model, qint32 number )
{
    Q_UNUSED( number )
  
    if( model->planetId() != "earth" )
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
    data = engine.evaluate( "(" + QString(file) + ")" );
    
    // Parse if any result exists
    if ( data.property( "data" ).isArray() )
    {  
    QScriptValueIterator iterator( data.property( "data" ) );
        // Add items to the list
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
                item->setId( personid );
                item->setCoordinate( coor );
                item->setTarget( "earth" );
                item->setFullName( QString( "%1 %2" ).arg( firstName ).arg( lastName ) );
                item->setLocation( QString( "%1, %2" ).arg( city ).arg( country ) );
                item->setRole( !role.isEmpty() ? role : QString( "nothing" ) );
                downloadItemData( avatarUrl, "avatar", item );
            }
        }
    }
}
 
#include "OpenDesktopModel.moc"
