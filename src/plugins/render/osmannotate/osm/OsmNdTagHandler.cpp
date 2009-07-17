//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoParser.h"
#include "OsmElementDictionary.h"
#include "OsmNdTagHandler.h"
#include "OsmWayGraphicsItem.h"

namespace Marble
{
namespace osm
{

static GeoTagHandlerRegistrar s_handler( GeoTagHandler::QualifiedName(osmTag_nd, ""),
                                         new OsmNdTagHandler() );


GeoNode* OsmNdTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() );

    GeoStackItem parent = parser.parentElement();

    GeoNode* wayNode = parent.associatedNode();

    OsmWayGraphicsItem* way = (OsmWayGraphicsItem*)wayNode;

    //the line that crashes
//    way->addNodeReferenceId( parser.attribute("ref").toInt() );

    return 0;

}
}
}
