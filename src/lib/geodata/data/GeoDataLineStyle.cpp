//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataLineStyle.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataLineStylePrivate
{
  public:
    GeoDataLineStylePrivate() : m_width( 1.0 )
    {
    }

    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataLineStyleType;
    }

    /// The current width of the line
    float  m_width;
};

GeoDataLineStyle::GeoDataLineStyle()
    : d (new GeoDataLineStylePrivate )
{
}

GeoDataLineStyle::GeoDataLineStyle( const GeoDataLineStyle& other )
    : GeoDataColorStyle( other ), d (new GeoDataLineStylePrivate( *other.d ) )
{
}

GeoDataLineStyle::GeoDataLineStyle( const QColor &color )
    : d ( new GeoDataLineStylePrivate )
{
    setColor( color );
}

GeoDataLineStyle::~GeoDataLineStyle()
{
    delete d;
}

GeoDataLineStyle& GeoDataLineStyle::operator=( const GeoDataLineStyle& other )
{
    GeoDataColorStyle::operator=( other );
    *d = *other.d;
    return *this;
}

const char* GeoDataLineStyle::nodeType() const
{
    return d->nodeType();
}

void GeoDataLineStyle::setWidth( const float &width )
{
    d->m_width = width;
}

float GeoDataLineStyle::width() const
{
    return d->m_width;
}

void GeoDataLineStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );
    
    stream << d->m_width;
}

void GeoDataLineStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );
    
    stream >> d->m_width;
}

}
