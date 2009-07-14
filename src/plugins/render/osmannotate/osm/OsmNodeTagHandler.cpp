//
// This file is part of the Marble Desktop Globe.
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

namespace Marble
{

namespace osm
{

static GeoTagHandlerRegistrar s_handlerbounds( GeoTagHandler::QualifiedName(osmTag_node, ""),
                                               new OsmNodeTagHandler() );

GeoNode* OsmNodeTagHandler::parse ( GeoParser& parser) const
{
    Q_ASSERT( parser.isStartElement() );

    OsmNodeGraphicsItem* item = new OsmNodeGraphicsItem();

    qreal lon = parser.attribute("lon").toDouble();
    qreal lat = parser.attribute("lat").toDouble();

    item->setPoint( GeoDataCoordinates(lon, lat, 0, GeoDataCoordinates::Degree ) );

    parser.activeModel()->append( item );

    return 0;
}

}

}
