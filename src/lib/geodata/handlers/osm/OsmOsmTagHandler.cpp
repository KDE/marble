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
static GeoTagHandlerRegistrar osmOsmTagHandler( GeoTagHandler::QualifiedName( osmTag_osm, "" ),
        new OsmOsmTagHandler() );

static QColor osmBuildingColor( 0xBE, 0xAD, 0xAD );
static QColor osmBackgroundColor( 0xF1, 0xEE, 0xE8 );
static QColor osmWaterColor( 0xB5, 0xD0, 0xD0 );

GeoNode* OsmOsmTagHandler::parse( GeoParser& parser ) const
{
    GeoDataDocument* doc = geoDataDoc( parser );

    GeoDataPolyStyle buildingPolyStyle;
    GeoDataLineStyle buildingLineStyle;
    buildingPolyStyle.setFill( true );
    buildingPolyStyle.setOutline( true );
    buildingPolyStyle.setColor( osmBuildingColor );
    buildingLineStyle.setColor( osmBuildingColor.darker() );
    GeoDataStyle buildingStyle;
    buildingStyle.setPolyStyle( buildingPolyStyle );
    buildingStyle.setLineStyle( buildingLineStyle );
    buildingStyle.setStyleId( "building" );
    doc->addStyle( buildingStyle );

    GeoDataPolyStyle waterPolyStyle;
    GeoDataLineStyle waterLineStyle;
    waterPolyStyle.setFill( true );
    waterPolyStyle.setOutline( true );
    waterPolyStyle.setColor( osmWaterColor );
    waterLineStyle.setColor( osmWaterColor );
    waterLineStyle.setWidth( 2 );
    GeoDataStyle waterStyle;
    waterStyle.setPolyStyle( waterPolyStyle );
    waterStyle.setLineStyle( waterLineStyle );
    waterStyle.setStyleId( "water" );
    doc->addStyle( waterStyle );

    GeoDataPolyStyle backgroundPolyStyle;
    backgroundPolyStyle.setFill( true );
    backgroundPolyStyle.setOutline( false );
    backgroundPolyStyle.setColor( osmBackgroundColor );
    GeoDataStyle backgroundStyle;
    backgroundStyle.setPolyStyle( backgroundPolyStyle );
    backgroundStyle.setStyleId( "background" );
    doc->addStyle( backgroundStyle );

    return doc;
}

}

}
