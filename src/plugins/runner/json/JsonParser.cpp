//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

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
}

GeoDataDocument* JsonParser::createDocument() const
{
    return new GeoDataDocument;
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
    Q_ASSERT( !m_document );
    m_document = createDocument();
    Q_ASSERT( m_document );

    // Fixes for test parsing
    QString temp = device->readAll().toLower();
    QStringList temps = temp.split(",\"granularity\":10000");
    QByteArray stream (temps.at(0).toAscii());
    stream.append("}");
    stream.replace("onkothicdataresponse(","");

    // For json add '(' and ')' to the stream
    if ( !m_engine.canEvaluate( "(" + stream + ")" ) ) {
        return false;
    }

    /**
     * FIXME ANDER THIS IS A TEST PARSER FOR KOTHIK's JSON FORMAT
     **/

    // Start parsing
    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    GeoDataFeature::GeoDataVisualCategory category;

    float east = 1;
    float south = -1;
    float west = -1;
    float north = 1;

    m_data = m_engine.evaluate( "(" + stream + ")" );

    // Global data (even if it is at the end of the json response
    // it is posible to read it now)

    if ( m_data.property( "bbox" ).isArray() ){
        QStringList coors = m_data.property( "bbox" ).toString().split(",");

        // Create a bounding box linearRing
        west  = coors.at(0).toFloat();
        east  = coors.at(2).toFloat();
        south = coors.at(1).toFloat();
        north = coors.at(3).toFloat();

//        GeoDataLinearRing *ring = new GeoDataLinearRing();

//        ring->append( GeoDataCoordinates( east, north, 0, GeoDataCoordinates::Degree ) );

//        ring->append( GeoDataCoordinates( east, south, 0, GeoDataCoordinates::Degree ) );

//        ring->append( GeoDataCoordinates( west, south, 0, GeoDataCoordinates::Degree ) );

//        ring->append( GeoDataCoordinates( west, north, 0, GeoDataCoordinates::Degree ) );

//        placemark = new GeoDataPlacemark();
//        placemark->setGeometry( ring );
//        placemark->setVisualCategory( GeoDataPlacemark::None );
//        placemark->setVisible( true );
//        m_document->append( placemark );

    }

    //  All downloaded placemarks will be features, so we should iterate
    //  on features
    if (m_data.property( "features" ).isArray()){

        QScriptValueIterator iterator( m_data.property( "features" ) );

        // Add items to the list
        while ( iterator.hasNext() ) {
            iterator.next();

            GeoDataGeometry * geom;
            placemark = new GeoDataPlacemark();


            if (iterator.value().property( "type" ).toString().toLower() == "polygon")
                geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );
            else
            if (iterator.value().property( "type" ).toString().toLower() == "linestring")
                geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );
            else
                geom = new GeoDataGeometry( );

            QScriptValueIterator it (iterator.value().property( "properties" ));

            bool c = false;

            // Parsing properties
            while ( it.hasNext() && !c ) {
                it.next();

                if ( it.name() == "name" )
                    placemark->setName( it.value().toString() );

                category = GeoDataFeature::OsmVisualCategory( it.name() + "=" + it.value().toString() );
                if (category != 0){
                placemark->setVisualCategory( category );
                c = true;
                }
            }

            // Parsing coordinates
            bool g = true;

            if ( iterator.value().property( "coordinates" ).isArray() ){

                QScriptValueIterator it (iterator.value().property( "coordinates" ));

                while ( it.hasNext() ) {
                    it.next();

                    g = true;

                    QStringList coors = it.value().toString().split(",");
                    for (int x = 0; x < coors.size()-1 && coors.size()>1 && g ;){

                        float auxX = ( coors.at(x++).toFloat() / 10000)*(east-west)   + west;
                        float auxY = ( coors.at(x++).toFloat() / 10000)*(north-south) + south;


                            if (iterator.value().property( "type" ).toString().toLower() == "polygon"){

                                GeoDataLinearRing ring = ((GeoDataPolygon*)geom)->outerBoundary();
                                ring.append( GeoDataCoordinates(auxX, auxY,0, GeoDataCoordinates::Degree ) );

                                ((GeoDataPolygon*)geom)->setOuterBoundary(ring);
                            }
                            else
                        if (iterator.value().property( "type" ).toString().toLower() == "linestring")
                                ((GeoDataLineString*) geom)->append( GeoDataCoordinates(auxX, auxY,0, GeoDataCoordinates::Degree ) );
//                            else if (iterator.value().property( "type" ).toString().toLower() == "point")
//                                geom = new GeoDataPoint(
//                                    GeoDataCoordinates(auxX,auxY,0, GeoDataCoordinates::Degree ) );
                            else
                                g = false;
                    }
                }
            }

            if ( g && geom ){
                placemark->setGeometry( geom );
                placemark->setVisible( true );
                m_document->append( placemark );
            }
        }
    }
    temp.clear();
    stream.clear();
    return true;
}

}

