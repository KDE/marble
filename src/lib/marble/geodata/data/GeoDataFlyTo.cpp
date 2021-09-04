// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
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
    m_duration( 0.0 ), m_flyToMode(), m_view( nullptr )
{

}

GeoDataFlyTo::GeoDataFlyTo() : d( new GeoDataFlyToPrivate )
{

}

GeoDataFlyTo::GeoDataFlyTo( const Marble::GeoDataFlyTo &other ) :
    GeoDataTourPrimitive( other ), d( new GeoDataFlyToPrivate( *other.d ) )
{

}

GeoDataFlyTo &GeoDataFlyTo::operator=( const GeoDataFlyTo &other )
{
    GeoDataTourPrimitive::operator=( other );
    *d = *other.d;
    return *this;
}

bool GeoDataFlyTo::operator==( const GeoDataFlyTo& other ) const
{
    if ( !equals(other) ||
         d->m_duration != other.d->m_duration ||
         d->m_flyToMode != other.d->m_flyToMode ) {
        return false;
    }

    if ( (!d->m_view && other.d->m_view) ||
         (d->m_view && !other.d->m_view) ) {
        return false;
    } else if ( !d->m_view && !other.d->m_view ) {
        return true;
    }

    if (*d->m_view != *other.d->m_view) {
        return false;
    }

    return true;
}

bool GeoDataFlyTo::operator!=( const GeoDataFlyTo& other ) const
{
    return !this->operator==(other);
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
