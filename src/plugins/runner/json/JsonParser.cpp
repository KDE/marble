/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "JsonParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "MarbleDebug.h"
#include "StyleBuilder.h"
#include "osm/OsmPlacemarkData.h"

#include <QIODevice>


namespace Marble {

JsonParser::JsonParser() : m_document( 0 )
{
}

JsonParser::~JsonParser()
{
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

    // Read file data
    QString fileData = QString::fromUtf8( device->readAll() );

    // Create JSON parsing engine
    // This engine evaluates and returns data from JSON
    QScriptEngine m_engine;

    // Store the data into JSON for better managing
    m_engine.evaluate( "var fileData = " + fileData );

    if (m_engine.hasUncaughtException()) {
        mDebug() << "Error parsing GeoJSON : " << m_engine.uncaughtException().toString();
        return false;
    }

    // Start parsing

    // In GeoJSON format, geometries are stored in features, so we iterate on features
    if ( m_engine.evaluate( "fileData.features" ).isArray() ) {

        // Parse each feature
        for ( int featureCounter = 0 ; featureCounter < m_engine.evaluate( "fileData.features.length" ).toNumber() ; featureCounter ++) {

            QString count = QString::number(featureCounter);

            // Check if the feature contains a geometry
            if ( !m_engine.evaluate( "fileData.features[" + count + "].geometry" ).isUndefined() ) {

                // Variables for creating the geometry
                QList<GeoDataGeometry*> geometryList;
                QList<GeoDataPlacemark*> placemarkList;

                // Create the different geometry types

                if (m_engine.evaluate( "fileData.features[" + count + "].geometry.type" ).toString().toUpper() == QLatin1String("POLYGON")) {

                    // Check first that there are coordinates
                    if ( m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates" ).isArray() ) {

                        GeoDataPolygon * geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );

                        // Coordinates first array will be the outer boundary, if there are more
                        // positions those will be inner holes
                        int ringsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates.length" ).toNumber();
                        for ( int rings = 0 ; rings < ringsCount ; rings ++ ) {

                            QString ringCount = QString::number(rings);
                            GeoDataLinearRing linearRing;

                            int coordinatePairsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[ " + ringCount + " ].length" ).toNumber();
                            for( int coordinatePairs = 0 ; coordinatePairs < coordinatePairsCount ; coordinatePairs++ ) {

                                QString coors = QString::number(coordinatePairs);
                                qreal longitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + ringCount + "][" + coors + "][0]" ).toNumber();
                                qreal latitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + ringCount + "][" + coors + "][1]" ).toNumber();

                                linearRing.append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                            }

                            // Outer ring
                            if (rings == 0) {
                                geom->setOuterBoundary( linearRing );
                            }
                            // Inner holes
                            else {
                                geom->appendInnerBoundary( linearRing );
                            }
                        }
                        geometryList.append( geom );
                    }

                } else if (m_engine.evaluate( "fileData.features[" + count + "].geometry.type" ).toString().toUpper() == QLatin1String("MULTIPOLYGON")) {

                    // Check first that there are coordinates
                    if ( m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates" ).isArray() ) {

                        int polygonsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates.length" ).toNumber();
                        for ( int polygons = 0 ; polygons < polygonsCount ; polygons++ ) {

                            QString polygon = QString::number( polygons );
                            GeoDataPolygon * geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );

                            // Coordinates first array will be the outer boundary, if there are more
                            // positions those will be inner holes
                            int ringsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + polygon + "].length" ).toNumber();
                            for ( int rings = 0 ; rings < ringsCount ; rings ++ ) {

                                QString ringCount = QString::number(rings);
                                GeoDataLinearRing linearRing;

                                int coordinatePairsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + polygon + "][ " + ringCount + " ].length" ).toNumber();
                                for( int coordinatePairs = 0 ; coordinatePairs < coordinatePairsCount ; coordinatePairs++ ) {

                                    QString coors = QString::number(coordinatePairs);
                                    qreal longitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + polygon + "][" + ringCount + "][" + coors + "][0]" ).toNumber();
                                    qreal latitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + polygon + "][" + ringCount + "][" + coors + "][1]" ).toNumber();

                                    linearRing.append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                                }

                                // Outer ring
                                if (rings == 0) {
                                    geom->setOuterBoundary( linearRing );
                                }
                                // Inner holes
                                else {
                                    geom->appendInnerBoundary( linearRing );
                                }
                            }
                            geometryList.append( geom );
                        }
                    }

                } else if (m_engine.evaluate( "fileData.features[" + count + "].geometry.type" ).toString().toUpper() == QLatin1String("LINESTRING")) {

                    // Check first that there are coordinates
                    if ( m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates" ).isArray() ) {

                        GeoDataLineString * geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );

                        int coordinatePairsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates.length" ).toNumber();
                        for( int coordinatePairs = 0 ; coordinatePairs < coordinatePairsCount ; coordinatePairs++ ) {

                            QString coors = QString::number(coordinatePairs);

                            qreal longitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + coors + "][0]" ).toNumber();
                            qreal latitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + coors + "][1]" ).toNumber();

                            geom->append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                        }
                        geometryList.append( geom );
                    }

                } else if (m_engine.evaluate( "fileData.features[" + count + "].geometry.type" ).toString().toUpper() == QLatin1String("MULTILINESTRING")) {

                    // Check first that there are coordinates
                    if ( m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates" ).isArray() ) {

                        int linestringsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates.length" ).toNumber();
                        for( int linestrings = 0 ; linestrings < linestringsCount ; linestrings++ ) {

                            QString linestring = QString::number( linestrings );
                            GeoDataLineString * geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );

                            int coordinatePairsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + linestring + "].length" ).toNumber();
                            for( int coordinatePairs = 0 ; coordinatePairs < coordinatePairsCount ; coordinatePairs++ ) {

                                QString coors = QString::number(coordinatePairs);

                                qreal longitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + linestring + "][" + coors + "][0]" ).toNumber();
                                qreal latitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + linestring + "][" + coors + "][1]" ).toNumber();

                                geom->append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                            }
                            geometryList.append( geom );
                        }
                    }

                } else if (m_engine.evaluate( "fileData.features[" + count + "].geometry.type" ).toString().toUpper() == QLatin1String("POINT")) {

                    // Check first that there are coordinates
                    if ( m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates" ).isArray() ) {

                        GeoDataPoint * geom = new GeoDataPoint();

                        qreal longitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[0]" ).toNumber();
                        qreal latitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[1]" ).toNumber();

                        geom->setCoordinates( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );

                        geometryList.append( geom );
                    }
                } else if (m_engine.evaluate( "fileData.features[" + count + "].geometry.type" ).toString().toUpper() == QLatin1String("MULTIPOINT")) {

                    // Check first that there are coordinates
                    if ( m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates" ).isArray() ) {

                        int pointsCount = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates.length" ).toNumber();
                        for( int points = 0 ; points < pointsCount ; points++ ) {

                            QString point = QString::number(points);

                            GeoDataPoint * geom = new GeoDataPoint();

                            qreal longitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + point + "][0]" ).toNumber();
                            qreal latitude = m_engine.evaluate( "fileData.features[" + count + "].geometry.coordinates[" + point + "][1]" ).toNumber();

                            geom->setCoordinates( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );

                            geometryList.append( geom );
                        }
                    }
                }


                // Parse the features properties
                if ( !geometryList.isEmpty() != 0 && !m_engine.evaluate( "fileData.features[" + count + "].properties" ).isUndefined() ) {

                    QScriptValue properties = m_engine.evaluate( "fileData.features[" + count + "].properties" );
                    QScriptValueIterator propertyIterator ( properties );

                    // First create a placemark for each geometry, there could be multi geometries
                    // that are translated into more than one geometry/placemark
                    for ( int numberGeometries = 0 ; numberGeometries < geometryList.length() ; numberGeometries++ ) {
                        GeoDataPlacemark * placemark = new GeoDataPlacemark();
                        placemarkList.append( placemark );
                    }

                    OsmPlacemarkData osmData;
                    while ( propertyIterator.hasNext() ) {
                        propertyIterator.next();
                        osmData.addTag(propertyIterator.name(), propertyIterator.value().toString());
                    }

                    // If the property read, is the features name
                    if (osmData.containsTagKey(QLatin1String("name"))) {
                        for (int pl = 0 ; pl < placemarkList.length(); ++pl) {
                            placemarkList.at(pl)->setName(osmData.tagValue(QLatin1String("name")));
                        }
                    }

                    const GeoDataFeature::GeoDataVisualCategory category = StyleBuilder::determineVisualCategory(osmData);
                    if (category != GeoDataFeature::None) {
                        // Add the visual category to all the placemarks
                        for (int pl = 0 ; pl < placemarkList.length(); ++pl) {
                            placemarkList.at(pl)->setVisualCategory(category);
                            placemarkList.at(pl)->setOsmData(osmData);
                        }
                    }
                }

                // Add the geometry to the document
                if ( geometryList.length() == placemarkList.length() ) {

                    while( placemarkList.length() > 0 ) {

                        GeoDataPlacemark * placemark = placemarkList.last();
                        placemarkList.pop_back();

                        GeoDataGeometry * geom = geometryList.last();
                        geometryList.pop_back();

                        placemark->setGeometry( geom );
                        placemark->setVisible( true );
                        m_document->append( placemark );
                    }
                }

                // If geometries or placemarks missing inside the lists, delete them
                qDeleteAll( geometryList.begin(), geometryList.end() );
                geometryList.clear();
                qDeleteAll( placemarkList.begin(), placemarkList.end() );
                placemarkList.clear();
            }
        }
    }
    return true;
}

}

