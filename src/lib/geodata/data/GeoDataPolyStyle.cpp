//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataPolyStyle.h"

namespace Marble
{

class GeoDataPolyStylePrivate
{
  public:
    GeoDataPolyStylePrivate()
     : m_fill( true ), m_outline( true )
    {
    }

    GeoDataPolyStylePrivate( const GeoDataPolyStylePrivate& other )
     : m_fill( other.m_fill ), m_outline( other.m_outline )
    {
    }

    ~GeoDataPolyStylePrivate()
    {
    }

    /// whether to fill the polygon
    bool  m_fill;
    /// whether to draw the outline
    bool  m_outline;
};

GeoDataPolyStyle::GeoDataPolyStyle()
    : d (new GeoDataPolyStylePrivate )
{
}

GeoDataPolyStyle::GeoDataPolyStyle( const GeoDataPolyStyle& other )
    : GeoDataColorStyle( other ), d (new GeoDataPolyStylePrivate( *other.d ) )
{
}

GeoDataPolyStyle::GeoDataPolyStyle( const QColor &color )
    : d (new GeoDataPolyStylePrivate )
{
    setColor( color );
}

GeoDataPolyStyle::~GeoDataPolyStyle()
{
    delete d;
}

GeoDataPolyStyle& GeoDataPolyStyle::operator=( const GeoDataPolyStyle& other )
{
    GeoDataColorStyle::operator=( other );
    *d = *other.d;
    return *this;
}

void GeoDataPolyStyle::setFill( const bool &fill )
{
    d->m_fill = fill;
}

bool GeoDataPolyStyle::fill() const
{
    return d->m_fill;
}

void GeoDataPolyStyle::setOutline( const bool &outline )
{
    d->m_outline = outline;
}

bool GeoDataPolyStyle::outline() const
{
    return d->m_outline;
}

void GeoDataPolyStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );
    
    stream << d->m_fill;
    stream << d->m_outline;
}

void GeoDataPolyStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );
    
    stream >> d->m_fill;
    stream >> d->m_outline;
}

}
