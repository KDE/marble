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
#include "OsmObjectManager.h"
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

//Qt
#include <QVector>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument( GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataDocumentType,
                                                                            osm::osmTag_version06 ),
                                               new OsmDocumentTagTranslator() );


bool OsmDocumentTagTranslator::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataDocument *document = static_cast<const GeoDataDocument*>(node);

    // Creating separate lists, to improve efficiency
    QList<const GeoDataObject*> polylines, polygons;
    QList<OsmBound> bounds;

    // Writing all the component nodes ( points, nodes of polylines, nodes of polygons )
    foreach ( GeoDataFeature* feature, document->featureList() ) {

        if ( feature->nodeType() != GeoDataTypes::GeoDataPlacemarkType ) {
            continue;
        }
        GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( feature );

        // If the placemark does not have osmData, it is initialized by the OsmObjectManager
        if ( !placemark->hasOsmData() ) {
            OsmObjectManager::initializeOsmData( placemark );
        }

        const OsmPlacemarkData osmData = placemark->osmData();

        if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType ) {
            writeElement( placemark, writer );
        }
        else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
            // Writing all the lineString nodes directly from the hash, as order is irrelevant
            OsmNodeTagWriter::writeAllNodes( osmData, writer );
            polylines.append( placemark );
        }
        else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( placemark->geometry() );

            // Writing all the outerRing's nodes
            const GeoDataLinearRing &outerRing = polygon->outerBoundary();
            const OsmPlacemarkData outerRingOsmData = osmData.reference( &outerRing );
            OsmNodeTagWriter::writeAllNodes( outerRingOsmData, writer );
            bounds.append( OsmBound( &outerRing, outerRingOsmData ) );

            // Writing all nodes for each innerRing
            foreach ( const GeoDataLinearRing &innerRing, polygon->innerBoundaries() ) {
                const OsmPlacemarkData innerRingOsmData = osmData.reference( &innerRing );
                OsmNodeTagWriter::writeAllNodes( innerRingOsmData, writer );
                bounds.append( OsmBound( &innerRing, innerRingOsmData ) );
            }
            polygons.append( placemark );
        }
    }


    // Writing the ways
    foreach ( const GeoDataObject* polyline, polylines ) {
        writeElement( polyline, writer );
    }

    // Writing the bounds ( innerBounds and outerBounds, part of polygons )
    foreach ( OsmBound bound, bounds ) {
        OsmWayTagWriter::writeWay( *bound.first, bound.second, writer );
    }

    // Writing polygons
    foreach ( const GeoDataObject* polygon, polygons ) {
        writeElement( polygon, writer );
    }

    return true;
}


OsmDocumentTagTranslator::OsmDocumentTagTranslator():
    OsmFeatureTagTranslator() {
    //nothing to do
}

}

