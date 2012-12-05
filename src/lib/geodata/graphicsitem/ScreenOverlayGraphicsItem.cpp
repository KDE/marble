//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "ScreenOverlayGraphicsItem.h"

#include "ViewportParams.h"

namespace Marble {

ScreenOverlayGraphicsItem::ScreenOverlayGraphicsItem( const GeoDataScreenOverlay *screenOverlay ) :
    m_screenOverlay( screenOverlay )
{
    /** @todo: take the possible size values into account according to kml spec */
    setSize( QSizeF( m_screenOverlay->size().x(), m_screenOverlay->size().y() ) );

    if ( !m_screenOverlay->icon().isNull() ) {
        m_image = m_screenOverlay->icon().scaled( size().toSize() );
    }
}

void ScreenOverlayGraphicsItem::setProjection( const ViewportParams *viewport )
{
    ScreenGraphicsItem::setProjection( viewport );
    /** @todo: take overlayXY into account */
    setPosition( QPointF( pixelValue( m_screenOverlay->screenXY().xunit(),
                                      viewport->width(),
                                      size().width(),
                                      m_screenOverlay->screenXY().x() ),
       viewport->height()-pixelValue( m_screenOverlay->screenXY().yunit(),
                                      viewport->height(),
                                      size().height(),
                                      m_screenOverlay->screenXY().y() ) ) );
}

qreal ScreenOverlayGraphicsItem::pixelValue( GeoDataVec2::Unit unit, qreal screenSize, qreal imageSize, qreal value ) const
{
    switch (unit) {
    case GeoDataVec2::Pixels:
        return imageSize;
        break;
    case GeoDataVec2::Fraction:
        return value * screenSize;
        break;
    case GeoDataVec2::InsetPixels:
        return screenSize - imageSize - value;
        break;
    }

    Q_ASSERT( false );
    return 0.0;
}

void ScreenOverlayGraphicsItem::paint( QPainter *painter )
{
    if ( m_image.isNull() ) {
        painter->setBrush( m_screenOverlay->color() );
        painter->drawRect( QRectF( QPointF( 0.0, 0.0 ), size() ) );
    } else {
        painter->drawImage( QPointF( 0.0, 0.0 ), m_image );
    }
}

}
