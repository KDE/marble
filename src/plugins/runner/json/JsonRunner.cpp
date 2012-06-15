//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
//
// GSoC 2012           Ander Pijoan <ander.pijoan@deusto.es

#include "JsonRunner.h"

#include "GeoDataDocument.h"
#include "MarbleDebug.h"

// For json parser
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

#include <QtCore/QFile>

namespace Marble
{

JsonRunner::JsonRunner(QObject *parent) :
    MarbleAbstractRunner(parent)
{
}

JsonRunner::~JsonRunner()
{
}

GeoDataFeature::GeoDataVisualCategory JsonRunner::category() const
{
    return GeoDataFeature::Folder;
}

GeoDocument* JsonRunner::createDocument() const
{
    return new GeoDataDocument;
}

void JsonRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFile  file( fileName );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        emit parsingFinished( 0 );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );
    QByteArray result = file.readAll();

    // json parsing engine
    QScriptValue data;
    QScriptEngine engine;

    if ( !engine.canEvaluate( "(" + result + ")" ) ) {
        emit parsingFinished( 0, "parser.errorString()" );
        return;
    }

    m_document = createDocument();
    Q_ASSERT( m_document );
    GeoDataDocument* doc = static_cast<GeoDataDocument*>( m_document );
    doc->setDocumentRole( role );
    GeoDataPlacemark *placemark = new GeoDataPlacemark();

    float east;
    float south;
    float west;
    float north;

    data = engine.evaluate( "(" + result + ")" );

    // Global data (even if it is at the end of the json response
    // it is posible to read it now)

    if ( data.property( "bbox" ).isArray() ){
    QStringList coors = data.property( "bbox" ).toString().split(",");;

    // Create bounding box ring

    east = coors.at(0).toFloat();
    south = coors.at(1).toFloat();
    west = coors.at(2).toFloat();
    north = coors.at(3).toFloat();

    GeoDataPoint *point = new GeoDataPoint();
    point->set( east, south, 0, GeoDataCoordinates::Degree );
    placemark->setGeometry( point );
    placemark->setVisible( true );
    doc->append( placemark );

    point = new GeoDataPoint();
    point->set( east, north , 0, GeoDataCoordinates::Degree );
    placemark->setGeometry( point );
    placemark->setVisible( true );
    doc->append( placemark );

    point = new GeoDataPoint();
    point->set( west, north , 0, GeoDataCoordinates::Degree );
    placemark->setGeometry( point );
    placemark->setVisible( true );
    doc->append( placemark );

    point = new GeoDataPoint();
    point->set( west, south , 0, GeoDataCoordinates::Degree );
    placemark->setGeometry( point );
    placemark->setVisible( true );
    doc->append( placemark );

    }

    //  All downloaded placemarks will be features, so we should iterate
    //  on features
    if (data.property( "features" ).isArray()){

        QScriptValueIterator iterator( data.property( "features" ) );

        // Add items to the list
        while ( iterator.hasNext() ) {
            iterator.next();

            mDebug() << "------------------------" << iterator.value().property( "type" ).toString();

            QScriptValueIterator it (iterator.value().property( "properties" ));

            mDebug() << "------------------------Properties";
            while ( it.hasNext() ) {
                it.next();

                mDebug() <<  it.name() << it.value().toString();
            }

            mDebug() << "------------------------Coordinates";
            if ( iterator.value().property( "coordinates" ).isArray() ){

                QScriptValueIterator it (iterator.value().property( "coordinates" ));

                while ( it.hasNext() ) {
                    it.next();

                    QStringList coors = it.value().toString().split(",");
                    for (int x = 0; x < coors.size()-1 && coors.size()>1 ;){

                        GeoDataPoint *point = new GeoDataPoint();
                        point->set( coors.at(x++).toFloat(), coors.at(x++).toFloat(), 0, GeoDataCoordinates::Degree );
                        placemark->setGeometry( point );
                        placemark->setVisible( true );
                        doc->append( placemark );
                    }
                }
            }
        }

        file.close();
        emit parsingFinished( doc );
    }
}

}

#include "JsonRunner.moc"
