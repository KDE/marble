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

class GeoDataIconStylePrivate
{
  public:
    GeoDataIconStylePrivate() :
        m_scale( 1.0 ),
        m_icon( QPixmap() ),
        m_hotSpot( new GeoDataHotSpot() )
    {
    }

    ~GeoDataIconStylePrivate()
    {
    }

    float            m_scale;

    QPixmap          m_icon;    // To save memory we use a pointer
    GeoDataHotSpot  *m_hotSpot; // default unit is "fraction"
    mutable QPointF  m_pixelHotSpot;
};

GeoDataIconStyle::GeoDataIconStyle() :
    d( new GeoDataIconStylePrivate() )
{
}

GeoDataIconStyle::GeoDataIconStyle( const QPixmap& icon, const QPointF &hotSpot ) :
    d( new GeoDataIconStylePrivate() )
{
    d->m_icon = icon;
    d->m_hotSpot->setHotSpot(hotSpot);
}

GeoDataIconStyle::~GeoDataIconStyle()
{
    delete d->m_hotSpot;
    delete d;
}

void GeoDataIconStyle::setIcon( const QPixmap &icon )
{
    d->m_icon = icon;
}

QPixmap GeoDataIconStyle::icon() const
{
    return d->m_icon;
}

void GeoDataIconStyle::setHotSpot( const QPointF& hotSpot, 
                                   GeoDataHotSpot::Units xunits,
                                   GeoDataHotSpot::Units yunits )
{
    d->m_hotSpot->setHotSpot( hotSpot, xunits, yunits );
}

const QPointF& GeoDataIconStyle::hotSpot() const // always in pixels, Origin upper left
{
    GeoDataHotSpot::Units xunits;
    GeoDataHotSpot::Units yunits;

    d->m_pixelHotSpot = d->m_hotSpot->hotSpot( xunits, yunits );

    if ( xunits == GeoDataHotSpot::Fraction )
        d->m_pixelHotSpot.setX( d->m_icon.width()  * d->m_pixelHotSpot.x() );
    else {
        if ( xunits == GeoDataHotSpot::InsetPixels )
            d->m_pixelHotSpot.setX( d->m_icon.width()  - d->m_pixelHotSpot.x() );
    }

    if ( yunits == GeoDataHotSpot::Fraction )
        d->m_pixelHotSpot.setY( d->m_icon.height() * ( 1.0 - d->m_pixelHotSpot.y() ) );
    else {
        if ( yunits == GeoDataHotSpot::Pixels )
            d->m_pixelHotSpot.setY( d->m_icon.height() - d->m_pixelHotSpot.y() );
    }

    return d->m_pixelHotSpot;
}

void GeoDataIconStyle::setScale( const float &scale )
{
    d->m_scale = scale;
}

float GeoDataIconStyle::scale() const
{
    return d->m_scale;
}

void GeoDataIconStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << d->m_icon;
}

void GeoDataIconStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );

    stream >> d->m_icon;
}
