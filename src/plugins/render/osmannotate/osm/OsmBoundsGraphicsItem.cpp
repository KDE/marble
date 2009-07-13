//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmBoundsGraphicsItem.h"

#include <QtCore/QDebug>

namespace Marble{

OsmBoundsGraphicsItem::OsmBoundsGraphicsItem()
{
}

void OsmBoundsGraphicsItem::append( const GeoDataCoordinates& value )
{
    m_lineString.append( value );
}

void OsmBoundsGraphicsItem::paint( GeoPainter* painter, ViewportParams * viewport,
                                   const QString &renderPos, GeoSceneLayer* layer )
{

    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    qDebug() << "Painting from OSMBounds";

}

}
