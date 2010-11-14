//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmNdTagHandler.h"

#include "GeoParser.h"
#include "OsmElementDictionary.h"
#include "OsmWayGraphicsItem.h"

#include "MarbleDebug.h"

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

    //FIXME: problem with the stack implementation

    //this should be "way"
//    mDebug() << Q_FUNC_INFO;
//    mDebug() << "parent element name:" << parent.first.first;
    Q_ASSERT( parent.first.first.compare("way") == 0 );

    //this should be set because we know that the containing
    // <way> element did not return 0
//    mDebug() << " parent associated node pointer:" << parent.associatedNode();

    Q_ASSERT( parent.associatedNode() );

    GeoNode* wayNode = parent.associatedNode();

    QString n = parser.name().toString();

    OsmWayGraphicsItem* way = reinterpret_cast<OsmWayGraphicsItem*>(wayNode);

    Q_ASSERT( way );

    //the line that crashes
    way->addNodeReferenceId( parser.attribute("ref").toInt() );

    return 0;

}
}
}
