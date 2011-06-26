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

static QStringList tagBlackList = QStringList() << "created_by";

GeoNode* OsmTagTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    GeoStackItem parentItem = parser.parentElement();
    GeoDataDocument* doc = geoDataDoc( parser );
    QString key = parser.attribute( "k" );
    QString value = parser.attribute( "v" );

    if ( tagBlackList.contains( key ) )
        return 0;

    GeoDataGeometry * geometry = parentItem.nodeAs<GeoDataGeometry>();
    if ( !geometry )
        return 0;
    
    GeoDataGeometry *placemarkGeometry = geometry;
    
    //If node geometry is part of multigeometry -> go up to placemark geometry.
    while( dynamic_cast<GeoDataMultiGeometry*>(placemarkGeometry->parent()) )
        placemarkGeometry = dynamic_cast<GeoDataMultiGeometry*>(placemarkGeometry->parent());
    
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(placemarkGeometry->parent());
    if( !placemark )
        return 0;

    if ( key == "name" )
    {
        if ( !placemark )
        {
            if ( parentItem.represents( osmTag_node ) )
                placemark = createPOI( doc, geometry );
            else
                return 0;
        }
        placemark->setName( value );
        return 0;
    }

    if ( parentItem.represents( osmTag_way ) )
    {
        Q_ASSERT( placemark );

        //Convert area ways to polygons
        if( !dynamic_cast<GeoDataPolygon*>( geometry ) && OsmGlobals::tagNeedArea( key + "=" + value ) )
        {
            placemark = convertWayToPolygon( doc, placemark, geometry );
        }
        if ( key == "building" && value == "yes" && placemark->visualCategory() == GeoDataFeature::Default )
        {
            placemark->setStyle( &doc->style( "building" ) );
            placemark->setVisible( true );
        }
    }
    else if ( parentItem.represents( osmTag_node ) ) //POI
    {
        GeoDataFeature::GeoDataVisualCategory poiCategory = OsmGlobals::visualCategories().value( key + "=" + value );

        //Placemark is an accepted POI
        if ( poiCategory )
        {
            if ( !placemark )
                placemark = createPOI( doc, geometry );

            placemark->setVisible( true );
        }
    }

    if ( placemark )
    {
        GeoDataFeature::GeoDataVisualCategory category;

        if ( category = OsmGlobals::visualCategories().value( key + "=" + value ) )
        {
            //Remove assigned style (i.e. building style)
            placemark->setStyle( 0 );
            placemark->setVisualCategory( category );
            placemark->setVisible( true );
        }
        else if ( category = OsmGlobals::visualCategories().value( key ) )
        {
            //Remove assigned style (i.e. building style)
            placemark->setStyle( 0 );
            placemark->setVisualCategory( category );
            placemark->setVisible( true );
        }
    }

    return 0;
}

GeoDataPlacemark* OsmTagTagHandler::createPOI( GeoDataDocument* doc, GeoDataGeometry* geometry ) const
{
    GeoDataPoint *point = dynamic_cast<GeoDataPoint *>( geometry );
    Q_ASSERT( point );
    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    placemark->setGeometry( point );
    placemark->setVisible( false );
    placemark->setPopularityIndex( 1 );
    doc->append( placemark );
    return placemark;
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

