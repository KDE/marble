/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "OsmRelationTagHandler.h"

#include "GeoParser.h"
#include "OsmRelationFactory.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataPolygon.h"
#include "OsmElementDictionary.h"
#include "MarbleDebug.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmRelationTagHandler( GeoParser::QualifiedName( osmTag_relation, "" ),
        new OsmRelationTagHandler() );

GeoNode* OsmRelationTagHandler::parse( GeoParser& parser ) const
{
    // Osm Relation http://wiki.openstreetmap.org/wiki/Data_Primitives#Relation

    Q_ASSERT( parser.isStartElement() );

    GeoDataDocument* doc = geoDataDoc( parser );
    Q_ASSERT( doc );

    GeoDataPolygon *polygon = new GeoDataPolygon();
    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    placemark->setGeometry( polygon );

    // In the beginning visibility = false. Afterwards when it parses
    // the tags for the placemark it will decide if it should be displayed or not
    placemark->setVisible( false );
    doc->append( placemark );

    osm::OsmRelationFactory::appendPolygon( parser.attribute( "id" ).toULongLong(), polygon );

    return polygon;
}

}

}
