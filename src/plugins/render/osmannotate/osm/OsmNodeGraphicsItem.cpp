//
// This file is part of the Marble Virtual Globe.
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
        : GeoPointGraphicsItem()
{
    m_pen.setColor( Qt::black );
}

void OsmNodeGraphicsItem::paint( GeoPainter *painter, ViewportParams* viewport,
                                 const QString &renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )
    Q_UNUSED( viewport )

    painter->save();
    //stop points from blurring
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setPen(m_pen);
    painter->drawRect( m_point, 2, 2, false );
    painter->restore();

}

void OsmNodeGraphicsItem::setId( int id )
{
    m_id = id;
}

int OsmNodeGraphicsItem::id()
{
    return m_id;
}


}
