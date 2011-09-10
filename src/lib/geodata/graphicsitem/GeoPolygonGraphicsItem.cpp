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
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"

namespace Marble
{

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem( GeoDataPolygon* polygon )
        : GeoGraphicsItem(),
          m_polygon( polygon )
{
}

void GeoPolygonGraphicsItem::setPolygon( GeoDataPolygon* polygon )
{
    m_polygon = polygon;
    setCoordinate( m_polygon->latLonAltBox().center() );
    setLatLonAltBox( m_polygon->latLonAltBox() );
}

void GeoPolygonGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                    const QString& renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if ( !style() )
    {
        painter->save();
        painter->setPen( QPen() );
        painter->drawPolygon( *m_polygon );
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
        if ( currentPen.color() != style()->lineStyle().color() ||
                currentPen.widthF() != style()->lineStyle().width() )
        {
            currentPen.setColor( style()->lineStyle().color() );
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
        if ( painter->brush().color() != style()->polyStyle().color() )
        {
            painter->setBrush( style()->polyStyle().color() );
        }
    }

    painter->drawPolygon( *m_polygon );
    painter->restore();
}

}
