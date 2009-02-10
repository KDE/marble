//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataStyle.h"

namespace Marble
{

class GeoDataStylePrivate
{
  public:
    GeoDataStylePrivate()
        : m_iconStyle( 0 ),
          m_labelStyle( new GeoDataLabelStyle ),
          m_lineStyle( 0 ),
          m_polyStyle( 0 )
    {
    }

    GeoDataStylePrivate(const QPixmap& icon, 
                        const QFont &font, const QColor &color )
        : m_iconStyle( new GeoDataIconStyle( icon ) ),
          m_labelStyle( new GeoDataLabelStyle( font, color ) ),
          m_lineStyle( new GeoDataLineStyle( color ) ),
          m_polyStyle( new GeoDataPolyStyle( color ) )
    {
    }


    ~GeoDataStylePrivate()
    {
        delete m_labelStyle;
        delete m_iconStyle;
        delete m_lineStyle;
        delete m_polyStyle;
    }

    GeoDataIconStyle   *m_iconStyle;
    GeoDataLabelStyle  *m_labelStyle;
    GeoDataLineStyle   *m_lineStyle;
    GeoDataPolyStyle   *m_polyStyle;
    // BalloonStyle
    // ListStyle
};

GeoDataStyle::GeoDataStyle()
    : d( new GeoDataStylePrivate() )
{
}

GeoDataStyle::GeoDataStyle( const GeoDataStyle& other )
    : GeoDataStyleSelector( other ), d( new GeoDataStylePrivate( *other.d ) )
{
}

GeoDataStyle::GeoDataStyle( const QPixmap& icon, 
                            const QFont &font, const QColor &color  )
    : d( new GeoDataStylePrivate(icon, font, color) )
{
}

GeoDataStyle::~GeoDataStyle()
{
    delete d;
}

void GeoDataStyle::setIconStyle( const GeoDataIconStyle& style )
{
    delete d->m_iconStyle;
    d->m_iconStyle = new GeoDataIconStyle( style );
}

void GeoDataStyle::setLineStyle( const GeoDataLineStyle& style )
{
    delete d->m_lineStyle;
    d->m_lineStyle = new GeoDataLineStyle( style );
}

void GeoDataStyle::setLabelStyle( const GeoDataLabelStyle& style )
{
    delete d->m_labelStyle;
    d->m_labelStyle = new GeoDataLabelStyle( style );
}

void GeoDataStyle::setPolyStyle( const GeoDataPolyStyle& style )
{
    delete d->m_polyStyle;
    d->m_polyStyle = new GeoDataPolyStyle( style );
}

GeoDataIconStyle& GeoDataStyle::iconStyle() const
{
    return *d->m_iconStyle;
}

GeoDataLineStyle& GeoDataStyle::lineStyle() const
{
    return *d->m_lineStyle;
}

GeoDataPolyStyle& GeoDataStyle::polyStyle() const
{
    return *d->m_polyStyle;
}

GeoDataLabelStyle& GeoDataStyle::labelStyle() const
{
    return *d->m_labelStyle;
}

void GeoDataStyle::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );

    if( !d->m_iconStyle ) d->m_iconStyle = new GeoDataIconStyle;
    if( !d->m_labelStyle ) d->m_labelStyle = new GeoDataLabelStyle;
    if( !d->m_lineStyle ) d->m_lineStyle = new GeoDataLineStyle;
    if( !d->m_polyStyle ) d->m_polyStyle = new GeoDataPolyStyle;

    d->m_iconStyle->pack( stream );
    d->m_labelStyle->pack( stream );
    d->m_polyStyle->pack( stream );
    d->m_lineStyle->pack( stream );
}

void GeoDataStyle::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );

    if( !d->m_iconStyle ) d->m_iconStyle = new GeoDataIconStyle;
    if( !d->m_labelStyle ) d->m_labelStyle = new GeoDataLabelStyle;
    if( !d->m_lineStyle ) d->m_lineStyle = new GeoDataLineStyle;
    if( !d->m_polyStyle ) d->m_polyStyle = new GeoDataPolyStyle;

    d->m_iconStyle->unpack( stream );
    d->m_labelStyle->unpack( stream );
    d->m_lineStyle->unpack( stream );
    d->m_polyStyle->unpack( stream );
}

}
