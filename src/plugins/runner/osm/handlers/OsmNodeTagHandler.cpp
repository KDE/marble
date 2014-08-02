//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmNodeTagHandler.h"

#include "OsmElementDictionary.h"
#include "OsmParser.h"

#include "GeoDataCoordinates.h"
#include "GeoDataPoint.h"

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

    GeoDataPoint *point = new GeoDataPoint( lon, lat, 0, GeoDataCoordinates::Degree );
    parser.setNode( parser.attribute( "id" ).toULongLong(), point );
    return point;
}

}

}
