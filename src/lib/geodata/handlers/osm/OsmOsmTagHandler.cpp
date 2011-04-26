//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmOsmTagHandler.h"

#include "GeoParser.h"
#include "GeoDataPoint.h"
#include "GeoDataDocument.h"
#include "global.h"
#include "GeoDataParser.h"
#include "GeoDataLineStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataStyle.h"
#include "OsmElementDictionary.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmOsmTagHandler( GeoTagHandler::QualifiedName(osmTag_osm, ""),
                                               new OsmOsmTagHandler() );

GeoNode* OsmOsmTagHandler::parse ( GeoParser& parser) const
{
    GeoDataDocument* doc = geoDataDoc( parser );    
    return doc;
}

}

}
