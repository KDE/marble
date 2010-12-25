//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#include "EarthquakeModel.h"
#include "EarthquakeItem.h"

#include "global.h"
#include "MarbleDataFacade.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtGui/QMessageBox>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

namespace Marble {

EarthquakeModel::EarthquakeModel( PluginManager *pluginManager,
                                  QObject *parent )
    : AbstractDataPluginModel( "earthquake", pluginManager, parent )
{
    // nothing to do
}

EarthquakeModel::~EarthquakeModel()
{
}

void EarthquakeModel::getAdditionalItems( const GeoDataLatLonAltBox& box, MarbleDataFacade *facade, qint32 number )
{
    if( facade->target() != "earth" ) {
        return;
    }

    QString geonamesUrl( "http://ws.geonames.org/earthquakesJSON" );
    geonamesUrl += "?north="   + QString::number( box.north() * RAD2DEG );
    geonamesUrl += "&south="   + QString::number( box.south() * RAD2DEG );
    geonamesUrl += "&east="    + QString::number( box.east() * RAD2DEG );
    geonamesUrl += "&west="    + QString::number( box.west() * RAD2DEG );
    geonamesUrl += "&date=" + QDate::currentDate().addMonths( -1 ).toString( "yyyy-MM-dd" );
    geonamesUrl += "&maxRows=" + QString::number( number );
    geonamesUrl += "&formatted=true";
    downloadDescriptionFile( QUrl( geonamesUrl ) );
}

void EarthquakeModel::parseFile( const QByteArray& file )
{
    QScriptValue data;
    QScriptEngine engine;

    // Qt requires parentheses around json code
    data = engine.evaluate( "(" + QString( file ) + ")" );

    // Parse if any result exists
    if ( data.property( "earthquakes" ).isArray() ) {
        QScriptValueIterator iterator( data.property( "earthquakes" ) );
        // Add items to the list
        while ( iterator.hasNext() ) {
            iterator.next();
            // Converting earthquake's properties from QScriptValue to appropriate types
            QString eqid = iterator.value().property( "eqid" ).toString(); // Earthquake's ID
            double longitude = iterator.value().property( "lng" ).toNumber();
            double latitude = iterator.value().property( "lat" ).toNumber();
            double magnitude = iterator.value().property( "magnitude" ).toNumber();

            if( !itemExists( eqid ) ) {
                // If it does not exists, create it
                GeoDataCoordinates coordinates( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
                EarthquakeItem *item = new EarthquakeItem( this );
                item->setId( eqid );
                item->setCoordinate( coordinates );
                item->setTarget( "earth" );
                item->setMagnitude( magnitude );
                addItemToList( item );
            }
        }
    }
}

}

#include "EarthquakeModel.moc"
