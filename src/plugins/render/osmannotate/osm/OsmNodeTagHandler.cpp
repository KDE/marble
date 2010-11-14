//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmNodeTagHandler.h"

#include "GeoParser.h"
#include "OsmElementDictionary.h"
#include "OsmNodeGraphicsItem.h"

#include <QtCore/QMap>

namespace Marble
{

namespace osm
{

QMap<int, OsmNodeGraphicsItem*> OsmNodeTagHandler::nodeRef;

static GeoTagHandlerRegistrar osmNodeTagHandler( GeoTagHandler::QualifiedName(osmTag_node, ""),
                                               new OsmNodeTagHandler() );

GeoNode* OsmNodeTagHandler::parse ( GeoParser& parser) const
{
    Q_ASSERT( parser.isStartElement() );

    OsmNodeGraphicsItem* item = new OsmNodeGraphicsItem();

    qreal lon = parser.attribute("lon").toDouble();
    qreal lat = parser.attribute("lat").toDouble();

    item->setPoint( GeoDataPoint(lon, lat, 0, GeoDataCoordinates::Degree ) );
    item->setId( parser.attribute("id").toInt() );

    nodeRef.insert( item->id(), item );

    parser.activeModel()->append( item );

    return 0;
}

}

}
