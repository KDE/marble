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

#include "GeoParser.h"
#include "OsmWayFactory.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataLineString.h"
#include "OsmElementDictionary.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmWayTagHandler( GeoParser::QualifiedName( osmTag_way, "" ),
        new OsmWayTagHandler() );

GeoNode* OsmWayTagHandler::parse( GeoParser& parser ) const
{
    // Osm Way http://wiki.openstreetmap.org/wiki/Data_Primitives#Way

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

    osm::OsmWayFactory::appendLine( parser.attribute( "id" ).toULongLong(), polyline );

    return polyline;
}

}

}
