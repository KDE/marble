//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoImageGraphicsItem.h"

#include "GeoPainter.h"
#include "ViewportParams.h"

namespace Marble
{

void GeoImageGraphicsItem::setLatLonBox( const GeoDataLatLonBox &box )
{
    m_latLonBox = box;
}

GeoDataLatLonBox GeoImageGraphicsItem::latLonBox() const
{
    return m_latLonBox;
}

void GeoImageGraphicsItem::setImage( const QImage &image )
{
    m_image = image;
}

QImage GeoImageGraphicsItem::image() const
{
    return m_image;
}

void GeoImageGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                  const QString& renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if ( viewport->projection() != Spherical ) {
        qreal x1(0.0), x2(0.0), y1( 0.0 ), y2( 0.0 );
        viewport->screenCoordinates( m_latLonBox.west(), m_latLonBox.north(), x1, y1 );
        viewport->screenCoordinates( m_latLonBox.east(), m_latLonBox.south(), x2, y2 );
        painter->drawImage( QRectF( x1, y1, x2-x1, y2-y1 ), m_image );
    } else {
        /** @todo: Implement for spherical projection, possibly reusing code
                   from SphericalScanlineTextureMapper */
    }
    /** @todo: Respect x-repeat */
}

}
