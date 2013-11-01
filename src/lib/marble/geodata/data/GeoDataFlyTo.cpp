//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include "GeoDataFlyTo.h"
#include "GeoDataTypes.h"
#include "GeoDataAbstractView.h"

namespace Marble {

class GeoDataFlyToPrivate
{
public:
    double m_duration;

    GeoDataFlyTo::FlyToMode m_flyToMode;

    GeoDataAbstractView* m_view;

    GeoDataFlyToPrivate();
};

GeoDataFlyToPrivate::GeoDataFlyToPrivate() :
    m_duration( 0.0 ), m_flyToMode(), m_view( 0 )
{

}

GeoDataFlyTo::GeoDataFlyTo() : d( new GeoDataFlyToPrivate )
{

}

GeoDataFlyTo::GeoDataFlyTo( const Marble::GeoDataFlyTo &other ) :
    GeoDataObject( other ), d( new GeoDataFlyToPrivate( *other.d ) )
{

}

GeoDataFlyTo &GeoDataFlyTo::operator=( const GeoDataFlyTo &other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}

GeoDataFlyTo::~GeoDataFlyTo()
{
    delete d;
}

const char *GeoDataFlyTo::nodeType() const
{
    return GeoDataTypes::GeoDataFlyToType;
}

const GeoDataAbstractView *GeoDataFlyTo::view() const
{
    return d->m_view;
}

GeoDataAbstractView *GeoDataFlyTo::view()
{
    return d->m_view;
}

void GeoDataFlyTo::setView( GeoDataAbstractView *view )
{
    d->m_view = view;
}

double GeoDataFlyTo::duration() const
{
    return d->m_duration;
}

void GeoDataFlyTo::setDuration( double duration )
{
    d->m_duration = duration;
}

GeoDataFlyTo::FlyToMode GeoDataFlyTo::flyToMode() const
{
    return d->m_flyToMode;
}

void GeoDataFlyTo::setFlyToMode( const GeoDataFlyTo::FlyToMode flyToMode )
{
    d->m_flyToMode = flyToMode;
}

}
