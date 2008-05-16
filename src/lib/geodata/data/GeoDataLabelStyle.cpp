//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataLabelStyle.h"

class GeoDataLabelStylePrivate
{
  public:
    GeoDataLabelStylePrivate()
    {
    }

    GeoDataLabelStylePrivate( const QFont &font )
        : m_scale( 1.0 ),
          m_alignment( GeoDataLabelStyle::Corner ),
          m_font( font )
    {
    }

    ~GeoDataLabelStylePrivate()
    {
    }

    /// The current scale of the label
    float  m_scale;
    /// The current alignment of the label
    GeoDataLabelStyle::Alignment m_alignment;
    /// The current font of the label
    QFont  m_font;   // Not a KML property
};

GeoDataLabelStyle::GeoDataLabelStyle()
    : d (new GeoDataLabelStylePrivate )
{
}

GeoDataLabelStyle::GeoDataLabelStyle( const QFont &font, const QColor &color )
    : d (new GeoDataLabelStylePrivate( font ) )
{
    setColor( color );
}

GeoDataLabelStyle::~GeoDataLabelStyle()
{
    delete d;
}

void GeoDataLabelStyle::setAlignment( GeoDataLabelStyle::Alignment alignment )
{
    d->m_alignment = alignment;
}

GeoDataLabelStyle::Alignment GeoDataLabelStyle::alignment() const
{
    return d->m_alignment;
}

void GeoDataLabelStyle::setScale( const float &scale )
{
    d->m_scale = scale;
}

float GeoDataLabelStyle::scale() const
{
    return d->m_scale;
}

void GeoDataLabelStyle::setFont( const QFont &font )
{
    d->m_font = font;
}

QFont GeoDataLabelStyle::font() const
{
    return d->m_font;
}


void GeoDataLabelStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );
}

void GeoDataLabelStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );
}
