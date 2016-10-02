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
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "MarbleDebug.h"
#include "StyleBuilder.h"
#include "osm/OsmPlacemarkData.h"

#include <QIODevice>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


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
    QJsonParseError error;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(device->readAll(), &error);

    if (jsonDoc.isNull()) {
        qDebug() << "Error parsing GeoJSON : " << error.errorString();
        return false;
    }

    // Start parsing
    const QJsonValue featuresValue = jsonDoc.object().value(QStringLiteral("features"));

    // In GeoJSON format, geometries are stored in features, so we iterate on features
    if (featuresValue.isArray()) {
        const QJsonArray featureArray = featuresValue.toArray();

        // Parse each feature
        for (int featureIndex = 0; featureIndex < featureArray.size(); ++featureIndex) {
            const QJsonObject featureObject = featureArray[featureIndex].toObject();

            // Check if the feature contains a geometry
            const QJsonValue geometryValue = featureObject.value(QStringLiteral("geometry"));
            if (geometryValue.isObject()) {
                const QJsonObject geometryObject = geometryValue.toObject();

                // Variables for creating the geometry
                QList<GeoDataGeometry*> geometryList;
                QList<GeoDataPlacemark*> placemarkList;

                // Create the different geometry types
                const QString geometryType = geometryObject.value(QStringLiteral("type")).toString().toUpper();

                if (geometryType == QLatin1String("POLYGON")) {
                    // Check first that there are coordinates
                    const QJsonValue coordinatesValue = geometryObject.value(QStringLiteral("coordinates"));
                    if (coordinatesValue.isArray()) {
                        const QJsonArray coordinateArray = coordinatesValue.toArray();

                        GeoDataPolygon * geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );

                        // Coordinates first array will be the outer boundary, if there are more
                        // positions those will be inner holes
                        for (int ringIndex = 0 ; ringIndex < coordinateArray.size(); ++ringIndex) {
                            const QJsonArray ringArray = coordinateArray[ringIndex].toArray();

                            GeoDataLinearRing linearRing;

                            for (int coordinatePairIndex = 0; coordinatePairIndex < ringArray.size(); ++coordinatePairIndex) {
                                const QJsonArray coordinatePairArray = ringArray[coordinatePairIndex].toArray();

                                const qreal longitude = coordinatePairArray.at(0).toDouble();
                                const qreal latitude = coordinatePairArray.at(1).toDouble();

                                linearRing.append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                            }

                            // Outer ring
                            if (ringIndex == 0) {
                                geom->setOuterBoundary( linearRing );
                            }
                            // Inner holes
                            else {
                                geom->appendInnerBoundary( linearRing );
                            }
                        }
                        geometryList.append( geom );
                    }

                } else if (geometryType == QLatin1String("MULTIPOLYGON")) {
                    // Check first that there are coordinates
                    const QJsonValue coordinatesValue = geometryObject.value(QStringLiteral("coordinates"));
                    if (coordinatesValue.isArray()) {
                        const QJsonArray coordinateArray = coordinatesValue.toArray();

                        for (int polygonIndex = 0; polygonIndex < coordinateArray.size(); ++polygonIndex) {
                            const QJsonArray polygonArray = coordinateArray[polygonIndex].toArray();

                            GeoDataPolygon * geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );

                            // Coordinates first array will be the outer boundary, if there are more
                            // positions those will be inner holes
                            for (int ringIndex = 0 ; ringIndex < polygonArray.size(); ++ringIndex) {
                                const QJsonArray ringArray = polygonArray[ringIndex].toArray();

                                GeoDataLinearRing linearRing;

                                for (int coordinatePairIndex = 0; coordinatePairIndex < ringArray.size(); ++coordinatePairIndex) {
                                    const QJsonArray coordinatePairArray = ringArray[coordinatePairIndex].toArray();

                                    const qreal longitude = coordinatePairArray.at(0).toDouble();
                                    const qreal latitude = coordinatePairArray.at(1).toDouble();

                                    linearRing.append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                                }

                                // Outer ring
                                if (ringIndex == 0) {
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

                } else if (geometryType == QLatin1String("LINESTRING")) {

                    // Check first that there are coordinates
                    const QJsonValue coordinatesValue = geometryObject.value(QStringLiteral("coordinates"));
                    if (coordinatesValue.isArray()) {
                        const QJsonArray coordinateArray = coordinatesValue.toArray();

                        GeoDataLineString * geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );

                        for (int coordinatePairIndex = 0; coordinatePairIndex < coordinateArray.size(); ++coordinatePairIndex) {
                            const QJsonArray coordinatePairArray = coordinateArray[coordinatePairIndex].toArray();

                            const qreal longitude = coordinatePairArray.at(0).toDouble();
                            const qreal latitude = coordinatePairArray.at(1).toDouble();

                            geom->append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                        }
                        geometryList.append( geom );
                    }

                } else if (geometryType == QLatin1String("MULTILINESTRING")) {

                    // Check first that there are coordinates
                    const QJsonValue coordinatesValue = geometryObject.value(QStringLiteral("coordinates"));
                    if (coordinatesValue.isArray()) {
                        const QJsonArray coordinateArray = coordinatesValue.toArray();

                        for (int lineStringIndex = 0; lineStringIndex < coordinateArray.size(); ++lineStringIndex) {
                            const QJsonArray lineStringArray = coordinateArray[lineStringIndex].toArray();

                            GeoDataLineString * geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );

                            for (int coordinatePairIndex = 0; coordinatePairIndex < lineStringArray.size(); ++coordinatePairIndex) {
                                const QJsonArray coordinatePairArray = lineStringArray[coordinatePairIndex].toArray();

                                const qreal longitude = coordinatePairArray.at(0).toDouble();
                                const qreal latitude = coordinatePairArray.at(1).toDouble();

                                geom->append( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );
                            }
                            geometryList.append( geom );
                        }
                    }

                } else if (geometryType == QLatin1String("POINT")) {

                    // Check first that there are coordinates
                    const QJsonValue coordinatesValue = geometryObject.value(QStringLiteral("coordinates"));
                    if (coordinatesValue.isArray()) {
                        const QJsonArray coordinatePairArray = coordinatesValue.toArray();

                        GeoDataPoint * geom = new GeoDataPoint();

                        const qreal longitude = coordinatePairArray.at(0).toDouble();
                        const qreal latitude = coordinatePairArray.at(1).toDouble();

                        geom->setCoordinates( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );

                        geometryList.append( geom );
                    }
                } else if (geometryType == QLatin1String("MULTIPOINT")) {

                    // Check first that there are coordinates
                    const QJsonValue coordinatesValue = geometryObject.value(QStringLiteral("coordinates"));
                    if (coordinatesValue.isArray()) {
                        const QJsonArray coordinateArray = coordinatesValue.toArray();

                        for (int pointIndex = 0; pointIndex < coordinateArray.size(); ++pointIndex) {
                            const QJsonArray coordinatePairArray = coordinateArray[pointIndex].toArray();

                            GeoDataPoint * geom = new GeoDataPoint();

                            const qreal longitude = coordinatePairArray.at(0).toDouble();
                            const qreal latitude = coordinatePairArray.at(1).toDouble();

                            geom->setCoordinates( GeoDataCoordinates( longitude , latitude , 0 , GeoDataCoordinates::Degree ) );

                            geometryList.append( geom );
                        }
                    }
                }


                // Parse the features properties
                const QJsonValue propertiesValue = featureObject.value(QStringLiteral("properties"));
                if (!geometryList.isEmpty() && propertiesValue.isObject()) {
                    const QJsonObject propertiesObject = propertiesValue.toObject();

                    // First create a placemark for each geometry, there could be multi geometries
                    // that are translated into more than one geometry/placemark
                    for ( int numberGeometries = 0 ; numberGeometries < geometryList.length() ; numberGeometries++ ) {
                        GeoDataPlacemark * placemark = new GeoDataPlacemark();
                        placemarkList.append( placemark );
                    }

                    OsmPlacemarkData osmData;

                    QJsonObject::ConstIterator it = propertiesObject.begin();
                    const QJsonObject::ConstIterator end = propertiesObject.end();
                    for ( ; it != end; ++it) {
                        if (it.value().isObject() || it.value().isArray()) {
                            qDebug() << "Skipping property, values of type arrays and objects not supported:" << it.key();
                            continue;
                        }

                        // pass value through QVariant to also get bool & numbers
                        osmData.addTag(it.key(), it.value().toVariant().toString());
                    }

                    // If the property read, is the features name
                    const auto tagIter = osmData.findTag(QStringLiteral("name"));
                    if (tagIter != osmData.tagsEnd()) {
                        const QString& name = tagIter.value();
                        for (int pl = 0 ; pl < placemarkList.length(); ++pl) {
                            placemarkList.at(pl)->setName(name);
                        }
                    }

                    const GeoDataPlacemark::GeoDataVisualCategory category = StyleBuilder::determineVisualCategory(osmData);
                    if (category != GeoDataPlacemark::None) {
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

