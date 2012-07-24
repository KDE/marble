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

void GeoImageGraphicsItem::setImageFile( const QString &filename )
{
    m_imageFile = filename;
}

QString GeoImageGraphicsItem::imageFile() const
{
    return m_imageFile;
}

void GeoImageGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                  const QString& renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    bool unloadImage = true;
    if ( viewport->projection() != Spherical ) {
        qreal x1(0.0), x2(0.0), y1( 0.0 ), y2( 0.0 );
        viewport->screenCoordinates( m_latLonBox.west(), m_latLonBox.north(), x1, y1 );
        viewport->screenCoordinates( m_latLonBox.east(), m_latLonBox.south(), x2, y2 );
        QRectF const screen( QPointF( 0, 0), viewport->size() );
        QRectF const position( x1, y1, x2-x1, y2-y1 );
        if ( !(screen & position).isEmpty() ) {
            if ( m_image.isNull() && !m_imageFile.isEmpty() ) {
                /** @todo: Load in a thread */
                m_image = QImage( m_imageFile );
            }
            unloadImage = false;
            painter->drawImage( position, m_image );
        }
    } else {
        /** @todo: Implement for spherical projection, possibly reusing code
                   from SphericalScanlineTextureMapper */
    }
    /** @todo: Respect x-repeat */

    if ( unloadImage && !m_imageFile.isEmpty() ) {
        // No unloading if no path is known
        m_image = QImage();
    }
}

GeoDataLatLonAltBox GeoImageGraphicsItem::latLonAltBox() const
{
    return m_latLonBox;
}

}
