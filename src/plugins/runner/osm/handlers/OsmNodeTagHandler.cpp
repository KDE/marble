//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// Copyright 2015      Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

// Self
#include "OsmNodeTagHandler.h"

// Osm plugin
#include "OsmObjectManager.h"
#include "OsmElementDictionary.h"
#include "OsmParser.h"

// Marble
#include "GeoDataCoordinates.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataGeometry.h"
#include "GeoDataData.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmNodeTagHandler( GeoParser::QualifiedName( osmTag_node, "" ),
        new OsmNodeTagHandler() );

GeoNode* OsmNodeTagHandler::parse( GeoParser &geoParser ) const
{
    // Osm Node http://wiki.openstreetmap.org/wiki/Data_Primitives#Node

    Q_ASSERT( dynamic_cast<OsmParser *>( &geoParser ) );
    OsmParser &parser = static_cast<OsmParser &>( geoParser );
    Q_ASSERT( parser.isStartElement() );

    qreal lon = parser.attribute( "lon" ).toDouble();
    qreal lat = parser.attribute( "lat" ).toDouble();
    qint64 id = parser.attribute( "id" ).toLongLong();

    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    GeoDataPoint *point = new GeoDataPoint( lon, lat, 0, GeoDataCoordinates::Degree );
    point->setParent( placemark );
    placemark->setZoomLevel( 18 ); // Not really sure it's the right thing?

    // Saving osm server generated data
    OsmPlacemarkData osmData = parser.osmAttributeData();
    GeoDataExtendedData extendedData;
    extendedData.addValue( GeoDataData( OsmPlacemarkData::osmHashKey(), QVariant::fromValue( osmData ) ) );
    placemark->setExtendedData( extendedData );
    parser.addDummyPlacemark( placemark );
    OsmObjectManager::registerId( id );

    // Initially, it is not visible. If a tag is found, this will change.
    placemark->setVisible( false );

    // Adding the node to the parser's hash ( in case we need to ref it )
    parser.setNode( id, point );
    return point;
}

}

}
