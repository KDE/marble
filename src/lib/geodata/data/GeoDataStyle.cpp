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
          m_labelStyle( 0 ),
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

GeoDataStyle::GeoDataStyle( const QPixmap& icon, 
                            const QFont &font, const QColor &color  )
    : d( new GeoDataStylePrivate(icon, font, color) )
{
}

GeoDataStyle::~GeoDataStyle()
{
    delete d;
}

void GeoDataStyle::setIconStyle( GeoDataIconStyle* style )
{
    delete d->m_iconStyle;
    d->m_iconStyle = style;
}

void GeoDataStyle::setLineStyle( GeoDataLineStyle* style )
{
    delete d->m_lineStyle;
    d->m_lineStyle = style;
}

void GeoDataStyle::setLabelStyle( GeoDataLabelStyle* style )
{
    delete d->m_labelStyle;
    d->m_labelStyle = style;
}

void GeoDataStyle::setPolyStyle( GeoDataPolyStyle* style )
{
    delete d->m_polyStyle;
    d->m_polyStyle = style;
}

GeoDataIconStyle* GeoDataStyle::iconStyle()
{
    return d->m_iconStyle;
}

GeoDataLineStyle* GeoDataStyle::lineStyle()
{
    return d->m_lineStyle;
}

GeoDataPolyStyle* GeoDataStyle::polyStyle()
{
    return d->m_polyStyle;
}

GeoDataLabelStyle* GeoDataStyle::labelStyle()
{
    return d->m_labelStyle;
}

void GeoDataStyle::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );

    d->m_iconStyle->pack( stream );
    d->m_labelStyle->pack( stream );
    d->m_polyStyle->pack( stream );
    d->m_lineStyle->pack( stream );
}

void GeoDataStyle::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );

    d->m_iconStyle->unpack( stream );
    d->m_labelStyle->unpack( stream );
    d->m_lineStyle->unpack( stream );
    d->m_polyStyle->unpack( stream );
}

}
