/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "JsonParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"

#include "MarbleDebug.h"

#include <QIODevice>


namespace Marble {

JsonParser::JsonParser() : m_document( 0 )
{
}

JsonParser::~JsonParser()
{
    m_data = 0;
    delete m_document;
}

GeoDataDocument *JsonParser::releaseDocument()
{
    GeoDataDocument* document = m_document;
    m_document = 0;
    return document;
}

bool JsonParser::read( QIODevice* device )
{
    // Assert previous document got released.
    delete m_document;
    m_document = new GeoDataDocument;
    Q_ASSERT( m_document );

    // Fixes for test parsing
    device->seek(21); // Strip off 'onKothicDataRespone('
    QString temp = QString::fromUtf8( device->readAll() );
    int midIndex = temp.size();
    int rightIndex = midIndex;
    for ( int i=0; i<4; ++i ) {
        rightIndex = midIndex;
        midIndex = temp.lastIndexOf( ',', midIndex-1 );
        if ( i==1 ) {
            QString name = temp.mid( midIndex-1 );
            name.remove( name.size()-2,2 );
            m_document->setName( "Kothic " + name );
        }
    }

    QString stream = temp.mid(0, midIndex);
    stream.prepend('(');
    stream.append("})");
    bool hasGranularity = false;
    int const granularity = temp.mid(midIndex+15, rightIndex-midIndex-16).toInt( &hasGranularity );
    if (!hasGranularity) {
        mDebug() << "Cannot parse json file (failed to parse granularity) " << temp;
        return false;
    }

    /** THIS IS A TEST PARSER FOR KOTHIK's JSON FORMAT **/

    m_data = m_engine.evaluate( stream );
    if (m_engine.hasUncaughtException()) {
        mDebug() << "Cannot parse json file: " << m_engine.uncaughtException().toString();
        return false;
    }

    // Start parsing
    GeoDataPlacemark *placemark;
    GeoDataFeature::GeoDataVisualCategory category;

    // Bounding box coordinates
    float east;
    float south;
    float west;
    float north;

    // Global data (even if it is at the end of the json response
    // it is posible to read it now)

    if ( m_data.property( "bbox" ).isArray() ){
        QStringList coors = m_data.property( "bbox" ).toString().split(",");

        // Load the bounding box coordinates
        west  = coors.at(0).toFloat();
        east  = coors.at(2).toFloat();
        south = coors.at(1).toFloat();
        north = coors.at(3).toFloat();
    }
    else{
        mDebug() << "Cannot parse bbox";
        return false;
    }

    //  All downloaded placemarks will be features, so we should iterate
    //  on features
    QScriptValue const features = m_data.property( "features" );
    if (features.isArray()){
        QScriptValueIterator iterator( features );

        // Add items to the list
        while ( iterator.hasNext() ) {
            iterator.next();

            GeoDataGeometry * geom;
            placemark = new GeoDataPlacemark();

            QString const typeProperty = iterator.value().property( "type" ).toString();
            if ( typeProperty == "Polygon" ){
                geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );
            } else if ( typeProperty == "LineString" ){
                    geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );
            } else if ( typeProperty == "Point" ){
                geom = new GeoDataPoint();
            } else
                    geom = 0;

            QScriptValueIterator it (iterator.value().property( "properties" ));

            bool propertiesCorrect = false;

            // Parsing properties
            while ( it.hasNext() && geom != 0 ) {
                it.next();

                if ( it.name() == "name" ){
                    placemark->setName( it.value().toString() );
                }else if ( !propertiesCorrect ){
                    category = GeoDataFeature::OsmVisualCategory( it.name() + "=" + it.value().toString() );

                    if (category != 0){
                        placemark->setVisualCategory( category );
                        propertiesCorrect = true;
                    }
                }
            }

            // Parsing coordinates

            QScriptValue const coordinatesProperty = iterator.value().property( "coordinates" );
            if ( coordinatesProperty.isArray() ){

                QScriptValueIterator it ( coordinatesProperty );

                while ( it.hasNext() ) {
                    it.next();

                    QStringList coors = it.value().toString().split(",");
                    for (int x = 0; x < coors.size()-1 && coors.size()>1 ;){

                        float auxX = ( coors.at(x++).toFloat() / granularity)*(east-west)   + west;
                        float auxY = ( coors.at(x++).toFloat() / granularity)*(north-south) + south;


                        QString const typeProperty = iterator.value().property( "type" ).toString();
                            if (typeProperty == "Polygon"){

                                GeoDataLinearRing ring = ((GeoDataPolygon*)geom)->outerBoundary();
                                ring.append( GeoDataCoordinates(auxX, auxY,0, GeoDataCoordinates::Degree ) );

                                // FIXME appending to the ring could be done more efficiently

                                ((GeoDataPolygon*)geom)->setOuterBoundary(ring);
                            }
                            else
                                if (typeProperty == "LineString"){
                                    ((GeoDataLineString*) geom)->append( GeoDataCoordinates(auxX, auxY,0, GeoDataCoordinates::Degree ) );
                           } else if (typeProperty == "Point"){
                                    ((GeoDataPoint*) geom)->set( auxX,auxY,0, GeoDataCoordinates::Degree );
                           }

                    }
                }
            }

            if ( propertiesCorrect && geom != 0 ){
                placemark->setGeometry( geom );
                placemark->setVisible( true );
                m_document->append( placemark );
            }
        }
    }
    return true;
}

}

