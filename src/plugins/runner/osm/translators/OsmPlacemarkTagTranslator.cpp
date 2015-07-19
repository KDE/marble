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
#include "OsmPlacemarkTagTranslator.h"

//Marble
#include "OsmObjectManager.h"
#include "OsmNodeTagWriter.h"
#include "OsmWayTagWriter.h"
#include "OsmRelationTagWriter.h"
#include "OsmElementDictionary.h"
#include "GeoDataPoint.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataPolygon.h"
#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"
#include "GeoWriter.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerPlacemark( GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataPlacemarkType,
                                                osm::osmTag_version06 ), new OsmPlacemarkTagTranslator() );

OsmPlacemarkTagTranslator::OsmPlacemarkTagTranslator():
    OsmFeatureTagTranslator() {
    //nothing to do
}

bool OsmPlacemarkTagTranslator::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark*>( node );
    const OsmPlacemarkData &osmData = placemark->osmData();

    if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType ) {
        const GeoDataPoint* point = static_cast<const GeoDataPoint*>( placemark->geometry() );
        const GeoDataCoordinates& coordinates = point->coordinates();
        OsmNodeTagWriter::writeNode( coordinates, osmData, writer );
    }
    else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
        const GeoDataLineString* lineString = static_cast<const GeoDataLineString*>( placemark->geometry() );
        OsmWayTagWriter::writeWay( *lineString, osmData, writer );
    }
    else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        const GeoDataPolygon* polygon = static_cast<const GeoDataPolygon*>( placemark->geometry() );
        OsmRelationTagWriter::writeMultipolygon( *polygon, osmData, writer );
    }

    return true;
}

}


