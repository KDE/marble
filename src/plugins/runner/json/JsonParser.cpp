/*
    This file is part of the Marble Virtual Globe.

    The JsonParser class reads in a GeoJSON document that conforms to
    RFC7946 (including relevant errata).  Attributes are stored as OSM
    tags.

    This program is free software licensed under the GNU LGPL. You can
    find a copy of this license in LICENSE.txt in the top directory of
    the source code.

    Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
    Copyright 2019 John Zaitseff <J.Zaitseff@zap.org.au>
*/

#include "JsonParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataMultiGeometry.h"
#include "MarbleDebug.h"
#include "StyleBuilder.h"
#include "osm/OsmPlacemarkData.h"

#include <QIODevice>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


namespace Marble {

JsonParser::JsonParser() : m_document( nullptr )
{
}

JsonParser::~JsonParser()
{
    delete m_document;
}

GeoDataDocument *JsonParser::releaseDocument()
{
    GeoDataDocument* document = m_document;
    m_document = nullptr;
    return document;
}

bool JsonParser::read( QIODevice* device )
{
    // Release the previous document if required
    delete m_document;
    m_document = new GeoDataDocument;
    Q_ASSERT( m_document );

    // Read JSON file data
    QJsonParseError error;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(device->readAll(), &error);

    if (jsonDoc.isNull()) {
        qDebug() << "Error parsing GeoJSON:" << error.errorString();
        return false;
    } else if (! jsonDoc.isObject()) {
        qDebug() << "Invalid file, does not contain a GeoJSON object";
        return false;
    }

    // Valid GeoJSON documents may not always contain a FeatureCollection object with subsidiary
    // Feature objects, or even a single Feature object: they might contain just a single geometry
    // object.  Handle such cases by creating a wrapper Feature object if required.

    const QString jsonObjectType = jsonDoc.object().value(QStringLiteral("type")).toString();

    if (jsonObjectType == QStringLiteral("FeatureCollection")
        || jsonObjectType == QStringLiteral("Feature")) {

        // A normal GeoJSON document: parse it recursively
        return parseGeoJsonTopLevel(jsonDoc.object());

    } else {
        // Create a wrapper Feature object and parse that

        QJsonObject jsonWrapper;
        QJsonObject jsonWrapperProperties;

        jsonWrapper["type"] = QStringLiteral("Feature");
        jsonWrapper["geometry"] = jsonDoc.object();
        jsonWrapper["properties"] = jsonWrapperProperties;

        return parseGeoJsonTopLevel(jsonWrapper);
    }
}

bool JsonParser::parseGeoJsonTopLevel( const QJsonObject& jsonObject )
{
    // Every GeoJSON object must have a case-sensitive "type" member (see RFC7946 section 3)
    const QString jsonObjectType = jsonObject.value(QStringLiteral("type")).toString();

    if (jsonObjectType == QStringLiteral("FeatureCollection")) {
        // Handle the FeatureCollection object, which may contain multiple Feature objects in it

        const QJsonArray featureArray = jsonObject.value(QStringLiteral("features")).toArray();
        for (int featureIndex = 0; featureIndex < featureArray.size(); ++featureIndex) {
            if (! parseGeoJsonTopLevel( featureArray[featureIndex].toObject() )) {
                return false;
            }
        }
        return true;

    } else if (jsonObjectType == QStringLiteral("Feature")) {
        // Handle the Feature object, which contains a single geometry object and possibly
        // associated properties.  Note that only Feature objects can have recognised properties.

        QVector<GeoDataGeometry*> geometryList;     // Populated by parseGeoJsonSubLevel()

        if (! parseGeoJsonSubLevel( jsonObject.value(QStringLiteral("geometry")).toObject(),
                geometryList )) {
            return false;
        }

        // Create the placemark for this feature object with appropriate geometry

        GeoDataPlacemark* placemark = new GeoDataPlacemark();

        if (geometryList.length() < 1) {
            // No geometries available to add to the placemark
            ;

        } else if (geometryList.length() == 1) {
            // Single geometry
            placemark->setGeometry(geometryList[0]);

        } else {
            // Multiple geometries require a GeoDataMultiGeometry class

            GeoDataMultiGeometry* geom = new GeoDataMultiGeometry();
            for (int i = 0; i < geometryList.length(); ++i) {
                geom->append(geometryList[i]);
            }
            placemark->setGeometry(geom);
        }

        // Parse any associated properties

        const QJsonObject propertiesObject = jsonObject.value(QStringLiteral("properties")).toObject();
        QJsonObject::ConstIterator iter = propertiesObject.begin();
	const QJsonObject::ConstIterator end = propertiesObject.end();

	OsmPlacemarkData osmData;

	for ( ; iter != end; ++iter) {
	    // Pass the value through QVariant to also get booleans and numbers
	    const QString propertyValue = iter.value().toVariant().toString();
	    const QString propertyKey = iter.key();

	    if (iter.value().isObject() || iter.value().isArray()) {
		qDebug() << "Skipping unsupported JSON property containing an object or array:" << propertyKey;
		continue;
	    }

	    osmData.addTag(propertyKey, propertyValue);

	    if (propertyKey == QStringLiteral("name")) {
		placemark->setName(propertyValue);
	    }
	}

	placemark->setOsmData(osmData);
	placemark->setVisible(true);

	const GeoDataPlacemark::GeoDataVisualCategory category =
	    StyleBuilder::determineVisualCategory(osmData);
	if (category != GeoDataPlacemark::None) {
	    placemark->setVisualCategory(category);
	}

	m_document->append(placemark);
	return true;

    } else {
	qDebug() << "Missing FeatureCollection or Feature object in GeoJSON file";
	return false;
    }
}

bool JsonParser::parseGeoJsonSubLevel( const QJsonObject& jsonObject,
				       QVector<GeoDataGeometry*>& geometryList )
{
    // The GeoJSON object type
    const QString jsonObjectType = jsonObject.value(QStringLiteral("type")).toString();

    if (jsonObjectType == QStringLiteral("FeatureCollection")
	|| jsonObjectType == QStringLiteral("Feature")) {

	qDebug() << "Cannot have FeatureCollection or Feature objects at this level of the GeoJSON file";
	return false;

    } else if (jsonObjectType == QStringLiteral("GeometryCollection")) {
	// Handle the GeometryCollection object, which may contain multiple geometry objects

	const QJsonArray geometryArray = jsonObject.value(QStringLiteral("geometries")).toArray();
	for (int geometryIndex = 0; geometryIndex < geometryArray.size(); ++geometryIndex) {
	    if (! parseGeoJsonSubLevel( geometryArray[geometryIndex].toObject(), geometryList )) {
		return false;
	    }
	}

	return true;
    }

    // Handle remaining GeoJSON objects, which each have a "coordinates" member (an array)

    const QJsonArray coordinateArray = jsonObject.value(QStringLiteral("coordinates")).toArray();

    if (jsonObjectType == QStringLiteral("Point")) {
	// A Point object has a single GeoJSON position: an array of at least two values

	GeoDataPoint* geom = new GeoDataPoint();
	const qreal lon = coordinateArray.at(0).toDouble();
	const qreal lat = coordinateArray.at(1).toDouble();
	const qreal alt = coordinateArray.at(2).toDouble();	// If missing, uses 0 as the default

	geom->setCoordinates( GeoDataCoordinates( lon, lat, alt, GeoDataCoordinates::Degree ));
	geometryList.append(geom);

	return true;

    } else if (jsonObjectType == QStringLiteral("MultiPoint")) {
	// A MultiPoint object has an array of GeoJSON positions (ie, a two-level array)

	for (int positionIndex = 0; positionIndex < coordinateArray.size(); ++positionIndex) {
	    const QJsonArray positionArray = coordinateArray[positionIndex].toArray();

	    GeoDataPoint* geom = new GeoDataPoint();
	    const qreal lon = positionArray.at(0).toDouble();
	    const qreal lat = positionArray.at(1).toDouble();
	    const qreal alt = positionArray.at(2).toDouble();

	    geom->setCoordinates( GeoDataCoordinates( lon, lat, alt, GeoDataCoordinates::Degree ));
	    geometryList.append(geom);
	}

	return true;

    } else if (jsonObjectType == QStringLiteral("LineString")) {
	// A LineString object has an array of GeoJSON positions (ie, a two-level array)

	GeoDataLineString* geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );

	for (int positionIndex = 0; positionIndex < coordinateArray.size(); ++positionIndex) {
	    const QJsonArray positionArray = coordinateArray[positionIndex].toArray();

	    const qreal lon = positionArray.at(0).toDouble();
	    const qreal lat = positionArray.at(1).toDouble();
	    const qreal alt = positionArray.at(2).toDouble();

	    geom->append( GeoDataCoordinates( lon, lat, alt, GeoDataCoordinates::Degree ));
	}
	geometryList.append(geom);

	return true;

    } else if (jsonObjectType == QStringLiteral("MultiLineString")) {
	// A MultiLineString object has an array of arrays of GeoJSON positions (three-level)

	for (int lineStringIndex = 0; lineStringIndex < coordinateArray.size(); ++lineStringIndex) {
	    const QJsonArray lineStringArray = coordinateArray[lineStringIndex].toArray();

	    GeoDataLineString* geom = new GeoDataLineString( RespectLatitudeCircle | Tessellate );

	    for (int positionIndex = 0; positionIndex < lineStringArray.size(); ++positionIndex) {
		const QJsonArray positionArray = lineStringArray[positionIndex].toArray();

		const qreal lon = positionArray.at(0).toDouble();
		const qreal lat = positionArray.at(1).toDouble();
		const qreal alt = positionArray.at(2).toDouble();

		geom->append( GeoDataCoordinates( lon, lat, alt, GeoDataCoordinates::Degree ));
	    }
	    geometryList.append(geom);
	}

	return true;

    } else if (jsonObjectType == QStringLiteral("Polygon")) {
	// A Polygon object has an array of arrays of GeoJSON positions: the first array within the
	// top-level Polygon coordinates array is the outer boundary, following arrays are inner
	// holes (if any)

	GeoDataPolygon* geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );

	for (int ringIndex = 0; ringIndex < coordinateArray.size(); ++ringIndex) {
	    const QJsonArray ringArray = coordinateArray[ringIndex].toArray();

	    GeoDataLinearRing linearRing;

	    for (int positionIndex = 0; positionIndex < ringArray.size(); ++positionIndex) {
		const QJsonArray positionArray = ringArray[positionIndex].toArray();

		const qreal lon = positionArray.at(0).toDouble();
		const qreal lat = positionArray.at(1).toDouble();
		const qreal alt = positionArray.at(2).toDouble();

		linearRing.append( GeoDataCoordinates( lon, lat, alt, GeoDataCoordinates::Degree ));
	    }

	    if (ringIndex == 0) {
		// Outer boundary of the polygon
		geom->setOuterBoundary(linearRing);
	    } else {
		geom->appendInnerBoundary(linearRing);
	    }
	}
	geometryList.append(geom);

	return true;

    } else if (jsonObjectType == QStringLiteral("MultiPolygon")) {
	// A MultiPolygon object has an array of Polygon arrays (ie, a four-level array)

	for (int polygonIndex = 0; polygonIndex < coordinateArray.size(); ++polygonIndex) {
	    const QJsonArray polygonArray = coordinateArray[polygonIndex].toArray();

	    GeoDataPolygon* geom = new GeoDataPolygon( RespectLatitudeCircle | Tessellate );

	    for (int ringIndex = 0; ringIndex < polygonArray.size(); ++ringIndex) {
		const QJsonArray ringArray = polygonArray[ringIndex].toArray();

		GeoDataLinearRing linearRing;

		for (int positionIndex = 0; positionIndex < ringArray.size(); ++positionIndex) {
		    const QJsonArray positionArray = ringArray[positionIndex].toArray();

		    const qreal lon = positionArray.at(0).toDouble();
		    const qreal lat = positionArray.at(1).toDouble();
		    const qreal alt = positionArray.at(2).toDouble();

		    linearRing.append( GeoDataCoordinates( lon, lat, alt, GeoDataCoordinates::Degree ));
		}

		if (ringIndex == 0) {
		    // Outer boundary of the polygon
		    geom->setOuterBoundary(linearRing);
		} else {
		    geom->appendInnerBoundary(linearRing);
		}
	    }
	    geometryList.append(geom);
	}

	return true;

    } else if (jsonObjectType == QStringLiteral("")) {
	// Unlocated Feature objects have a null value for "geometry" (RFC7946 section 3.2)
	return true;

    } else {
	qDebug() << "Unknown GeoJSON object type" << jsonObjectType;
	return false;
    }
}

}
