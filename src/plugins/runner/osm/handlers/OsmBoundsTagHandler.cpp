//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmBoundsTagHandler.h"

#include "GeoParser.h"
#include "OsmNodeFactory.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataLineString.h"
#include "MarbleDebug.h"
#include "OsmElementDictionary.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmBoundsTagHandler( GeoParser::QualifiedName( osmTag_bounds, "" ),
        new OsmBoundsTagHandler() );

GeoNode* OsmBoundsTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    GeoDataDocument* doc = geoDataDoc( parser );
    GeoDataLinearRing r;
    GeoDataPolygon *p = new GeoDataPolygon();
    qreal minlat = parser.attribute("minlat").toFloat();
    qreal minlon = parser.attribute("minlon").toFloat();
    qreal maxlat = parser.attribute("maxlat").toFloat();
    qreal maxlon = parser.attribute("maxlon").toFloat();
    r.append( GeoDataCoordinates( minlon, minlat, 0, GeoDataCoordinates::Degree ) );
    r.append( GeoDataCoordinates( maxlon, minlat, 0, GeoDataCoordinates::Degree ) );
    r.append( GeoDataCoordinates( maxlon, maxlat, 0, GeoDataCoordinates::Degree ) );
    r.append( GeoDataCoordinates( minlon, maxlat, 0, GeoDataCoordinates::Degree ) );
    p->setOuterBoundary( r );

    GeoDataPlacemark *pl = new GeoDataPlacemark();
    pl->setGeometry( p );
    pl->setVisualCategory( GeoDataFeature::None );
    pl->setStyle( &doc->style( "background" ) );
    pl->setVisible( true );
    doc->append( pl );

    mDebug() << "[OSM] Bounds: " << minlat << " " << minlon << " " << maxlat << " " << maxlon;

    return 0;
}

}

}
