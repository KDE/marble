//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataStyle.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataStylePrivate
{
  public:
    GeoDataStylePrivate()
    {
    }

    GeoDataStylePrivate(const QImage& icon,
                        const QFont &font, const QColor &color )
        : m_iconStyle( icon ),
          m_labelStyle( font, color ),
          m_lineStyle( color ),
          m_polyStyle( color ),
          m_balloonStyle()
    {
    }

    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataStyleType;
    }

    GeoDataIconStyle   m_iconStyle;
    GeoDataLabelStyle  m_labelStyle;
    GeoDataLineStyle   m_lineStyle;
    GeoDataPolyStyle   m_polyStyle;
    GeoDataBalloonStyle m_balloonStyle;
    // ListStyle
};

GeoDataStyle::GeoDataStyle()
    : d( new GeoDataStylePrivate )
{
}

GeoDataStyle::GeoDataStyle( const GeoDataStyle& other )
    : GeoDataStyleSelector( other ), d( new GeoDataStylePrivate( *other.d ) )
{
}

GeoDataStyle::GeoDataStyle( const QImage& icon,
                            const QFont &font, const QColor &color  )
    : d( new GeoDataStylePrivate( icon, font, color ) )
{
}

GeoDataStyle::~GeoDataStyle()
{
    delete d;
}

GeoDataStyle& GeoDataStyle::operator=( const GeoDataStyle& other )
{
    GeoDataStyleSelector::operator=( other );
    *d = *other.d;
    return *this;
}

const char* GeoDataStyle::nodeType() const
{
    return d->nodeType();
}

void GeoDataStyle::setIconStyle( const GeoDataIconStyle& style )
{
    d->m_iconStyle = style;
    d->m_iconStyle.setParent( this );
}

void GeoDataStyle::setLineStyle( const GeoDataLineStyle& style )
{
    d->m_lineStyle = style;
}

void GeoDataStyle::setLabelStyle( const GeoDataLabelStyle& style )
{
    d->m_labelStyle = style;
}

void GeoDataStyle::setPolyStyle( const GeoDataPolyStyle& style )
{
    d->m_polyStyle = style;
}

void GeoDataStyle::setBalloonStyle( const GeoDataBalloonStyle& style )
{
    d->m_balloonStyle = style;
}

GeoDataIconStyle& GeoDataStyle::iconStyle() const
{
    return d->m_iconStyle;
}

GeoDataLineStyle& GeoDataStyle::lineStyle() const
{
    return d->m_lineStyle;
}

GeoDataPolyStyle& GeoDataStyle::polyStyle() const
{
    return d->m_polyStyle;
}

GeoDataLabelStyle& GeoDataStyle::labelStyle() const
{
    return d->m_labelStyle;
}

GeoDataBalloonStyle& GeoDataStyle::balloonStyle()
{
    return d->m_balloonStyle;
}

const GeoDataBalloonStyle& GeoDataStyle::balloonStyle() const
{
    return d->m_balloonStyle;
}

void GeoDataStyle::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );

    d->m_iconStyle.pack( stream );
    d->m_labelStyle.pack( stream );
    d->m_polyStyle.pack( stream );
    d->m_lineStyle.pack( stream );
    d->m_balloonStyle.pack( stream );
}

void GeoDataStyle::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );

    d->m_iconStyle.unpack( stream );
    d->m_labelStyle.unpack( stream );
    d->m_lineStyle.unpack( stream );
    d->m_polyStyle.unpack( stream );
    d->m_balloonStyle.unpack( stream );
}

}
