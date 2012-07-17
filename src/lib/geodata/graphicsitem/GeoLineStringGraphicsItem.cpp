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

#include "GeoDataLineString.h"
#include "GeoDataLineStyle.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"

namespace Marble
{

GeoLineStringGraphicsItem::GeoLineStringGraphicsItem( const GeoDataLineString* lineString )
        : GeoGraphicsItem(),
          m_lineString( lineString )
{
}

void GeoLineStringGraphicsItem::setLineString( const GeoDataLineString* lineString )
{
    m_lineString = lineString;
}

GeoDataCoordinates GeoLineStringGraphicsItem::coordinate() const
{
    return m_lineString->latLonAltBox().center();
}

void GeoLineStringGraphicsItem::coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const
{
    GeoDataCoordinates coords = m_lineString->latLonAltBox().center();
    longitude = coords.longitude();
    latitude = coords.latitude();
    altitude = coords.altitude();
}

GeoDataLatLonAltBox& GeoLineStringGraphicsItem::latLonAltBox() const
{
    return m_lineString->latLonAltBox();
}

void GeoLineStringGraphicsItem::paint( GeoPainter* painter, ViewportParams* viewport,
                                       const QString& renderPos, GeoSceneLayer* layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if ( !style() )
    {
        painter->save();
        painter->setPen( QPen() );
        painter->drawPolyline( *m_lineString );
        painter->restore();
        return;
    }
    
    if(style()->lineStyle().color() == Qt::transparent)
        return;

    painter->save();
    QPen currentPen = painter->pen();

    if ( currentPen.color() != style()->lineStyle().color() )
        currentPen.setColor( style()->lineStyle().color() );

    if ( currentPen.widthF() != style()->lineStyle().width() ||
            style()->lineStyle().physicalWidth() != 0.0 )
    {
        if ( float( viewport->radius() ) / EARTH_RADIUS * style()->lineStyle().physicalWidth() < style()->lineStyle().width() )
            currentPen.setWidthF( style()->lineStyle().width() );
        else
            currentPen.setWidthF( float( viewport->radius() ) / EARTH_RADIUS * style()->lineStyle().physicalWidth() );
    }

    if ( currentPen.capStyle() != style()->lineStyle().capStyle() )
        currentPen.setCapStyle( style()->lineStyle().capStyle() );

    if ( currentPen.style() != style()->lineStyle().penStyle() )
        currentPen.setStyle( style()->lineStyle().penStyle() );
    
    if ( style()->lineStyle().penStyle() == Qt::CustomDashLine )
        currentPen.setDashPattern( style()->lineStyle().dashPattern() );

    if ( painter->mapQuality() != Marble::HighQuality
            && painter->mapQuality() != Marble::PrintQuality )
    {
        QColor penColor = currentPen.color();
        penColor.setAlpha( 255 );
        currentPen.setColor( penColor );
    }

    if ( painter->pen() != currentPen ) painter->setPen( currentPen );
    if ( style()->lineStyle().background() )
    {
        QBrush brush = painter->background();
        brush.setColor( style()->polyStyle().color() );
        painter->setBackground( brush );

        painter->setBackgroundMode( Qt::OpaqueMode );
    }
    painter->drawPolyline( *m_lineString );
    painter->restore();
}

}
