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
#include "MarbleMath.h"

#include <QtCore/QUrl>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

namespace Marble
{
    
FoursquareModel::FoursquareModel(QObject* parent)
    : AbstractDataPluginModel("foursquare", parent)
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
    qreal const distanceLon = model->planetRadius() * distanceSphere( box.west(), box.north(), box.east(), box.north() );
    qreal const distanceLat = model->planetRadius() * distanceSphere( box.west(), box.north(), box.west(), box.south() );
    qreal const area = distanceLon * distanceLat;
    if ( area > 10 * 1000 * KM2METER * KM2METER ) {
        // Large area (> 10.000 km^2) => too large for bbox queries
        apiUrl += "?ll=" + QString::number( box.center().latitude(Marble::GeoDataCoordinates::Degree) );
        apiUrl += ',' + QString::number( box.center().longitude(Marble::GeoDataCoordinates::Degree) );
        apiUrl += "&intent=checkin";
    } else {
        apiUrl += "?ne=" + QString::number( box.north(Marble::GeoDataCoordinates::Degree) );
        apiUrl += ',' + QString::number( box.east(Marble::GeoDataCoordinates::Degree) );
        apiUrl += "&sw=" + QString::number( box.south(Marble::GeoDataCoordinates::Degree) );
        apiUrl += ',' + QString::number( box.west(Marble::GeoDataCoordinates::Degree) );
        apiUrl += "&intent=browse";
    }
    apiUrl += "&limit=" + QString::number( number );
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
    data = engine.evaluate( '(' + QString::fromUtf8( file ) + ')' );
    data = data.property("response");
    
    // Parse if any result exists
    if ( data.property( "venues" ).isArray() ) {
        QScriptValueIterator iterator( data.property( "venues" ) );
        // Add items to the list
        QList<AbstractDataPluginItem*> items;
        do {
            iterator.next();
            QString id = iterator.value().property( "id" ).toString();
            QString name = iterator.value().property( "name" ).toString();
            QString category = iterator.value().property( "categories" ).property( 0 ).property( "name" ).toString();
            QString address = iterator.value().property( "location" ).property( "address" ).toString();
            QString city = iterator.value().property( "location" ).property( "city" ).toString();
            QString country = iterator.value().property( "location" ).property( "country" ).toString();
            double latitude = iterator.value().property( "location" ).property( "lat" ).toString().toDouble();
            double longitude = iterator.value().property( "location" ).property( "lng" ).toString().toDouble();
            int usersCount = iterator.value().property( "stats" ).property( "usersCount" ).toInteger();
            
            QScriptValue categoryIcon = iterator.value().property( "categories" ).property( 0 ).property( "icon" );
            QString iconUrl;
            QString largeIconUrl;
            if ( categoryIcon.isValid() ) {
                iconUrl = categoryIcon.property( "prefix" ).toString()
                        + "32" // That's the icon size hardcoded
                        + categoryIcon.property( "name" ).toString();
                        
                largeIconUrl = categoryIcon.property( "prefix" ).toString()
                        + "64" // Larger icon
                        + categoryIcon.property( "name" ).toString();
            }

            if( !itemExists( id ) ) {
                GeoDataCoordinates coordinates( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
                FoursquareItem *item = new FoursquareItem( this );
                item->setId( id );
                item->setCoordinate( coordinates );
                item->setTarget( "earth" );
                item->setName( name );
                item->setCategory( category );
                item->setAddress( address );
                item->setCity( city );
                item->setCountry( country );
                item->setUsersCount( usersCount );
                item->setCategoryIconUrl( iconUrl );
                item->setCategoryLargeIconUrl( largeIconUrl );
                
                items << item;
            }
        }
        while ( iterator.hasNext() );
        addItemsToList( items );
    }
}

}

#include "FoursquareModel.moc"
