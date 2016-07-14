//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

//Self
#include "OsmDocumentTagTranslator.h"

//Marble
#include "OsmNodeTagWriter.h"
#include "OsmWayTagWriter.h"
#include "OsmElementDictionary.h"
#include "GeoDataDocument.h"
#include "GeoWriter.h"
#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"
#include "OsmRelationTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument( GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataDocumentType,
                                                                            osm::osmTag_version06 ),
                                               new OsmDocumentTagTranslator() );


bool OsmDocumentTagTranslator::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataDocument *document = static_cast<const GeoDataDocument*>(node);

    // Creating separate lists, to improve efficiency
    typedef QPair<const GeoDataPolygon*, OsmPlacemarkData > OsmPolygon;
    typedef QPair<const GeoDataLineString*, OsmPlacemarkData > OsmLineString;
    QList<OsmPlacemarkData> nodes;
    QList<OsmLineString> ways;
    QList<OsmPolygon> polygons;

    // Writing all the component nodes ( points, nodes of polylines, nodes of polygons )
    foreach (GeoDataPlacemark* placemark, document->placemarkList()) {
        // If the placemark's osmData is not complete, it is initialized by the OsmObjectManager
        OsmObjectManager::initializeOsmData( placemark );
        const OsmPlacemarkData osmData = placemark->osmData();

        if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType ) {
            nodes << osmData;
        } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
            const GeoDataLineString* lineString = static_cast<const GeoDataLineString*>( placemark->geometry() );
            nodes << osmData;
            ways << OsmLineString(lineString, osmData);
        } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType ) {
            const GeoDataLinearRing* linearRing = static_cast<const GeoDataLinearRing*>( placemark->geometry() );
            nodes << osmData;
            ways << OsmLineString(linearRing, osmData);
        } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( placemark->geometry() );
            int index = -1;

            // Writing all the outerRing's nodes
            const GeoDataLinearRing &outerRing = polygon->outerBoundary();
            const OsmPlacemarkData outerRingOsmData = osmData.memberReference( index );
            nodes << outerRingOsmData;
            ways << OsmLineString(&outerRing, outerRingOsmData);

            // Writing all nodes for each innerRing
            foreach ( const GeoDataLinearRing &innerRing, polygon->innerBoundaries() ) {
                ++index;
                const OsmPlacemarkData innerRingOsmData = osmData.memberReference( index );
                nodes << innerRingOsmData;
                ways << OsmLineString(&innerRing, innerRingOsmData);
            }
            polygons.append(OsmPolygon(polygon, osmData));
        }
    }

    OsmNodeTagWriter::writeAllNodes(nodes, writer);

    qSort(ways.begin(), ways.end(), [] (const OsmLineString &a, const OsmLineString &b) { return a.second.id() < b.second.id(); });
    qint64 lastId = 0;
    foreach(const OsmLineString &way, ways) {
        if (way.second.id() != lastId) {
            OsmWayTagWriter::writeWay(*way.first, way.second, writer);
            lastId = way.second.id();
        }
    }

    // Writing polygons
    qSort(polygons.begin(), polygons.end(), [] (const OsmPolygon &a, const OsmPolygon &b) { return a.second.id() < b.second.id(); });
    foreach (const OsmPolygon& polygon, polygons) {
        OsmRelationTagWriter::writeMultipolygon(*polygon.first, polygon.second, writer );
    }

    return true;
}

}

