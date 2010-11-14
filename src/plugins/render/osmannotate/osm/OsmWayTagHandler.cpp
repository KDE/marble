//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmWayTagHandler.h"

#include "GeoParser.h"
#include "OsmElementDictionary.h"
#include "OsmWayGraphicsItem.h"
#include <QDebug>

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar osmWayTagHandler( GeoTagHandler::QualifiedName(osmTag_way, ""),
                                               new OsmWayTagHandler() );

GeoNode* OsmWayTagHandler::parse( GeoParser& parser ) const
{
    GeoNode* result;
    OsmWayGraphicsItem* way = new OsmWayGraphicsItem();

    way->setFlag( GeoGraphicsItem::ItemIsVisible );

    parser.activeModel()->append(way);

    result = reinterpret_cast<GeoNode*>(way);
    Q_ASSERT( result );
    return result;
}

}

}
