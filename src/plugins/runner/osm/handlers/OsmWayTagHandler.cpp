//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmWayTagHandler.h"

#include "OsmParser.h"
#include "OsmElementDictionary.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataLineString.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmWayTagHandler( GeoParser::QualifiedName( osmTag_way, "" ),
        new OsmWayTagHandler() );

GeoNode* OsmWayTagHandler::parse( GeoParser &geoParser ) const
{
    // Osm Way http://wiki.openstreetmap.org/wiki/Data_Primitives#Way

    Q_ASSERT( dynamic_cast<OsmParser *>( &geoParser ) );
    OsmParser &parser = static_cast<OsmParser &>( geoParser );

    Q_ASSERT( parser.isStartElement() );

    GeoDataDocument* doc = geoDataDoc( parser );
    Q_ASSERT( doc );

    GeoDataLineString *polyline = new GeoDataLineString();
    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    placemark->setGeometry( polyline );

    // At the beginning visibility = false. Afterwards when parsing
    // the tags for the placemark it will decide if it should be displayed or not
    placemark->setVisible( false );
    doc->append( placemark );

    parser.setWay( parser.attribute( "id" ).toULongLong(), polyline );

    return polyline;
}

}

}
