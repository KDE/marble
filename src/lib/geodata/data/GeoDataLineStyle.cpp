//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataLineStyle.h"

class GeoDataLineStylePrivate
{
  public:
    GeoDataLineStylePrivate()
    {
    }

    ~GeoDataLineStylePrivate()
    {
    }

    /// The current width of the line
    float  m_width;
};

GeoDataLineStyle::GeoDataLineStyle()
    : d (new GeoDataLineStylePrivate )
{
}

GeoDataLineStyle::GeoDataLineStyle( const QColor &color )
    : d (new GeoDataLineStylePrivate )
{
    setColor( color );
}

GeoDataLineStyle::~GeoDataLineStyle()
{
    delete d;
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
}

void GeoDataLineStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );
}
