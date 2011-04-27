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

    if ( parentItem.represents( osmTag_way ) )
    {
        GeoDataGeometry *g = parentItem.nodeAs<GeoDataGeometry>();
        Q_ASSERT( g );
        GeoDataPlacemark *p = dynamic_cast<GeoDataPlacemark*>( g->parent() );
        Q_ASSERT( p );
        //Convert area ways to polygons
        if (( key == "building" || key == "area" ) && ( value == "yes" ) )
        {
            GeoDataLineString *l = dynamic_cast<GeoDataLineString *>( g );
            Q_ASSERT( l );
            doc->remove( doc->childPosition( p ) );
            GeoDataPlacemark *newp = new GeoDataPlacemark( *p );
            GeoDataPolygon *pol = new GeoDataPolygon;
            pol->setOuterBoundary( *l );
            newp->setGeometry( pol );
            doc->append( newp );
        }

        return 0;
    }

    return 0;
}

}

}
