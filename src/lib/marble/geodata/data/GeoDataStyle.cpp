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
#include "GeoDataBalloonStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataListStyle.h"
#include "GeoDataPolyStyle.h"

namespace Marble
{

class GeoDataStylePrivate
{
  public:
    GeoDataStylePrivate()
    {
    }

    GeoDataStylePrivate(const QString& iconPath,
                        const QFont &font, const QColor &color )
        : m_iconStyle( iconPath ),
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
    GeoDataListStyle m_listStyle;
};

GeoDataStyle::GeoDataStyle()
    : d( new GeoDataStylePrivate )
{
}

GeoDataStyle::GeoDataStyle( const GeoDataStyle& other )
    : GeoDataStyleSelector( other ), d( new GeoDataStylePrivate( *other.d ) )
{
}

GeoDataStyle::GeoDataStyle( const QString& iconPath,
                            const QFont &font, const QColor &color  )
    : d( new GeoDataStylePrivate( iconPath, font, color ) )
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

bool GeoDataStyle::operator==( const GeoDataStyle &other ) const
{
    if ( GeoDataStyleSelector::operator!=( other ) ) {
        return false;
    }

    return d->m_iconStyle == other.d->m_iconStyle &&
           d->m_labelStyle == other.d->m_labelStyle &&
           d->m_lineStyle == other.d->m_lineStyle &&
           d->m_polyStyle == other.d->m_polyStyle &&
           d->m_balloonStyle == other.d->m_balloonStyle &&
           d->m_listStyle == other.d->m_listStyle;
}

bool GeoDataStyle::operator!=( const GeoDataStyle &other ) const
{
    return !this->operator==( other );
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

void GeoDataStyle::setListStyle( const GeoDataListStyle& style )
{
    d->m_listStyle = style;
    d->m_listStyle.setParent( this );
}

GeoDataIconStyle& GeoDataStyle::iconStyle()
{
    return d->m_iconStyle;
}

const GeoDataIconStyle& GeoDataStyle::iconStyle() const
{
    return d->m_iconStyle;
}

GeoDataLineStyle& GeoDataStyle::lineStyle()
{
    return d->m_lineStyle;
}

const GeoDataLineStyle& GeoDataStyle::lineStyle() const
{
    return d->m_lineStyle;
}

GeoDataPolyStyle& GeoDataStyle::polyStyle()
{
    return d->m_polyStyle;
}

const GeoDataPolyStyle& GeoDataStyle::polyStyle() const
{
    return d->m_polyStyle;
}

GeoDataLabelStyle& GeoDataStyle::labelStyle()
{
    return d->m_labelStyle;
}

const GeoDataLabelStyle& GeoDataStyle::labelStyle() const
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

GeoDataListStyle& GeoDataStyle::listStyle()
{
    return d->m_listStyle;
}

const GeoDataListStyle& GeoDataStyle::listStyle() const
{
    return d->m_listStyle;
}

void GeoDataStyle::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );

    d->m_iconStyle.pack( stream );
    d->m_labelStyle.pack( stream );
    d->m_polyStyle.pack( stream );
    d->m_lineStyle.pack( stream );
    d->m_balloonStyle.pack( stream );
    d->m_listStyle.pack( stream );
}

void GeoDataStyle::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );

    d->m_iconStyle.unpack( stream );
    d->m_labelStyle.unpack( stream );
    d->m_lineStyle.unpack( stream );
    d->m_polyStyle.unpack( stream );
    d->m_balloonStyle.unpack( stream );
    d->m_listStyle.unpack( stream );
}

}
