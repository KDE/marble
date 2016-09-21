//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//


#include "GeoDataCamera.h"
#include "GeoDataCamera_p.h"

#include "GeoDataTypes.h"

#include <QDataStream>

namespace Marble
{

GeoDataCamera::GeoDataCamera() :
    GeoDataAbstractView(),
    d( new GeoDataCameraPrivate )
{
}

GeoDataCamera::GeoDataCamera( const GeoDataCamera& other ) :
    GeoDataAbstractView(),
    d( other.d )
{
    d->ref.ref();
}

GeoDataCamera& GeoDataCamera::operator=( const GeoDataCamera &other )
{
    GeoDataAbstractView::operator=( other );
    qAtomicAssign( d, other.d );
    return *this;
}

bool GeoDataCamera::operator==( const GeoDataCamera &other ) const
{
    return equals(other) &&
           d->m_coordinates == other.d->m_coordinates &&
           d->m_roll == other.d->m_roll &&
           d->m_heading == other.d->m_heading &&
           d->m_tilt == other.d->m_tilt &&
           altitudeMode() == other.altitudeMode();
}

bool GeoDataCamera::operator!=( const GeoDataCamera &other ) const
{
    return !this->operator==(other);
}

GeoDataCamera::~GeoDataCamera()
{
    if( !d->ref.deref() ) {
        delete d;
    }
}

GeoDataAbstractView *GeoDataCamera::copy() const
{
    return new GeoDataCamera( *this );
}

void GeoDataCamera::setCoordinates( const GeoDataCoordinates& coordinates )
{
    detach();
    d->m_coordinates = coordinates;
}

const char* GeoDataCamera::nodeType() const
{
    return GeoDataTypes::GeoDataCameraType;
}

void GeoDataCamera::setAltitude( qreal altitude )
{
    detach();
    d->m_coordinates.setAltitude( altitude );
}

qreal GeoDataCamera::altitude() const
{
    return d->m_coordinates.altitude();
}

void GeoDataCamera::setLatitude( qreal latitude, GeoDataCoordinates::Unit unit )
{
    detach();
    d->m_coordinates.setLatitude( latitude, unit );
}

qreal GeoDataCamera::latitude( GeoDataCoordinates::Unit unit ) const
{
    return d->m_coordinates.latitude( unit );
}

void GeoDataCamera::setLongitude( qreal longitude, GeoDataCoordinates::Unit unit )
{
    detach();
    d->m_coordinates.setLongitude( longitude, unit );
}

qreal GeoDataCamera::longitude( GeoDataCoordinates::Unit unit ) const
{
    return d->m_coordinates.longitude( unit );
}

GeoDataCoordinates GeoDataCamera::coordinates() const
{
    return d->m_coordinates;
}

void GeoDataCamera::setRoll(qreal roll)
{
    detach();
    d->m_roll = roll;
}

qreal GeoDataCamera::roll() const
{
    return d->m_roll;
}

qreal GeoDataCamera::heading() const
{
    return d->m_heading;
}

void GeoDataCamera::setHeading(qreal heading)
{
    detach();
    d->m_heading = heading;
}

qreal GeoDataCamera::tilt() const
{
    return d->m_tilt;
}

void GeoDataCamera::setTilt(qreal tilt)
{
    detach();
    d->m_tilt = tilt;
}

void GeoDataCamera::detach()
{
    qAtomicDetach( d );
}

}
