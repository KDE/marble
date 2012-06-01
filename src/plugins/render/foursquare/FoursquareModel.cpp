//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#include "FoursquareModel.h"
#include "FoursquarePlugin.h"
#include "FoursquareItem.h"

#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"

#include <QDebug>
#include <QtCore/QUrl>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

namespace Marble
{
    
FoursquareModel::FoursquareModel(const PluginManager* pluginManager, QObject* parent)
    : AbstractDataPluginModel("foursquare", pluginManager, parent)
{
    // Enjoy laziness
}

FoursquareModel::~FoursquareModel()
{
}

void FoursquareModel::getAdditionalItems( const GeoDataLatLonAltBox& box, const MarbleModel *model, qint32 number )
{
    if( model->planetId() != "earth" ) {
        return;
    }
    
    QString clientId = "YPRWSYFW1RVL4PJQ2XS5G14RTOGTHOKZVHC1EP5KCCCYQPZF";
    QString clientSecret = "5L2JDCAYQCEJWY5FNDU4A1RWATE4E5FIIXXRM41YBTFSERUH";
    
    QString apiUrl( "https://api.foursquare.com/v2/venues/search" );
    apiUrl += "?ll=" + QString::number( box.center().latitude(Marble::GeoDataCoordinates::Degree) );
    apiUrl += "," + QString::number( box.center().longitude(Marble::GeoDataCoordinates::Degree) );
    apiUrl += "&client_id=" + clientId;
    apiUrl += "&client_secret=" + clientSecret;
    apiUrl += "&v=20120601";
    downloadDescriptionFile( QUrl( apiUrl ) );
}

void FoursquareModel::parseFile( const QByteArray& file )
{
    QScriptValue data;
    QScriptEngine engine;
    // Qt requires parentheses around JSON
    data = engine.evaluate( "(" + QString( file ) + ")" );
    data = data.property("response");
    
    // Parse if any result exists
    if ( data.property( "venues" ).isArray() ) {
        QScriptValueIterator iterator( data.property( "venues" ) );
        // Add items to the list
        do {
            iterator.next();
            QString id = iterator.value().property( "id" ).toString(); qDebug() << id;
            QString name = iterator.value().property( "name" ).toString(); qDebug() << name;
            double latitude = iterator.value().property( "location" ).property( "lat" ).toString().toDouble(); qDebug() << latitude;
            double longitude = iterator.value().property( "location" ).property( "lng" ).toString().toDouble(); qDebug() << longitude;
            int usersCount = iterator.value().property( "stats" ).property( "usersCount" ).toInteger();

            if( !itemExists( id ) ) {
                GeoDataCoordinates coordinates( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
                FoursquareItem *item = new FoursquareItem( this );
                item->setId( id );
                item->setCoordinate( coordinates );
                item->setTarget( "earth" );
                item->setName( name );
                item->setUsersCount( usersCount );

                addItemToList( item );
            }
        }
        
        while ( iterator.hasNext() );
    }
}

}

#include "FoursquareModel.moc"