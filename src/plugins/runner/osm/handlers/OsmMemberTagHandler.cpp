/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "OsmMemberTagHandler.h"

#include "GeoParser.h"
#include "OsmWayFactory.h"
#include "OsmRelationFactory.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataPolygon.h"
#include "OsmElementDictionary.h"
#include "MarbleDebug.h"

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmMemberTagHandler( GeoParser::QualifiedName( osmTag_member, "" ),
        new OsmMemberTagHandler() );

GeoNode* OsmMemberTagHandler::parse( GeoParser& parser ) const
{
    // Relations members examples
    // http://wiki.openstreetmap.org/wiki/Relation:multipolygon#Examples

    Q_ASSERT( parser.isStartElement() );

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( osmTag_relation ) )
    {
        // Never heard of a type different from "way" but
        // maybe it should be checked

        if (parser.attribute( "type" ) == "way")
        {
            // Outer poligons (sometimes the role is empty)
            if (parser.attribute( "role" ) == "outer" || parser.attribute( "role" ) == "")
            {

                GeoDataPolygon *polygon = parentItem.nodeAs<GeoDataPolygon>();
                Q_ASSERT( polygon );
                quint64 id = parser.attribute( "ref" ).toULongLong();

                // With the id we get the way geometry
                if ( GeoDataLineString *line =  osm::OsmWayFactory::line( id )  )
                {
                    // Some of the ways that build the relation
                    // might be in opposite directions
                    // so the final linearRing would be wrong.
                    // It is needed to seek in the linearRing
                    // to know if the new way should be added
                    // at the beginning or end and in which order.
                    // Also the shared node (which will be in both
                    // geometries) has to be removed to avoid having
                    // it repeated.

                    GeoDataLinearRing envelope = polygon->outerBoundary();

                    // Case 0: envelope is empty
                    if ( envelope.isEmpty() )
                    {
                        envelope = *line;
                    }

                    // Case 1: line.first = envelope.first
                    else if ( line->first() == envelope.first() )
                    {
                        GeoDataLinearRing temp = GeoDataLinearRing( envelope.tessellationFlags() );

                        // Invert envelopes direction
                        for (int x = envelope.size()-1; x > -1; x--)
                        {
                            temp.append( GeoDataCoordinates ( envelope.at(x) ) );
                        }
                        envelope = temp;

                        // Now its the same as case 2
                        // envelope-last not to repeat the shared node
                        envelope.remove( envelope.size() - 1 );
                        envelope << *line;
                    }

                    // Case 2: line.first = envelope.last
                    else if (line->first() == envelope.last() )
                    {
                        // envelope-last not to repeat the shared node
                        envelope.remove( envelope.size() - 1 );
                        envelope << *line;
                    }

                    // Case 3: line.last = envelope.first
                    else if (line->last() == envelope.first() )
                    {
                        GeoDataLinearRing temp = GeoDataLinearRing( envelope.tessellationFlags() );

                        // Invert envelopes direction
                        for (int x = envelope.size()-1; x > -1; x--)
                        {
                            temp.append( GeoDataCoordinates ( envelope.at(x) ) );
                        }
                        envelope = temp;

                        // Now its the same as case 4
                        // size-2 not to repeat the shared node
                        for (int x = line->size()-2; x > -1; x--)
                        {
                            envelope.append( GeoDataCoordinates ( line->at(x) ) );
                        }
                    }

                    // Case 4: line.last = envelope.last
                    else if (line->last() == envelope.last() )
                    {
                        // size-2 not to repeat the shared node
                        for (int x = line->size()-2; x > -1; x--)
                        {
                            envelope.append( GeoDataCoordinates ( line->at(x) ) );
                        }
                    }

                    // Update the outer boundary
                    polygon->setOuterBoundary( envelope );
                }
            }

            // Inner poligons
            if (parser.attribute( "role" ) == "inner")
            {
                GeoDataPolygon *polygon = parentItem.nodeAs<GeoDataPolygon>();
                Q_ASSERT( polygon );
                quint64 id = parser.attribute( "ref" ).toULongLong();

                // With the id we get the way geometry
                if ( GeoDataLineString *line = osm::OsmWayFactory::line( id ) )
                {
                    polygon->appendInnerBoundary( GeoDataLinearRing( *line ) );
                }
            }
        }

        else if (parser.attribute( "type" ) == "relation")
        {
            // Never seen this case
            if ( parser.attribute( "role" ) == "outer" )
            {
                mDebug() << "Parsed relation with a relation outer member";
            }

            // It only can be an inner relation or subarea
            // Subarea is mainly used for administrative boundaries
            else if (parser.attribute( "role" ) == "inner"
                     || parser.attribute( "role" ) == "subarea"
                     || parser.attribute( "role" ) == "")
            {
                GeoDataPolygon *polygon = parentItem.nodeAs<GeoDataPolygon>();
                Q_ASSERT( polygon );
                quint64 id = parser.attribute( "ref" ).toULongLong();

                // With the id we get the relation geometry
                if ( GeoDataPolygon *p =  osm::OsmRelationFactory::polygon( id ) )
                {
                    polygon->appendInnerBoundary( p->outerBoundary() );
                }
            }
        }

        return 0;
    }

    return 0;
}

}

}
