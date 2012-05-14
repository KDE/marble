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
#include "MarbleGlobal.h"
#include "GeoDataParser.h"
#include "GeoDataLineStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataStyle.h"
#include "OsmElementDictionary.h"
#include "OsmGlobals.h"

namespace Marble
{

namespace osm
{
static GeoTagHandlerRegistrar osmOsmTagHandler( GeoParser::QualifiedName( osmTag_osm, "" ),
        new OsmOsmTagHandler() );

GeoNode* OsmOsmTagHandler::parse( GeoParser& parser ) const
{
    GeoDataDocument* doc = geoDataDoc( parser );

    GeoDataPolyStyle backgroundPolyStyle;
    backgroundPolyStyle.setFill( true );
    backgroundPolyStyle.setOutline( false );
    backgroundPolyStyle.setColor( OsmGlobals::backgroundColor );
    GeoDataStyle backgroundStyle;
    backgroundStyle.setPolyStyle( backgroundPolyStyle );
    backgroundStyle.setStyleId( "background" );
    doc->addStyle( backgroundStyle );

    return doc;
}

}

}
