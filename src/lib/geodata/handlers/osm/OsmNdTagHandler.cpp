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

static GeoTagHandlerRegistrar osmNdTagHandler( GeoTagHandler::QualifiedName( osmTag_nd, "" ),
        new OsmNdTagHandler() );

GeoNode* OsmNdTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( osmTag_way ) )
    {
        GeoDataLineString *s = parentItem.nodeAs<GeoDataLineString>();
        Q_ASSERT( s );
        quint64 id = parser.attribute( "ref" ).toULongLong();
        if ( GeoDataPoint *p = osm::OsmNodeFactory::getPoint( id ) )
        {
            s->append( GeoDataCoordinates( p->longitude(), p->latitude() ) );
        }

        return 0;
    }

    return 0;
}

}

}
