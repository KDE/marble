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
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtGui/QMessageBox>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

namespace Marble {

EarthquakeModel::EarthquakeModel( PluginManager *pluginManager, QObject *parent )
    : AbstractDataPluginModel( "earthquake", pluginManager, parent ),
      m_numResults( numberOfItemsOnScreen ),
      m_minMagnitude( 0.0 ),
      m_startDate( QDateTime::fromString( "2006-02-04", "yyyy-MM-dd" ) ),
      m_endDate( QDateTime::currentDateTime() )
{
    // nothing to do
}

EarthquakeModel::~EarthquakeModel()
{
}

void EarthquakeModel::setNumResults( int numResults )
{
    m_numResults = numResults;
}

void EarthquakeModel::setMinMagnitude( double minMagnitude )
{
    m_minMagnitude = minMagnitude;
}

void EarthquakeModel::setStartDate( const QDateTime& startDate )
{
    m_startDate = startDate;
}

void EarthquakeModel::setEndDate( const QDateTime& endDate )
{
    m_endDate = endDate;
}

void EarthquakeModel::getAdditionalItems( const GeoDataLatLonAltBox& box, const MarbleModel *model, qint32 number )
{
    Q_UNUSED( number );

    if( model->planetId() != "earth" ) {
        return;
    }

    QString geonamesUrl( "http://ws.geonames.org/earthquakesJSON" );
    geonamesUrl += "?north="   + QString::number( box.north() * RAD2DEG );
    geonamesUrl += "&south="   + QString::number( box.south() * RAD2DEG );
    geonamesUrl += "&east="    + QString::number( box.east() * RAD2DEG );
    geonamesUrl += "&west="    + QString::number( box.west() * RAD2DEG );
    geonamesUrl += "&date=" + m_endDate.toString( "yyyy-MM-dd" );
    geonamesUrl += "&maxRows=" + QString::number( m_numResults );
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
            QString data = iterator.value().property( "datetime" ).toString();
            QDateTime date = QDateTime::fromString( data, "yyyy-MM-dd hh:mm:ss" );
            double depth = iterator.value().property( "depth" ).toNumber();

            if( date <= m_endDate && date >= m_startDate && magnitude >= m_minMagnitude ) {
                if( !itemExists( eqid ) ) {
                    // If it does not exists, create it
                    GeoDataCoordinates coordinates( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
                    EarthquakeItem *item = new EarthquakeItem( this );
                    item->setId( eqid );
                    item->setCoordinate( coordinates );
                    item->setTarget( "earth" );
                    item->setMagnitude( magnitude );
                    item->setDateTime( date );
                    item->setDepth( depth );

                    addItemToList( item );
                }
            }
        }
    }
}


}

#include "EarthquakeModel.moc"
