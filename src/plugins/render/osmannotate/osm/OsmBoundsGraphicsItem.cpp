//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmBoundsGraphicsItem.h"

#include "GeoDataLinearRing.h"
#include "GeoPainter.h"

#include "MarbleDebug.h"

namespace Marble
{

OsmBoundsGraphicsItem::OsmBoundsGraphicsItem()
        : GeoGraphicsItem()
{
    m_pen.setColor( Qt::black );
    m_pen.setStyle( Qt::SolidLine );
    m_pen.setWidth( 1 );
}

void OsmBoundsGraphicsItem::append( const GeoDataCoordinates& value )
{
    m_lineString.append( value );
    setCoordinate( m_lineString.latLonAltBox().center() );
}

void OsmBoundsGraphicsItem::paint( GeoPainter* painter, ViewportParams * viewport,
                                   const QString &renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )
    Q_UNUSED( viewport )

    painter->save();
    painter->setPen( m_pen );

    painter->drawPolygon( GeoDataLinearRing(m_lineString) );

    painter->restore();

}

GeoDataLatLonBox OsmBoundsGraphicsItem::latLonBox()
{
    return GeoDataLatLonBox( m_lineString.latLonAltBox() );
}

}
