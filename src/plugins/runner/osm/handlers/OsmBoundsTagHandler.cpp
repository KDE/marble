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
#include "GeoDataParser.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLinearRing.h"
#include "GeoDataRegion.h"
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

    qreal minlat = parser.attribute("minlat").toFloat();
    qreal minlon = parser.attribute("minlon").toFloat();
    qreal maxlat = parser.attribute("maxlat").toFloat();
    qreal maxlon = parser.attribute("maxlon").toFloat();

    mDebug() << "[OSM] Bounds: " << minlat << " " << minlon << " " << maxlat << " " << maxlon;

    return 0;
}

}

}
