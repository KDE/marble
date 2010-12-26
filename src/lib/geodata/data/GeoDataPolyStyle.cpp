//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataPolyStyle.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataPolyStylePrivate
{
  public:
    GeoDataPolyStylePrivate()
     : m_fill( true ), m_outline( true )
    {
    }

    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataPolyStyleType;
    }

    /// whether to fill the polygon
    bool  m_fill;
    /// whether to draw the outline
    bool  m_outline;
};

GeoDataPolyStyle::GeoDataPolyStyle()
    : d( new GeoDataPolyStylePrivate )
{
}

GeoDataPolyStyle::GeoDataPolyStyle( const GeoDataPolyStyle& other )
    : GeoDataColorStyle( other ), d( new GeoDataPolyStylePrivate( *other.d ) )
{
}

GeoDataPolyStyle::GeoDataPolyStyle( const QColor &color )
    : d( new GeoDataPolyStylePrivate )
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

const char* GeoDataPolyStyle::nodeType() const
{
    return d->nodeType();
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
