//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmWayGraphicsItem.h"

#include "GeoPainter.h"
#include "OsmNodeGraphicsItem.h"
#include "OsmNodeTagHandler.h"

#include <QtCore/QDebug>
#include <QtCore/QMap>

namespace Marble
{

OsmWayGraphicsItem::OsmWayGraphicsItem()
        : GeoLineStringGraphicsItem()
{
}

void OsmWayGraphicsItem::addNodeReferenceId( int reference )
{
    m_nodeIdList.append( reference );
    if( osm::OsmNodeTagHandler::nodeRef.contains( reference ) ) {
        OsmNodeGraphicsItem* node = osm::OsmNodeTagHandler::nodeRef.value( reference );
        append( node->point() );
        //make node invisible only if its in a way
        node->setFlag( GeoGraphicsItem::ItemIsVisible, false );

    }
}

void OsmWayGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                const QString& renderPos, GeoSceneLayer* layer )
{
    painter->save();
    painter->setPen( Qt::black );
    painter->drawPolyline( m_lineString );
    painter->restore();
}

}
