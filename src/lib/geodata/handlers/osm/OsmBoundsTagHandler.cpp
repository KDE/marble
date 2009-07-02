//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmBoundsTagHandler.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "OsmElementDictionary.h"

namespace Marble
{
namespace osm
{

static GeoTagHandlerRegistrar s_handlerbounds( GeoTagHandler::QualifiedName(osmTag_bounds, ""),
                                               new OsmBoundsTagHandler() );

GeoNode* OsmBoundsTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    GeoDataLineString bound;

    qreal minlon = parser.attribute("minlon").toDouble();
    qreal minlat = parser.attribute("minlat").toDouble();

    qreal maxlon = parser.attribute("maxlon").toDouble();
    qreal maxlat = parser.attribute("maxlat").toDouble();

    GeoDataCoordinates topLeft( minlon, minlat, 0, GeoDataCoordinates::Degree );
    GeoDataCoordinates topRight( maxlon, minlat, 0 , GeoDataCoordinates::Degree );
    GeoDataCoordinates botLeft( minlon, maxlat, 0 , GeoDataCoordinates::Degree );
    GeoDataCoordinates botRight( maxlon, maxlat, 0 , GeoDataCoordinates::Degree );

    bound.append( topLeft );
    bound.append( topRight );
    bound.append( botRight );
    bound.append( botLeft );

    GeoDataDocument* doc = geoDataDoc( parser );

    GeoDataPlacemark place;

    place.setCoordinate( topLeft.longitude(), topLeft.latitude(), topLeft.altitude() );

    place.setGeometry( GeoDataLinearRing( bound ) );

    doc->append( place );

    return 0;

}

}
}
