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

#include "GeoDataFeature.h"
#include "GeoDataLineString.h"
#include "GeoDataLineStyle.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"

namespace Marble
{

GeoLineStringGraphicsItem::GeoLineStringGraphicsItem( const GeoDataFeature *feature,
                                                      const GeoDataLineString* lineString )
        : GeoGraphicsItem( feature ),
          m_lineString( lineString )
{
}


const float GeoLineStringGraphicsItem::s_outlineZValue = -0.001;

void GeoLineStringGraphicsItem::createDecorations()
{
    if ( style() != nullptr ) {
        if ( style()->lineStyle().cosmeticOutline() ) {
            GeoLineStringGraphicsItem* outline = new GeoLineStringGraphicsItem(this->feature(), this->m_lineString);
            outline->setZValue(this->zValue() + s_outlineZValue);

            this->addDecoration(outline);
        }
    }
}

void GeoLineStringGraphicsItem::setLineString( const GeoDataLineString* lineString )
{
    m_lineString = lineString;
}

const GeoDataLatLonAltBox& GeoLineStringGraphicsItem::latLonAltBox() const
{
    return m_lineString->latLonAltBox();
}

void GeoLineStringGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport )
{
    LabelPositionFlags labelPositionFlags = NoLabel;

    painter->save();

    QPen currentPen = painter->pen();
    if ( !style() ) {
        painter->setPen( QPen() );
    }
    else {

        if ( currentPen.color() != style()->lineStyle().paintedColor() )
            currentPen.setColor( style()->lineStyle().paintedColor() );

        if ( currentPen.widthF() != style()->lineStyle().width() ||
                style()->lineStyle().physicalWidth() != 0.0 ) {
            if ( float( viewport->radius() ) / EARTH_RADIUS * style()->lineStyle().physicalWidth() < style()->lineStyle().width() )
                currentPen.setWidthF( style()->lineStyle().width() );
            else
                currentPen.setWidthF( float( viewport->radius() ) / EARTH_RADIUS * style()->lineStyle().physicalWidth() );
        }
        else if ( style()->lineStyle().width() != 0.0 ) {
            currentPen.setWidthF( style()->lineStyle().width() );
        }

        if ( currentPen.capStyle() != style()->lineStyle().capStyle() )
            currentPen.setCapStyle( style()->lineStyle().capStyle() );

        if ( currentPen.style() != style()->lineStyle().penStyle() )
            currentPen.setStyle( style()->lineStyle().penStyle() );

        if ( style()->lineStyle().penStyle() == Qt::CustomDashLine )
            currentPen.setDashPattern( style()->lineStyle().dashPattern() );

        if ( painter->mapQuality() != Marble::HighQuality
                && painter->mapQuality() != Marble::PrintQuality ) {
            QColor penColor = currentPen.color();
            penColor.setAlpha( 255 );
            currentPen.setColor( penColor );
        }

        if ( painter->pen() != currentPen )
            painter->setPen( currentPen );

        if ( style()->lineStyle().background() ) {
            QBrush brush = painter->background();
            brush.setColor( style()->polyStyle().paintedColor() );
            painter->setBackground( brush );

            painter->setBackgroundMode( Qt::OpaqueMode );
        }

        // label styles
        painter->setFont( style()->labelStyle().font() );
        switch ( style()->labelStyle().alignment() ) {
        case GeoDataLabelStyle::Corner:
        case GeoDataLabelStyle::Right:
            labelPositionFlags |= LineStart;
            break;
        case GeoDataLabelStyle::Center:
            labelPositionFlags |= LineCenter;
            break;
        }
    }

    if ( ! ( isDecoration() && currentPen.widthF() < 2.5f ) )
    {
        if( style()->lineStyle().cosmeticOutline() &&
            style()->lineStyle().penStyle() == Qt::SolidLine ) {
            if ( isDecoration() ) {
                painter->drawPolyline( *m_lineString, "", NoLabel );
            } else {
                if ( currentPen.widthF() > 2.5f ) {
                    currentPen.setWidthF( currentPen.widthF() - 2.0f );
                }
                currentPen.setColor( style()->polyStyle().paintedColor() );
                painter->setPen( currentPen );
                painter->drawPolyline( *m_lineString, feature()->name(), FollowLine,
                                        style()->labelStyle().paintedColor(),
                                        style()->labelStyle().font());
            }
        } else {
            painter->drawPolyline( *m_lineString, feature()->name(), labelPositionFlags,
                                   style()->labelStyle().paintedColor(),
                                   style()->labelStyle().font() );
        }
    }

    painter->restore();
}

}
