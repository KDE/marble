//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmTagTagHandler.h"

#include "GeoParser.h"
#include "OsmNodeFactory.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataLineString.h"
#include "MarbleDebug.h"
#include "OsmElementDictionary.h"
#include "OsmGlobals.h"
#include "GeoDataStyle.h"

namespace Marble
{

namespace osm
{
static GeoTagHandlerRegistrar osmTagTagHandler( GeoTagHandler::QualifiedName( osmTag_tag, "" ),
        new OsmTagTagHandler() );

GeoNode* OsmTagTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    GeoStackItem parentItem = parser.parentElement();
    GeoDataDocument* doc = geoDataDoc( parser );
    QString key = parser.attribute( "k" );
    QString value = parser.attribute( "v" );

    GeoDataGeometry * geometry = parentItem.nodeAs<GeoDataGeometry>();
    if ( !geometry )
        return 0;
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( geometry->parent() );

    if ( parentItem.represents( osmTag_way ) )
    {
        if ( !placemark )
            return 0;

        //Convert area ways to polygons
        if ( key == "building" && value == "yes" )
        {
            placemark = convertWayToPolygon( doc, placemark, geometry );
            placemark->setStyle( &doc->style( "building" ) );
            placemark->setVisible( true );
        }
        else if ( key == "highway" )
        {
            placemark->setVisible( true );
        }
        else if ( key == "area" && value == "yes" )
        {
            placemark = convertWayToPolygon( doc, placemark, geometry );
        }
        else if ( key == "waterway" )
        {
            if ( value == "riverbank" )
                placemark = convertWayToPolygon( doc, placemark, geometry );
            placemark->setStyle( &doc->style( "water" ) );
            placemark->setVisible( true );
        }
    }
    else if ( parentItem.represents( osmTag_node ) ) //POI
    {
        GeoDataStyle *poiStyle = OsmGlobals::poiStyles().value( key + "=" + value );

        if ( !poiStyle )
            return 0;

        GeoDataPoint *point = dynamic_cast<GeoDataPoint *>( geometry );
        Q_ASSERT( point );

        if ( !placemark )
        {
            placemark = new GeoDataPlacemark();
            doc->append( placemark );
        }
        placemark->setGeometry( point );
        placemark->setStyle( poiStyle );
    }

    return 0;
}

GeoDataPlacemark *OsmTagTagHandler::convertWayToPolygon( GeoDataDocument *doc, GeoDataPlacemark *placemark, GeoDataGeometry *geometry ) const
{
    GeoDataLineString *polyline = dynamic_cast<GeoDataLineString *>( geometry );
    Q_ASSERT( polyline );
    doc->remove( doc->childPosition( placemark ) );
    GeoDataPlacemark *newPlacemark = new GeoDataPlacemark( *placemark );
    GeoDataPolygon *polygon = new GeoDataPolygon;
    polygon->setOuterBoundary( *polyline );
    newPlacemark->setGeometry( polygon );
    doc->append( newPlacemark );
    return newPlacemark;
}


}

}

