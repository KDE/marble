/*
    This file is part of the Marble Virtual Globe.

    The JsonParser class reads in a GeoJSON document that conforms to
    RFC7946 (including relevant errata) and optionally contains
    attributes from the Simplestyle specification version 1.1.0
    ((https://github.com/mapbox/simplestyle-spec).  Attributes are also
    stored as OSM tags as required.

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
#include <QApplication>
#include <QPalette>
#include <QColor>

#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataPolyStyle.h"


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

	// Set default style properties using the Simplestyle specification 1.1.0

	GeoDataStyle::Ptr style(new GeoDataStyle(*(placemark->style())));
	GeoDataIconStyle iconStyle = style->iconStyle();
	GeoDataLabelStyle labelStyle = style->labelStyle();
	GeoDataLineStyle lineStyle = style->lineStyle();
	GeoDataPolyStyle polyStyle = style->polyStyle();

	// TODO: Handle "marker-size": "medium" and "marker-symbol": ""
	iconStyle.setColor(QColor("#ff7e7e7e"));  // "marker-color": "#7e7e7e"

	lineStyle.setColor(QColor("#ff555555"));  // "stroke": "#555555" and "stroke-opacity": 1.0
	lineStyle.setWidth(2.5);		  // "stroke-width": 2 (increased to 2.5 due to
						  // problems with antialiased lines disappearing

	polyStyle.setColor(QColor("#99555555"));  // "fill": "#555555" and "fill-opacity": 0.6

	// Set visual properties not part of the Simplestyle spec

	labelStyle.setColor(QApplication::palette().brightText().color());
	labelStyle.setGlow(true);

	polyStyle.setFill(true);
	polyStyle.setOutline(true);

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

	    if (propertyKey == QStringLiteral("name")) {
		// The "name" property is not defined in the Simplestyle specification, but is used
		// extensively in the wild.  Treat "name" and "title" essentially the same for the
		// purposes of placemarks (although osmData tags will preserve the distinction).

		placemark->setName(propertyValue);
		osmData.addTag(propertyKey, propertyValue);

	    } else if (propertyKey == QStringLiteral("title")) {
		placemark->setName(propertyValue);
		osmData.addTag(propertyKey, propertyValue);

	    } else if (propertyKey == QStringLiteral("description")) {
		placemark->setDescription(propertyValue);
		osmData.addTag(propertyKey, propertyValue);

	    } else if (propertyKey == QStringLiteral("marker-size")) {
		// TODO: Implement marker-size handling

	    } else if (propertyKey == QStringLiteral("marker-symbol")) {
		// TODO: Implement marker-symbol handling

	    } else if (propertyKey == QStringLiteral("marker-color")) {
		// Even though the Simplestyle spec allows colors to omit the leading "#", this
		// implementation assumes it is always present, as this then allows named colors
		// understood by QColor as an extension
		QColor color = QColor(propertyValue);
		if (color.isValid()) {
		    iconStyle.setColor(color);
		} else {
		    qDebug() << "Ignoring invalid marker-color property:" << propertyValue;
		}

	    } else if (propertyKey == QStringLiteral("stroke")) {
		QColor color = QColor(propertyValue);	// Assume leading "#" is present
		if (color.isValid()) {
		    color.setAlpha(lineStyle.color().alpha());
		    lineStyle.setColor(color);
		} else {
		    qDebug() << "Ignoring invalid stroke property:" << propertyValue;
		}

	    } else if (propertyKey == QStringLiteral("stroke-opacity")) {
		bool ok;
		float opacity = propertyValue.toFloat(&ok);
		if (ok && opacity >= 0.0 && opacity <= 1.0) {
		    QColor color = lineStyle.color();
		    color.setAlphaF(opacity);
		    lineStyle.setColor(color);
		} else {
		    qDebug() << "Ignoring invalid stroke-opacity property:" << propertyValue;
		}

	    } else if (propertyKey == QStringLiteral("stroke-width")) {
		bool ok;
		float width = propertyValue.toFloat(&ok);
		if (ok && width >= 0.0) {
		    lineStyle.setWidth(width);
		} else {
		    qDebug() << "Ignoring invalid stroke-width property:" << propertyValue;
		}

	    } else if (propertyKey == QStringLiteral("fill")) {
		QColor color = QColor(propertyValue);	// Assume leading "#" is present
		if (color.isValid()) {
		    color.setAlpha(polyStyle.color().alpha());
		    polyStyle.setColor(color);
		} else {
		    qDebug() << "Ignoring invalid fill property:" << propertyValue;
		}

	    } else if (propertyKey == QStringLiteral("fill-opacity")) {
		bool ok;
		float opacity = propertyValue.toFloat(&ok);
		if (ok && opacity >= 0.0 && opacity <= 1.0) {
		    QColor color = polyStyle.color();
		    color.setAlphaF(opacity);
		    polyStyle.setColor(color);
		} else {
		    qDebug() << "Ignoring invalid fill-opacity property:" << propertyValue;
		}

	    } else {
		// Property is not defined by the Simplestyle spec
		osmData.addTag(propertyKey, propertyValue);
	    }
	}

	style->setIconStyle(iconStyle);
	style->setLineStyle(lineStyle);
	style->setPolyStyle(polyStyle);
	placemark->setStyle(style);

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
