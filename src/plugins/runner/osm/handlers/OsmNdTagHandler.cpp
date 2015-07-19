//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// Copyright 2015      Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#include <QHash>
#include <QVariant>
#include <QThread>

#include "OsmNdTagHandler.h"
#include "OsmElementDictionary.h"
#include "OsmParser.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"

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
        qint64 id = parser.attribute( "ref" ).toLongLong();

        if ( GeoDataPoint *p = parser.node( id ) ) {

            const GeoDataCoordinates coordinates = p->coordinates();
            s->append( coordinates );

            GeoDataPlacemark *nodePlacemark = dynamic_cast<GeoDataPlacemark *> ( p->parent() );
            GeoDataPlacemark *wayPlacemark = dynamic_cast<GeoDataPlacemark *> ( s->parent() );
            Q_ASSERT( nodePlacemark );
            Q_ASSERT( wayPlacemark );

            // Getting the node's osmData
            OsmPlacemarkData &nodeOsmData = nodePlacemark->osmData();

            // Getting the line's osmData
            OsmPlacemarkData &wayOsmData = wayPlacemark->osmData();

            // Inserting the point's osmData in the line osmData's reference hash map
            wayOsmData.addReference( coordinates, nodeOsmData );
        }

        return 0;
    }

    return 0;
}

}

}
