//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//                     Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataScale.h"
#include "GeoDataTypes.h"

namespace Marble {

class GeoDataScalePrivate
{
public:
    double m_x;

    double m_y;

    double m_z;

    GeoDataScalePrivate();
};

GeoDataScalePrivate::GeoDataScalePrivate() :
    m_x(1), m_y(1), m_z(1)
{
    // nothing to do
}

GeoDataScale::GeoDataScale() : d( new GeoDataScalePrivate )
{
    // nothing to do
}

GeoDataScale::GeoDataScale( const Marble::GeoDataScale &other ) :
    GeoDataObject( other ), d( new GeoDataScalePrivate( *other.d ) )
{
    // nothing to do
}

GeoDataScale &GeoDataScale::operator=( const GeoDataScale &other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}


bool GeoDataScale::operator==( const GeoDataScale &other ) const
{
    return equals(other) &&
           d->m_x == other.d->m_x &&
           d->m_y == other.d->m_y &&
           d->m_z == other.d->m_z;
}

bool GeoDataScale::operator!=( const GeoDataScale &other ) const
{
    return !this->operator==( other );
}

GeoDataScale::~GeoDataScale()
{
    delete d;
}

const char *GeoDataScale::nodeType() const
{
    return GeoDataTypes::GeoDataScaleType;
}

double GeoDataScale::x() const
{
    return d->m_x;
}

void GeoDataScale::setX( double x )
{
    d->m_x = x;
}

double GeoDataScale::y() const
{
    return d->m_y;
}

void GeoDataScale::setY( double y )
{
    d->m_y = y;
}

double GeoDataScale::z() const
{
    return d->m_z;
}

void GeoDataScale::setZ( double z )
{
    d->m_z = z;
}

}
