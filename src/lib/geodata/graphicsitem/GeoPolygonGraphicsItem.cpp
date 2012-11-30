//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoPolygonGraphicsItem.h"

#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"

namespace Marble
{

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem( const GeoDataPolygon* polygon )
        : GeoGraphicsItem(),
          m_polygon( polygon ),
          m_ring( 0 )
{
}

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem( const GeoDataLinearRing* ring )
        : GeoGraphicsItem(),
          m_polygon( 0 ),
          m_ring( ring )
{
}

GeoDataCoordinates GeoPolygonGraphicsItem::coordinate() const
{
    if( m_polygon ) {
        return m_polygon->latLonAltBox().center();
    } else if ( m_ring ) {
        return m_ring->latLonAltBox().center();
    } else {
        return GeoDataCoordinates();
    }
}

GeoDataLatLonAltBox& GeoPolygonGraphicsItem::latLonAltBox() const
{
    if( m_polygon ) {
        return m_polygon->latLonAltBox();
    } else if ( m_ring ) {
        return m_ring->latLonAltBox();
    } else {
        return GeoGraphicsItem::latLonAltBox();
    }
}

void GeoPolygonGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport )
{
    Q_UNUSED( viewport );

    if ( !style() )
    {
        painter->save();
        painter->setPen( QPen() );
        if ( m_polygon ) {
            painter->drawPolygon( *m_polygon );
        } else if ( m_ring ) {
            painter->drawPolygon( *m_ring );
        }
        painter->restore();
        return;
    }

    painter->save();
    QPen currentPen = painter->pen();

    if ( !style()->polyStyle().outline() )
    {
        currentPen.setColor( Qt::transparent );
    }
    else
    {
        if ( currentPen.color() != style()->lineStyle().paintedColor() ||
                currentPen.widthF() != style()->lineStyle().width() )
        {
            currentPen.setColor( style()->lineStyle().paintedColor() );
            currentPen.setWidthF( style()->lineStyle().width() );
        }

        if ( currentPen.capStyle() != style()->lineStyle().capStyle() )
            currentPen.setCapStyle( style()->lineStyle().capStyle() );

        if ( currentPen.style() != style()->lineStyle().penStyle() )
            currentPen.setStyle( style()->lineStyle().penStyle() );

        if ( painter->mapQuality() != Marble::HighQuality
                && painter->mapQuality() != Marble::PrintQuality )
        {
            QColor penColor = currentPen.color();
            penColor.setAlpha( 255 );
            currentPen.setColor( penColor );
        }
    }
    if ( painter->pen() != currentPen ) painter->setPen( currentPen );

    if ( !style()->polyStyle().fill() )
    {
        if ( painter->brush().color() != Qt::transparent )
            painter->setBrush( QColor( Qt::transparent ) );
    }
    else
    {
        if ( painter->brush().color() != style()->polyStyle().paintedColor() )
        {
            painter->setBrush( style()->polyStyle().paintedColor() );
        }
    }

    if ( m_polygon ) {
        painter->drawPolygon( *m_polygon );
    } else if ( m_ring ) {
        painter->drawPolygon( *m_ring );
    }
    painter->restore();
}

}
