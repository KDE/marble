//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmNdTagHandler.h"

#include "OsmElementDictionary.h"
#include "OsmParser.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataPoint.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmNdTagHandler( GeoParser::QualifiedName( osmTag_nd, "" ),
        new OsmNdTagHandler() );

GeoNode* OsmNdTagHandler::parse( GeoParser &geoParser ) const
{
    Q_ASSERT( dynamic_cast<OsmParser *>( &geoParser ) != 0 );
    OsmParser &parser = static_cast<OsmParser &>( geoParser );

    Q_ASSERT( parser.isStartElement() );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( osmTag_way ) )
    {
        GeoDataLineString *s = parentItem.nodeAs<GeoDataLineString>();
        Q_ASSERT( s );
        quint64 id = parser.attribute( "ref" ).toULongLong();
        if ( GeoDataPoint *p = parser.node( id ) ) {
            s->append( GeoDataCoordinates( p->coordinates().longitude(), p->coordinates().latitude() ) );
        }

        return 0;
    }

    return 0;
}

}

}
