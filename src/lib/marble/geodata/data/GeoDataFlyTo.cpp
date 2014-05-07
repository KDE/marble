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
#include "GeoDataCamera.h"
#include "GeoDataLookAt.h"

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

    if ( d->m_view->nodeType() != other.d->m_view->nodeType() ) {
        return false;
    }

    if ( d->m_view->nodeType() == GeoDataTypes::GeoDataCameraType ) {
        GeoDataCamera *thisCam = dynamic_cast<GeoDataCamera*>( d->m_view );
        GeoDataCamera *otherCam = dynamic_cast<GeoDataCamera*>( other.d->m_view );
        Q_ASSERT( thisCam && otherCam );

        if ( *thisCam != *otherCam ) {
            return false;
        }
    } else if ( d->m_view->nodeType() == GeoDataTypes::GeoDataLookAtType ) {
        GeoDataLookAt *thisLookAt = dynamic_cast<GeoDataLookAt*>( d->m_view );
        GeoDataLookAt *otherLookAt = dynamic_cast<GeoDataLookAt*>( other.d->m_view );
        Q_ASSERT( thisLookAt && otherLookAt );

        if ( *thisLookAt != *otherLookAt ) {
            return false;
        }
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
