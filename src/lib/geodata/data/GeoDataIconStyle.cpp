//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataIconStyle.h"

#include <QtCore/QDebug>

GeoDataIconStyle::GeoDataIconStyle()
  : m_scale( 1.0 ),
    m_icon( QPixmap() ),
    m_hotSpot( new GeoDataHotSpot() )
{
}

GeoDataIconStyle::GeoDataIconStyle( const QPixmap& icon, const QPointF &hotSpot )
  : m_scale( 1.0 ),
    m_icon( icon ),
    m_hotSpot( new GeoDataHotSpot( hotSpot ) )
{
}

GeoDataIconStyle::~GeoDataIconStyle()
{
    delete m_hotSpot;
}

void GeoDataIconStyle::setIcon( const QPixmap &icon )
{
    m_icon = icon;
}

QPixmap GeoDataIconStyle::icon() const
{
    return m_icon;
}

void GeoDataIconStyle::setHotSpot( const QPointF& hotSpot, 
                                   GeoDataHotSpot::Units xunits,
                                   GeoDataHotSpot::Units yunits )
{
    m_hotSpot->setHotSpot( hotSpot, xunits, uunits );
}

const QPointF& GeoDataIconStyle::hotSpot() const // always in pixels, Origin upper left
{
    GeoDataHotSpot::Units xunits;
    GeoDataHotSpot::Units yunits;

    m_pixelHotSpot = m_hotSpot->hotSpot( xunits, yunits );

    if ( xunits == GeoDataHotSpot::Fraction )
        m_pixelHotSpot.setX( m_icon.width()  * m_pixelHotSpot.x() );
    else {
        if ( xunits == GeoDataHotSpot::InsetPixels )
            m_pixelHotSpot.setX( m_icon.width()  - m_pixelHotSpot.x() );
    }

    if ( yunits == GeoDataHotSpot::Fraction )
        m_pixelHotSpot.setY( m_icon.height() * ( 1.0 - m_pixelHotSpot.y() ) );
    else {
        if ( yunits == GeoDataHotSpot::Pixels )
            m_pixelHotSpot.setY( m_icon.height() - m_pixelHotSpot.y() );
    }

    return m_pixelHotSpot;
}

void GeoDataIconStyle::setScale( const float &scale )
{
    m_scale = scale;
}

float GeoDataIconStyle::scale() const
{
    return m_scale;
}

void GeoDataIconStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << m_icon;
}

void GeoDataIconStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );

    stream >> m_icon;
}
