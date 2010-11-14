//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoLineStringGraphicsItem.h"

#include "GeoPainter.h"

namespace Marble
{

GeoLineStringGraphicsItem::GeoLineStringGraphicsItem()
        : GeoGraphicsItem()
{
}

void GeoLineStringGraphicsItem::setLineString(const GeoDataLineString& lineString )
{
    m_lineString = GeoDataLineString( lineString );
}

void GeoLineStringGraphicsItem::append( const GeoDataCoordinates& coordinates)
{
    m_lineString.append( coordinates );
}

void GeoLineStringGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                       const QString& renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED(viewport);
    Q_UNUSED(renderPos);
    Q_UNUSED(layer);
    painter->drawPolyline( m_lineString );
}

}
