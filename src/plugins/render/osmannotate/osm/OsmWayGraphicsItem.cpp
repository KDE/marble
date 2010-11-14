//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmWayGraphicsItem.h"

#include "GeoDataPlacemark.h"
#include "GeoPainter.h"
#include "OsmNodeGraphicsItem.h"
#include "OsmNodeTagHandler.h"
#include "MarbleDebug.h"

#include <QtCore/QMap>

namespace Marble
{

OsmWayGraphicsItem::OsmWayGraphicsItem()
        : GeoGraphicsItem()
{
    m_placemark = new GeoDataPlacemark();
    m_placemark->setGeometry( new GeoDataLineString() );
}

OsmWayGraphicsItem::~OsmWayGraphicsItem()
{
    delete m_placemark;
}

void OsmWayGraphicsItem::addNodeReferenceId( int reference )
{
    if( osm::OsmNodeTagHandler::nodeRef.contains( reference ) ) {
        OsmNodeGraphicsItem* node = osm::OsmNodeTagHandler::nodeRef.value( reference );
        append( node->point() );
        //make node invisible only if its in a way
        node->setFlag( GeoGraphicsItem::ItemIsVisible, false );

    }
}

void OsmWayGraphicsItem::append( const GeoDataPoint& point )
{
    GeoDataLineString* line = dynamic_cast<GeoDataLineString*>(m_placemark->geometry());
    if( line ) {
        line->append( point );
    }
}

void OsmWayGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                const QString& renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED(viewport);
    Q_UNUSED(renderPos);
    Q_UNUSED(layer);
    painter->save();

    painter->setPen( Qt::black );
    if( m_placemark->geometry() ) {
        painter->drawPolyline( *m_placemark->geometry() );
    }

    painter->restore();
}

}
