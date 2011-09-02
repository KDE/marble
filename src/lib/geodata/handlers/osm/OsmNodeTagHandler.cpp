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

#include "GeoParser.h"
#include "GeoDataPoint.h"
#include "MarbleDebug.h"
#include "OsmNodeFactory.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "OsmElementDictionary.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmNodeTagHandler( GeoTagHandler::QualifiedName( osmTag_node, "" ),
        new OsmNodeTagHandler() );

GeoNode* OsmNodeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    qreal lon = parser.attribute( "lon" ).toDouble();
    qreal lat = parser.attribute( "lat" ).toDouble();

    GeoDataPoint *point = new GeoDataPoint( GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree ) );
    osm::OsmNodeFactory::appendPoint( parser.attribute( "id" ).toULongLong(), point );
    return point;
}

}

}
