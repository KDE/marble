/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

// Self
#include "OsmRelationTagHandler.h"

// Qt
#include <QVariant>

// Osm plugin
#include "OsmObjectManager.h"
#include "OsmParser.h"
#include "OsmElementDictionary.h"

// Marble
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataPolygon.h"
#include "osm/OsmPlacemarkData.h"
#include "GeoDataExtendedData.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmRelationTagHandler( GeoParser::QualifiedName( osmTag_relation, "" ),
        new OsmRelationTagHandler() );

GeoNode* OsmRelationTagHandler::parse( GeoParser &geoParser ) const
{
    // Osm Relation http://wiki.openstreetmap.org/wiki/Data_Primitives#Relation

    Q_ASSERT( dynamic_cast<OsmParser *>( &geoParser ) != 0 );
    OsmParser &parser = static_cast<OsmParser &>( geoParser );

    Q_ASSERT( parser.isStartElement() );

    GeoDataDocument* doc = geoDataDoc( parser );
    Q_ASSERT( doc );

    GeoDataPolygon *polygon = new GeoDataPolygon();
    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    placemark->setGeometry( polygon );
    qint64 id = parser.attribute( "id" ).toLongLong();

    // Saving osm server generated data
    OsmPlacemarkData osmData = parser.osmAttributeData();
    GeoDataExtendedData extendedData;
    extendedData.addValue( GeoDataData( OsmPlacemarkData::osmHashKey(), QVariant::fromValue( osmData ) ) );
    placemark->setExtendedData( extendedData );
    OsmObjectManager::registerId( id );

    // At the beginning visibility = false. Afterwards when parsing
    // the tags for the placemark it will decide if it should be displayed or not
    placemark->setVisible( false );
    doc->append( placemark );

    parser.setPolygon( id, polygon );

    return polygon;
}

}

}
