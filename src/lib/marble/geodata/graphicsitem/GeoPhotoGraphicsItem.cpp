//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoPhotoGraphicsItem.h"

#include "GeoPainter.h"
#include "GeoDataStyle.h"
#include "ViewportParams.h"

#include <QImageReader>

#include <QDebug>

namespace Marble
{

GeoPhotoGraphicsItem::GeoPhotoGraphicsItem( const GeoDataFeature *feature )
    : GeoGraphicsItem( feature )
{
}

void GeoPhotoGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport )
{
    /* The code below loads the image lazily (only
    * when it will actually be displayed). Once it was
    * loaded but moves out of the viewport, it is unloaded
    * again. Otherwise memory consumption gets quite high
    * for a large set of photos
    */
    bool unloadImage = true;

    qreal x(0.0), y( 0.0 );
    viewport->screenCoordinates( m_point.coordinates(), x, y );

    QRectF position( QPointF( x, y ), style()->iconStyle().icon().size() );
    position.moveCenter( QPointF( x, y ) );

    QRectF displayed = position & QRectF( QPointF( 0, 0 ), viewport->size() );

    if ( !displayed.isEmpty() ) {
        if ( m_photo.isNull() ) {
            m_photo = style()->iconStyle().icon();
        }
        unloadImage = false;
        painter->drawImage( position, m_photo );
    }

    if ( unloadImage ) {
        m_photo = QImage();
    }
}

const GeoDataLatLonAltBox& GeoPhotoGraphicsItem::latLonAltBox() const
{
    return m_point.latLonAltBox();
}

void GeoPhotoGraphicsItem::setPoint( const GeoDataPoint &point )
{
    m_point = point;
}

GeoDataPoint GeoPhotoGraphicsItem::point() const
{
    return m_point;
}

}
