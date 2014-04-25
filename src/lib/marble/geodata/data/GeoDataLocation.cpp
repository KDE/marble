//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataLocation.h"
#include "GeoDataTypes.h"
#include "GeoDataCoordinates.h"

namespace Marble {

class GeoDataLocationPrivate
{
public:

    GeoDataCoordinates m_coordinates;

    GeoDataLocationPrivate();
};

GeoDataLocationPrivate::GeoDataLocationPrivate() :
    m_coordinates()
{
    // nothing to do
}

GeoDataLocation::GeoDataLocation() : d( new GeoDataLocationPrivate )
{
    // nothing to do
}

GeoDataLocation::GeoDataLocation( const Marble::GeoDataLocation &other ) :
    GeoDataObject( other ), d( new GeoDataLocationPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataLocation &GeoDataLocation::operator=( const GeoDataLocation &other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}


bool GeoDataLocation::operator==( const GeoDataLocation &other ) const
{
    return equals(other) &&
           d->m_coordinates == other.d->m_coordinates;
}

bool GeoDataLocation::operator!=( const GeoDataLocation &other ) const
{
    return !this->operator==( other );
}

GeoDataLocation::~GeoDataLocation()
{
    delete d;
}

const char *GeoDataLocation::nodeType() const
{
    return GeoDataTypes::GeoDataLocationType;
}

qreal GeoDataLocation::altitude() const
{
    return d->m_coordinates.altitude();
}

void GeoDataLocation::setAltitude(qreal altitude)
{

    d->m_coordinates.setAltitude(altitude);
}

qreal GeoDataLocation::latitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_coordinates.latitude(unit);
}

void GeoDataLocation::setLatitude(qreal latitude, GeoDataCoordinates::Unit unit)
{
    d->m_coordinates.setLatitude(latitude, unit);
}

qreal GeoDataLocation::longitude( GeoDataCoordinates::Unit unit ) const
{
    return d->m_coordinates.longitude(unit);
}

void GeoDataLocation::setLongitude(qreal longitude, GeoDataCoordinates::Unit unit)
{
    d->m_coordinates.setLongitude(longitude, unit);
}


}
