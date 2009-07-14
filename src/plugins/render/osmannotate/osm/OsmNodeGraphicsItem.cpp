//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmNodeGraphicsItem.h"

#include "GeoPainter.h"

namespace Marble
{

OsmNodeGraphicsItem::OsmNodeGraphicsItem()
        : GeoGraphicsItem()
{
    m_pen.setColor( Qt::black );
}

void OsmNodeGraphicsItem::paint( GeoPainter *painter, ViewportParams* viewport,
                                 const QString &renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    //FIXME GeoPainter should have a method to directly draw a
    //GeoDataPoint. after that most of this code can be removed
    qreal lon, lat;
    m_point.geoCoordinates( lon, lat );

    painter->save();
    //stop points from blurring
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setPen(m_pen);
    painter->drawPoint( m_point );
    painter->restore();

}

void OsmNodeGraphicsItem::setPoint( const GeoDataCoordinates& point )
{
    m_point = GeoDataCoordinates( point );
}

}
