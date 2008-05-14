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
class GeoDataStylePrivate
{
  public:
    GeoDataStylePrivate()
        : m_iconStyle( 0 ),
          m_labelStyle( 0 )
    {
    }

    GeoDataStylePrivate(const QPixmap& icon, 
                        const QFont &font, const QColor &color )
        : m_iconStyle( new GeoDataIconStyle( icon ) ),
          m_labelStyle( new GeoDataLabelStyle( font, color ) )
    {
    }


    ~GeoDataStylePrivate()
    {
        delete m_labelStyle;
        delete m_iconStyle;
    }

    GeoDataIconStyle   *m_iconStyle;
    GeoDataLabelStyle  *m_labelStyle;
    // LineStyle
    // PolyStyle
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

GeoDataIconStyle* GeoDataStyle::iconStyle()
{
    return d->m_iconStyle;
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
}

void GeoDataStyle::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );

    d->m_iconStyle->unpack( stream );
    d->m_labelStyle->unpack( stream );
}
