//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

// Self
#include "OsmWayTagHandler.h"

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
#include "GeoDataLineString.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "osm/OsmPlacemarkData.h"

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

    // Adding the way to the parser's hash ( in case we need to ref it )
    parser.setWay( id, polyline );

    return polyline;
}

}

}
