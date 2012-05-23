//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmMemberTagHandler.h"

#include "GeoParser.h"
#include "OsmWayFactory.h"
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
            // Outer poligons
            if (parser.attribute( "role" ) == "outer")
            {

                GeoDataPolygon *s = parentItem.nodeAs<GeoDataPolygon>();
                Q_ASSERT( s );
                quint64 id = parser.attribute( "ref" ).toULongLong();

                // With the id we get the way geometry
                if ( GeoDataLineString *l =  osm::OsmWayFactory::getLine( id )  )
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

                    GeoDataLinearRing envelope = s->outerBoundary();

                    // Case 0: envelope is empty
                    if ( envelope.isEmpty() )
                    {
                        // Append the way coordinates
                        for (int x = 0; x < l->size(); x++)
                        {
                            envelope.append( GeoDataCoordinates ( l->at(x).longitude(), l->at(x).latitude() ) );
                        }
                    }

                    // Case 1: l.first = envelope.first
                    else if ( l->first().operator ==(envelope.first()) )
                    {
                        GeoDataLinearRing *temp = new GeoDataLinearRing(envelope.tessellationFlags());

                        // Invert envelopes direction
                        for (int x = envelope.size()-1; x > -1; x--)
                        {
                            temp->append(GeoDataCoordinates ( envelope.at(x).longitude(), envelope.at(x).latitude() ));
                        }
                        envelope.operator =( *temp );

                        // Now its the same as case 2
                        // x=1 not to repeat the shared node
                        for (int x = 1; x < l->size(); x++)
                        {
                            envelope.append( GeoDataCoordinates ( l->at(x).longitude(), l->at(x).latitude() ) );
                        }

                    }

                    // Case 2: l.first = envelope.last
                    else if (l->first().operator ==(envelope.last()))
                    {
                        // x=1 not to repeat the shared node
                        for (int x = 1; x < l->size(); x++)
                        {
                            envelope.append( GeoDataCoordinates ( l->at(x).longitude(), l->at(x).latitude() ) );
                        }

                    }

                    // Case 3: l.last = envelope.first
                    else if (l->last().operator ==(envelope.first()) )
                    {
                        GeoDataLinearRing *temp = new GeoDataLinearRing(envelope.tessellationFlags());

                        // Invert envelopes direction
                        for (int x = envelope.size()-1; x > -1; x--)
                        {
                            temp->append(GeoDataCoordinates ( envelope.at(x).longitude(), envelope.at(x).latitude() ));
                        }
                        envelope.operator =( *temp );

                        // Now its the same as case 4
                        // size-2 not to repeat the shared node
                        for (int x = l->size()-2; x > -1; x--)
                        {
                            envelope.append( GeoDataCoordinates ( l->at(x).longitude(), l->at(x).latitude() ) );
                        }


                    }

                    // Case 4: l.last = envelope.last
                    else if (l->last().operator ==(envelope.last()) )
                    {
                        // size-2 not to repeat the shared node
                        for (int x = l->size()-2; x > -1; x--)
                        {
                            envelope.append( GeoDataCoordinates ( l->at(x).longitude(), l->at(x).latitude() ) );
                        }
                    }

                    // Update the outer boundary
                    s->setOuterBoundary( envelope );

                }
            }

            // Inner poligons
            if (parser.attribute( "role" ) == "inner")
            {

                GeoDataPolygon *s = parentItem.nodeAs<GeoDataPolygon>();
                Q_ASSERT( s );
                quint64 id = parser.attribute( "ref" ).toULongLong();

                // With the id we get the way geometry
                if ( GeoDataLineString *l = osm::OsmWayFactory::getLine( id ) )
                {
                    GeoDataLinearRing *temp = new GeoDataLinearRing(l->tessellationFlags());

                    for (int x = 0; x < l->size(); x++)
                    {
                        temp->append( GeoDataCoordinates ( l->at(x).longitude(), l->at(x).latitude() ) );
                    }
                    s->appendInnerBoundary( *temp );
                }
            }
        }

        return 0;
    }

    return 0;
}

}

}
